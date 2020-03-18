#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <iterator>
#include <sstream>
#include <thread>
#include <unordered_set>

#include "benchmarks/tpch/Queries.hpp"
#include "common/runtime/Import.hpp"
#include "profile.hpp"
#include "tbb/tbb.h"

#include "hybrid/code_generator.hpp"
#include "hybrid/compilation_engine.hpp"
#include "hybrid/hybrid_exception.hpp"

using namespace runtime;

static void escape(void* p) { asm volatile("" : : "g"(p) : "memory"); }

size_t nrTuples(Database& db, std::vector<std::string> tables) {
   size_t sum = 0;
   for (auto& table : tables) sum += db[table].nrTuples;
   return sum;
}

/// Clears Linux page cache.
/// This function only works on Linux.
void clearOsCaches() {
   if (system("sync; echo 3 > /proc/sys/vm/drop_caches")) {
      throw std::runtime_error("Could not flush system caches: " +
                               std::string(std::strerror(errno)));
   }
}

int main(int argc, char* argv[]) {
   if (argc <= 2) {
      std::cerr
          << "Usage: ./" << argv[0]
          << "<number of repetitions> <path to tpch dir> [nrThreads = all] \n "
             " EnvVars: [vectorSize = 1024] [SIMDhash = 0] [SIMDjoin = 0] "
             "[SIMDsel = 0]";
      exit(1);
   }

   PerfEvents e;
   Database tpch;
   // load tpch data
   importTPCH(argv[2], tpch);

   // run queries
   auto repetitions = atoi(argv[1]); //No. of iterations
   size_t nrThreads = std::thread::hardware_concurrency();
   size_t vectorSize = 1024; // Pre-defined vectorsize, can be overwritten
   bool clearCaches = false;
   bool verbose = false;
   if (argc > 3) { nrThreads = atoi(argv[3]); }
   if (argc > 4) { verbose = atoi(argv[4]); }
    std::unordered_set<std::string> q = {"1h",  "1v",  "1hv", "6h",  "6v",
                                        "6hv", "18h", "18v", "18hv", "3v", "3h", "3hv"};

   //Configures other environment variables
   if (auto v = std::getenv("vectorSize")) vectorSize = atoi(v); //overwrites vectorsize in case available in command-line
   if (auto v = std::getenv("SIMDhash")) conf.useSimdHash = atoi(v);
   if (auto v = std::getenv("SIMDjoin")) conf.useSimdJoin = atoi(v);
   if (auto v = std::getenv("SIMDsel")) conf.useSimdSel = atoi(v);
   if (auto v = std::getenv("SIMDproj")) conf.useSimdProj = atoi(v);
   if (auto v = std::getenv("clearCaches")) clearCaches = atoi(v);
   if (auto v = std::getenv("q")) {
      using namespace std;
      istringstream iss(( string(v)));
      q.clear();
      copy(istream_iterator<string>(iss), istream_iterator<string>(),
           insert_iterator<decltype(q)>(q, q.begin()));
   }

   tbb::task_scheduler_init scheduler(nrThreads);

   // precompile header for all evaluations
   hybrid::CompilationEngine::instance().precompileAPIHeader();

   // Q1
//   if (q.count("1h")) {
//      try {
//         // generate Typer code for Q1
//         const std::string& path_to_cpp =
//             hybrid::CodeGenerator::instance().generatePureTyperQ1();
//
//         // compile llvm
//         bool useLLVM = true;
//         const std::string& path_to_ll =
//             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                   useLLVM);
//
//         // run experiments
//         e.timeAndProfile(
//             "q1 hyper     ", nrTuples(tpch, {"lineitem"}),
//             [&]() {
//                auto start = std::chrono::steady_clock::now();
//                // link library
//                const std::string& path_to_lib =
//                    hybrid::CompilationEngine::instance().linkQueryLib(
//                        path_to_ll, useLLVM);
//                auto end = std::chrono::steady_clock::now();
//                if (verbose) {
//                   std::cout
//                       << "Compilation took "
//                       << std::chrono::duration_cast<std::chrono::milliseconds>(
//                              end - start)
//                              .count()
//                       << " milliseconds." << std::endl;
//                }
//                if (clearCaches) clearOsCaches();
//                auto result = q1_hyper(tpch, nrThreads);
//                escape(&result);
//             },
//             repetitions);
//      } catch (hybrid::HybridException& exc) {
//         std::cerr << exc.what() << std::endl;
//      }
//   }
//
//   if (q.count("1v")) {
//      e.timeAndProfile("q1 vectorwise", nrTuples(tpch, {"lineitem"}),
//                       [&]() {
//                          if (clearCaches) clearOsCaches();
//                          auto result =
//                              q1_vectorwise(tpch, nrThreads, vectorSize);
//                          escape(&result);
//                       },
//                       repetitions);
//   }
//
//   if (q.count("1hv")) {
//      try {
//         // generate Typer code for Q1
//         const std::string& path_to_cpp =
//             hybrid::CodeGenerator::instance().generateHybridTyperQ1();
//
//         // compile llvm
//         bool useLLVM = true;
//         const std::string& path_to_ll =
//             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                   useLLVM);
//
//         // run experiments
//         e.timeAndProfile("q1 hybrid    ", nrTuples(tpch, {"lineitem"}),
//                          [&]() {
//                             if (clearCaches) clearOsCaches();
//                             auto result =
//                                 q1_hybrid(tpch, nrThreads, vectorSize,
//                                           path_to_ll, useLLVM, verbose);
//                             escape(&result);
//                          },
//                          repetitions);
//      } catch (hybrid::HybridException& exc) {
//         std::cerr << exc.what() << std::endl;
//      }
//   }
//
//   // Q6
//   if (q.count("6h")) {
//      try {
//         // generate Typer code for Q6
//         const std::string& path_to_cpp =
//             hybrid::CodeGenerator::instance().generatePureTyperQ6();
//
//         // compile LLVM
//         bool useLLVM = true;
//         const std::string& path_to_ll =
//             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                   useLLVM);
//
//         // run experiments
//         e.timeAndProfile(
//             "q6 hyper     ", tpch["lineitem"].nrTuples,
//             [&]() {
//                auto start = std::chrono::steady_clock::now();
//                // link library
//                const std::string& path_to_lib =
//                    hybrid::CompilationEngine::instance().linkQueryLib(
//                        path_to_ll, useLLVM);
//                auto end = std::chrono::steady_clock::now();
//                if (verbose) {
//                   std::cout
//                       << "Compilation took "
//                       << std::chrono::duration_cast<std::chrono::milliseconds>(
//                              end - start)
//                              .count()
//                       << " milliseconds." << std::endl;
//                }
//                if (clearCaches) clearOsCaches();
//                auto result = q6_hyper(tpch, nrThreads);
//                escape(&result);
//             },
//             repetitions);
//      } catch (hybrid::HybridException& exc) {
//         std::cerr << exc.what() << std::endl;
//      }
//   }
//
//   if (q.count("6v")) {
//      e.timeAndProfile("q6 vectorwise", tpch["lineitem"].nrTuples,
//                       [&]() {
//                          if (clearCaches) clearOsCaches();
//                          auto result =
//                              q6_vectorwise(tpch, nrThreads, vectorSize);
//                          escape(&result);
//                       },
//                       repetitions);
//   }
//
    /*
     * Q6 is executed with tectorwise until the end of query pipeline. The thread for tectorwise is interuppted and tper gets executed once the compilation is done
     * Here, any point during interupt, tectorwise would have partially generated the results for the query and typer finishes the result set by processing the remaining input
     */
//   if (q.count("6hv")) {
//      try {
//         // generate Typer code for Q6
//         const std::string& path_to_cpp =
//             hybrid::CodeGenerator::instance().generateHybridTyperQ6();
//
//         // compile LLVM
//         bool useLLVM = true;
//         const std::string& path_to_ll =
//             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                   useLLVM);
//
//         // run experiments
//         e.timeAndProfile("q6 hybrid    ", tpch["lineitem"].nrTuples,
//                          [&]() {
//                             if (clearCaches) clearOsCaches();
//                             auto result =
//                                 q6_hybrid(tpch, nrThreads, vectorSize,
//                                           path_to_ll, useLLVM, verbose);
//                             escape(&result);
//                          },
//                          repetitions);
//      } catch (hybrid::HybridException& exc) {
//         std::cerr << exc.what() << std::endl;
//      }
//   }
//
   // Q18
//   if (q.count("18h")) {
//      try {
//         // generate Typer code for Q18
//         const std::string& path_to_cpp =
//             hybrid::CodeGenerator::instance().generateHybridTyperQ18();
//
//         // compile llvm
//         bool useLLVM = true;
//         const std::string& path_to_ll =
//             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                   useLLVM);
//
//         // run experiments
//         e.timeAndProfile(
//             "q18 hyper     ",
//             nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
//             [&]() {
//                auto start = std::chrono::steady_clock::now();
//                // link library
//                const std::string& path_to_lib =
//                    hybrid::CompilationEngine::instance().linkQueryLib(
//                        path_to_ll, useLLVM);
//                auto end = std::chrono::steady_clock::now();
//                if (verbose) {
//                   std::cout
//                       << "Compilation took "
//                       << std::chrono::duration_cast<std::chrono::milliseconds>(
//                              end - start)
//                              .count()
//                       << " milliseconds." << std::endl;
//                }
//                if (clearCaches) clearOsCaches();
//                auto result = q18_hyper(tpch, nrThreads);
//                escape(&result);
//             },
//             repetitions);
//      } catch (hybrid::HybridException& exc) {
//         std::cerr << exc.what() << std::endl;
//      }
//   }


//
//   if (q.count("18v")) {
//      e.timeAndProfile(
//          "q18 vectorwise",
//          nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
//          [&]() {
//             if (clearCaches) clearOsCaches();
//             auto result = q18_vectorwise(tpch, nrThreads, vectorSize);
//             escape(&result);
//          },
//          repetitions);
//   }
//
//   if (q.count("18hv")) {
//
//      try {
//         // generate Typer code for Q18
//         const std::string& path_to_cpp =
//             hybrid::CodeGenerator::instance().generateHybridTyperQ18();
//
//         // compile llvm
//         bool useLLVM = true;
//         const std::string& path_to_ll =
//             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                   useLLVM);
//         std::cout<<"After compilation"<<std::endl;
//         // run experiments
//         e.timeAndProfile(
//             "q18 hybrid   ",
//             nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
//             [&]() {
//                if (clearCaches) clearOsCaches();
//                auto result = q18_hybrid(tpch, nrThreads, vectorSize,
//                                         path_to_ll, useLLVM, verbose);
//                escape(&result);
//             },
//             repetitions);
//      } catch (hybrid::HybridException& exc) {
//         std::cerr << exc.what() << std::endl;
//      }
//   }
   std::cout<<"Executing Q3"<<std::endl;

//
//    if (q.count("3v"))
//        e.timeAndProfile(
//                "q3 vectorwise", nrTuples(tpch, {"customer", "orders", "lineitem"}),
//                [&]() {
//                    if (clearCaches) clearOsCaches();
//                    auto result = q3_vectorwise(tpch, nrThreads, vectorSize);
//                    escape(&result);
//                },
//                repetitions);

//   if (q.count("3h")) {
//      try {
//         // generate Typer code for Q18
//         const std::string& path_to_cpp =
//             hybrid::CodeGenerator::instance().generateHybridTyperQ3();
//
//         // compile llvm
//         bool useLLVM = true;
//         const std::string& path_to_ll =
//             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                   useLLVM);
//
//         // run experiments
//         e.timeAndProfile(
//             "q3 hyper     ",
//             nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
//             [&]() {
//                auto start = std::chrono::steady_clock::now();
//                // link library
//                const std::string& path_to_lib =
//                    hybrid::CompilationEngine::instance().linkQueryLib(
//                        path_to_ll, useLLVM);
//                auto end = std::chrono::steady_clock::now();
//                if (verbose) {
//                   std::cout
//                       << "Compilation took "
//                       << std::chrono::duration_cast<std::chrono::milliseconds>(
//                              end - start)
//                              .count()
//                       << " milliseconds." << std::endl;
//                }
//                if (clearCaches) clearOsCaches();
//                auto result = q3_hyper(tpch, nrThreads);
//                escape(&result);
//             },
//             repetitions);
//      } catch (hybrid::HybridException& exc) {
//         std::cerr << exc.what() << std::endl;
//      }
//   }

    if (q.count("3v")) {
        auto result = q3_vectorwise(tpch, nrThreads, vectorSize);
        escape(&result);
    }

    if (q.count("3h")) {
        try {

            auto result = q3_hyper(tpch, nrThreads);
            escape(&result);
        } catch (hybrid::HybridException& exc) {
            std::cerr << exc.what() << std::endl;
        }
    }

    if (q.count("3hv")) {
        try {
            // generate Typer code for Q3
            const std::string& path_to_cpp =
                    hybrid::CodeGenerator::instance().generateHybridTyperQ3();

            // compile llvm
            bool useLLVM = true;
            const std::string& path_to_ll =
                    hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                          useLLVM);
            // run experiments
            e.timeAndProfile(
                    "q3 hybrid   ",
                    nrTuples(tpch, {"customer", "orders"}),
                    [&]() {
                        if (clearCaches) clearOsCaches();
                        auto result = q3_hybrid(tpch, nrThreads, vectorSize,
                                                path_to_ll, useLLVM, verbose);
                        escape(&result);
                        exit(0);
                    },
                    repetitions);
        } catch (hybrid::HybridException& exc) {
            std::cout<<"There is some error on the processing"<<std::endl;
            std::cerr << exc.what() << std::endl;
        }
    }

   scheduler.terminate();
   return 0;
}