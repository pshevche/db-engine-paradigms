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

    template<class T>
    std::unique_ptr<runtime::Query> HybridExecution::compile_and_execute(   Database& db, //Pointer to data
                                size_t nrThreads,bool verbose,
                                const std::string& path_to_lib_src, bool fromLLVM, // Compilation parameter
                                size_t nrTuples,size_t vectorSize, std::unique_ptr<T>& vwOperator, //TectorWise parameter
                                hybrid::connector type, const std::string& LLVMfuncName //Connection parameter
    ){

        using namespace vectorwise;
        using namespace std::chrono_literals;

        /**
         * Typer compilation - Required variable from outside
         * const string& path_to_lib - src path
         * bool fromLLVM - switches whether to commile using LLVM or NOT (currently it is always comiled in LLVM)
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

        vectorwise::SharedStateManager shared;
        std::atomic<size_t> processedTuples;
        switch(type){

            case hybrid::connector::hash_join : //Executing hash join
                                                break;

            case hybrid::connector::hash_aggregate :    //Execute hash Aggregate
                                                        break;

            case hybrid::connector::hash_join_and_aggregate :   //Execute hash join and aggregate
                                                                break;
        }

        compilationThread.join();
        auto start = std::chrono::steady_clock::now();

        //load typer library
        if(!typerLib){

            throw hybrid::HybridException("Could not load shared Typer library!");
        }

        /**
         * Connection - Required variable and values
         * const string& funcName
         */
         //TODO - IMPORTANT - find right data type
        auto typer_query = typerLib.load()->getFunction<hybrid::GenericHashJoin>(LLVMfuncName);

        switch(type){

            case hybrid::connector::hash_join : typer_query =//here-> the compiled typer function definition has to be given properly
                                                        typerLib.load()->getFunction<hybrid::GenericHashJoin>(LLVMfuncName);
                                                break;

            case hybrid::connector::hash_aggregate :   typer_query = //here-> the compiled typer function definition has to be given properly
                        typerLib.load()->getFunction<hybrid::GenericHashGroup>(LLVMfuncName);//Execute hash Aggregate
                break;

            case hybrid::connector::hash_join_and_aggregate :// typer_query = //here-> the compiled typer function definition has to be given properly
                            //typerLib.load()->getFunction<hybrid::GenericHashJoinGroup>(LLVMfuncName);   //Execute hash join and aggregate
                break;
        }

        //loading function into the runtime


        if(!typer_query) {
            throw hybrid::HybridException(
                    "Could not find function for running Query in Typer!");
        }

        //get final computed result
        std::unique_ptr<runtime::Query> result = std::move(typer_query(
                db,
                nrThreads,
                shared.get<Hashjoin::Shared>(0).ht,
                processedTuples.load()
        ));
        delete typerLib;

        return; //result

    }
}