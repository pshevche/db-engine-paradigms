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
        template <class T>
        std::unique_ptr<runtime::Query> compile_and_execute(   Database& db, //Pointer to data
                                    size_t nrThreads,
                                    bool verbose,
                                    const std::string& path_to_lib_src, bool fromLLVM, // Compilation parameter
                                    size_t nrTuples,size_t vectorSize, std::unique_ptr<T>& vwOperator, //TectorWise parameter
                                    hybrid::connector type, const std::string& LLVMfuncName //Connection parameter
                                 );
        void hybrid_join();
        void hybrid_aggregate();
    };
}