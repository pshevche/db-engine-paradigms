#pragma once

#include <string>
#include "vectorwise/Operators.hpp"
#include "shared_library.hpp"

namespace hybrid {

using namespace vectorwise;
using namespace std::chrono_literals;
using namespace runtime;
// Custom exception for issues caused by hybrid approach
    class HybridExecution{

    public:
        std::unique_ptr<runtime::Hashmap>* hash_build;

        void compile(const std::string& path_to_lib_src,
                     bool fromLLVM,
                     bool verbose);

        void tectorwise_hash_build(std::unique_ptr<Hashjoin> hashBuild, size_t nrTuples);
        void tectorwise_hash_aggregate();
        void compute_hybrid(Database& db,
                            size_t nrThreads,
                            size_t vectorSize,
                            const std::string& path_to_src,
                            bool fromLLVM,

                            std::unique_ptr<Hashjoin> hashBuild,
                            size_t nrTuples,
                            bool verbose);
    };
}