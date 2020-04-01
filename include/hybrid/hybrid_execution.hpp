#pragma once

#include <string>
#include "vectorwise/Operators.hpp"
#include "shared_library.hpp"

namespace hybrid {

enum connector {hash_join, hash_aggregate, hash_join_and_aggregate};

using namespace vectorwise;
using namespace std::chrono_literals;
using namespace runtime;
// Custom exception for issues caused by hybrid approach
    class HybridExecution{

    public:

        std::unique_ptr<runtime::Query>
                compile_and_execute_hash_join(Database &db, size_t nrThreads, bool verbose,
                                              const std::string &path_to_lib_src, bool fromLLVM,
                                              size_t nrTuples, size_t vectorSize, std::unique_ptr<Hashjoin> &vwOperator,
                                              const std::string &LLVMfuncName);

        std::unique_ptr<runtime::Query>
        compile_and_execute_hash_group(Database &db, size_t nrThreads, bool verbose,
                                      const std::string &path_to_lib_src, bool fromLLVM,
                                      size_t nrTuples, size_t vectorSize, std::unique_ptr<vectorwise::HashGroup> &vwOperator,
                                      const std::string &LLVMfuncName);
    };
}