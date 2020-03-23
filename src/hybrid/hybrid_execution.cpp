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

    std::atomic<hybrid::SharedLibrary*> typerLib(nullptr);

    void HybridExecution::compile(const std::string& path_to_lib_src,
                                  bool fromLLVM,
                                  bool verbose) {

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
    }

    void tectorwise_hash_build( std::unique_ptr<Hashjoin> hashBuild,
                                size_t nrTuples,
                                size_t nrThreads,
                                bool verbose){
        /*
         * Starting TectorWise execution
         * build hash table for customer and order relation
         *
         */

        auto start = std::chrono::steady_clock::now();
        WorkerGroup workers(nrThreads);
        vectorwise::SharedStateManager shared;
        std::atomic<size_t> processedTuples(0);

        workers.run([&]() {
            size_t found = 0;
            // --- build phase 1: materialize ht entries
            for (auto n = hashBuild->left->next();
                 n != EndOfStream && !typerLib; n = hashBuild->left->next()) {
                found += n;

                // build hashes
                hashBuild->buildHash.evaluate(n);

                // scatter hash, keys and values into ht entries
                auto alloc = runtime::this_worker->allocator.allocate(
                        n * hashBuild->ht_entry_size);
                if (!alloc) throw std::runtime_error("malloc failed");

                hashBuild->allocations.push_back(std::make_pair(alloc, n));
                hashBuild->scatterStart =
                        reinterpret_cast<decltype(hashBuild->scatterStart)>(alloc);
                hashBuild->buildScatter.evaluate(n);

            }

            // --- build phase 2: insert ht entries
            hashBuild->shared.found.fetch_add(found);
            barrier([&]() {
                auto globalFound = hashBuild->shared.found.load();

                if (globalFound) hashBuild->shared.ht.setSize(globalFound);
            });
            auto globalFound = hashBuild->shared.found.load();
            if (globalFound != 0) {
                insertAllEntries(hashBuild->allocations, hashBuild->shared.ht,
                                 hashBuild->ht_entry_size);
            }
            barrier(); // wait for all threads to finish build phase
        });

        auto end = std::chrono::steady_clock::now();
        if(verbose){

            std::cout   << "TW took "
                        <<  std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                        <<" milliseconds to process "<< processedTuples.load()
                        << " tuples."<<std::endl;
        }

    }

    void HybridExecution::compute_hybrid(   Database& db,
                                            size_t nrThreads,
                                            size_t vectorSize,
                                            const std::string& path_to_src,
                                            bool fromLLVM,

                                            std::unique_ptr<Hashjoin> hashBuild,
                                            size_t nrTuples,
                                            bool verbose) {

        /*
         * call compilation
         */
        compile(path_to_src,fromLLVM,verbose);

        /*
         * call tectorwise hashing
         */
        hybrid::tectorwise_hash_build(
                nrTuples,
                nrThreads,
                verbose)
        /*
         * combine them together
         */
    }
}