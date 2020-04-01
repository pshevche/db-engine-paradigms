#pragma once

#include "hybrid/hybrid_execution.hpp"

#include "benchmarks/tpch/Queries.hpp"
#include "common/runtime/Hash.hpp"
#include "common/runtime/Types.hpp"
#include "hyper/GroupBy.hpp"
#include "hyper/ParallelHelper.hpp"
#include "tbb/tbb.h"
#include "vectorwise/Operations.hpp"
#include "vectorwise/Operators.hpp"
#include "vectorwise/Primitives.hpp"
#include "vectorwise/QueryBuilder.hpp"
#include "vectorwise/VectorAllocator.hpp"
#include <iostream>
#include "hybrid/compilation_engine.hpp"
#include "hybrid/hybrid_datatypes.hpp"
#include "hybrid/hybrid_exception.hpp"
#include "hybrid/shared_library.hpp"

#include <string>

namespace hybrid {

    size_t num_Tuples(Database& db, std::vector<std::string> tables) {
        size_t sum = 0;
        for (auto& table : tables) sum += db[table].nrTuples;
        return sum;
    }

    void HybridExecution::hybrid_join(){

        std::cout<<"execute hash join"<<std::endl;
        return;
    }

    std::unique_ptr<runtime::Query>
        HybridExecution::compile_and_execute_hash_join(Database &db, size_t nrThreads, bool verbose,
                                                       const std::string &path_to_lib_src,
                                                       bool fromLLVM, size_t nrTuples, size_t vectorSize,
                                                       std::unique_ptr<vectorwise::Hashjoin> &vwOperator,
                                                       const std::string &LLVMfuncName) {

        using namespace std;
        using namespace vectorwise;
        using namespace std::chrono_literals;

        /**
         * Typer compilation - Required variable from outside
         * const string& path_to_lib - src path
         * bool fromLLVM - switches whether to compile using LLVM or NOT (currently it is always comiled in LLVM)
         */

        std::atomic<hybrid::SharedLibrary*> typerLib(nullptr);
        std::thread compilationThread([&typerLib, &path_to_lib_src, &fromLLVM,
                                              &verbose] {
            try {
                auto start = std::chrono::steady_clock::now();
                // link library
                const std::string& path_to_lib =
                        hybrid::CompilationEngine::instance().linkQueryLib(path_to_lib_src,
                                                                           fromLLVM);
                // open library
                typerLib = hybrid::SharedLibrary::load(path_to_lib + ".so");
                auto end = std::chrono::steady_clock::now();
                if (verbose) {
                    std::cout << "Compilation took "
                              << std::chrono::duration_cast<std::chrono::milliseconds>(
                                      end - start)
                                      .count()
                              << " milliseconds." << std::endl;
                }
            } catch (hybrid::HybridException& exc) {
                std::cout<<"error in compilation"<<std::endl;
                std::cerr << exc.what() << std::endl;
            }
        });

        /*
         * TODO - Execute TectorWise
         *
         * Tectorwise - Required variables and values
         * size_t nrThreads - no of threads to be used
         * size_t  nrTuples - no of tuples to initialized
         * unique_ptr< Hash_Join | Hash_Group > connector - unique_ptr to hook vectorwise with hyper
         * unique_ptr<Qx> query - ptr to the target query to run (optional)
         * -- these are the results pipelined into typer execution --
         * vectorwise::SharedStateManager shared - this result will be provided to compiled execution
         * std::atomic<size_t> processedTuples - this provides how much has been completed in vectorwise
         *
         */

        auto start = std::chrono::steady_clock::now();

        WorkerGroup workers(nrThreads);
        vectorwise::SharedStateManager shared;
        std::atomic<size_t> processedTuples(0);

        /*
         * Performing generic hash join
         */
        workers.run([&]() {
            {
                size_t found = 0;
                // --- build phase 1: materialize ht entries
                for (auto n = vwOperator->left->next();
                     n != EndOfStream && !typerLib; n = vwOperator->left->next()) {
                    found += n;

                    // build hashes
                    vwOperator->buildHash.evaluate(n);

                    // scatter hash, keys and values into ht entries
                    auto alloc = runtime::this_worker->allocator.allocate(
                            n * vwOperator->ht_entry_size);
                    if (!alloc) throw std::runtime_error("malloc failed");

                    vwOperator->allocations.push_back(std::make_pair(alloc, n));
                    vwOperator->scatterStart =
                            reinterpret_cast<decltype(vwOperator->scatterStart)>(alloc);
                    vwOperator->buildScatter.evaluate(n);

                }

                // --- build phase 2: insert ht entries
                vwOperator->shared.found.fetch_add(found);
                barrier([&]() {
                    auto globalFound = vwOperator->shared.found.load();

                    if (globalFound) vwOperator->shared.ht.setSize(globalFound);
                });
                auto globalFound = vwOperator->shared.found.load();
                if (globalFound != 0) {
                    insertAllEntries(vwOperator->allocations, vwOperator->shared.ht,
                                     vwOperator->ht_entry_size);
                }
                barrier(); // wait for all threads to finish build phase
            }
        });

        processedTuples.fetch_add(vwOperator->shared.found.load());
        auto end = std::chrono::steady_clock::now();
        if(verbose){

            std::cout   << "TW took "
                        <<  std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                        <<" milliseconds to process "<< processedTuples.load()
                        << " tuples."<<std::endl;
        }

        compilationThread.join();
        start = std::chrono::steady_clock::now();

        //load typer library
        if(!typerLib)
            throw hybrid::HybridException("Could not load shared Typer library!");


        /**
         * Connection - Required variable and values
         * const string& funcName
         */
         //TODO - IMPORTANT - find right data type
        auto typer_query = typerLib.load()->getFunction<hybrid::GenericHashJoin>(LLVMfuncName);


        if(!typer_query) {
            throw hybrid::HybridException(
                    "Could not find function for running Query in Typer!");
        }

        //get final computed result
        std::unique_ptr<runtime::Query> result = std::move(typer_query(
                db,
                nrThreads,
                vwOperator->shared.ht,
                processedTuples.load()
        ));
        delete typerLib;
        return result; //result
    }

    std::unique_ptr<runtime::Query>
    compile_and_execute_hash_group(Database &db, size_t nrThreads, bool verbose,
                                   const std::string &path_to_lib_src, bool fromLLVM,
                                   size_t nrTuples, size_t vectorSize, std::unique_ptr<vectorwise::HashGroup> &vwOperator,
                                   const std::string &LLVMfuncName){

        using namespace std;
        using namespace vectorwise;
        using namespace std::chrono_literals;

        /**
         * Typer compilation - Required variable from outside
         * const string& path_to_lib - src path
         * bool fromLLVM - switches whether to compile using LLVM or NOT (currently it is always comiled in LLVM)
         */

        std::atomic<hybrid::SharedLibrary*> typerLib(nullptr);
        std::thread compilationThread([&typerLib, &path_to_lib_src, &fromLLVM,
                                              &verbose] {
            try {
                auto start = std::chrono::steady_clock::now();
                // link library
                const std::string& path_to_lib =
                        hybrid::CompilationEngine::instance().linkQueryLib(path_to_lib_src,
                                                                           fromLLVM);
                // open library
                typerLib = hybrid::SharedLibrary::load(path_to_lib + ".so");
                auto end = std::chrono::steady_clock::now();
                if (verbose) {
                    std::cout << "Compilation took "
                              << std::chrono::duration_cast<std::chrono::milliseconds>(
                                      end - start)
                                      .count()
                              << " milliseconds." << std::endl;
                }
            } catch (hybrid::HybridException& exc) {
                std::cout<<"error in compilation"<<std::endl;
                std::cerr << exc.what() << std::endl;
            }
        });

        /*
         * TODO - Execute TectorWise
         *
         * Tectorwise - Required variables and values
         * size_t nrThreads - no of threads to be used
         * size_t  nrTuples - no of tuples to initialized
         * unique_ptr< Hash_Join | Hash_Group > connector - unique_ptr to hook vectorwise with hyper
         * unique_ptr<Qx> query - ptr to the target query to run (optional)
         * -- these are the results pipelined into typer execution --
         * vectorwise::SharedStateManager shared - this result will be provided to compiled execution
         * std::atomic<size_t> processedTuples - this provides how much has been completed in vectorwise
         *
         */

        auto start = std::chrono::steady_clock::now();

        WorkerGroup workers(nrThreads);
        vectorwise::SharedStateManager shared;
        std::atomic<size_t> processedTuples(0);

        /*
         * Performing generic hash group
         */


        auto end = std::chrono::steady_clock::now();
        if(verbose){

            std::cout   << "TW took "
                        <<  std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                        <<" milliseconds to process "<< processedTuples.load()
                        << " tuples."<<std::endl;
        }

        compilationThread.join();
        start = std::chrono::steady_clock::now();

        //load typer library
        if(!typerLib)
            throw hybrid::HybridException("Could not load shared Typer library!");


        /**
         * Connection - Required variable and values
         * const string& funcName
         */
        //TODO - IMPORTANT - find right data type
        auto typer_query = typerLib.load()->getFunction<hybrid::GenericHashJoin>(LLVMfuncName);


        if(!typer_query) {
            throw hybrid::HybridException(
                    "Could not find function for running Query in Typer!");
        }

        //get final computed result
        std::unique_ptr<runtime::Query> result = std::move(typer_query(
                db,
                nrThreads,
                vwOperator->shared.ht,
                processedTuples.load()
        ));
        delete typerLib;
        return result; //result
    }
}