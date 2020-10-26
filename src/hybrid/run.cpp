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
#include <hybrid/hybrid_execution.hpp>
#include <fstream>

#include "benchmarks/tpch/Queries.hpp"
#include "common/runtime/Import.hpp"
#include "profile.hpp"
#include "tbb/tbb.h"

#include "hybrid/code_generator.hpp"
#include "hybrid/compilation_engine.hpp"
#include "hybrid/hybrid_exception.hpp"
#include "hybrid/hybrid_execution.hpp"

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

//std::ofstream output("output.txt");
//    std::streambuf* p_cerrbuffer=std::cerr.rdbuf();
//    std::cerr.rdbuf(output.rdbuf()); // redirecting to a file

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
//	std::cout<<"Importing TPCH"<<std::endl;
   importTPCH(argv[2], tpch);
//	std::cout<<"Imported"<<std::endl;
   // run queries
   auto repetitions = atoi(argv[1]); //No. of iterations
   size_t nrThreads =
//1;
16;
//32;
//std::thread::hardware_concurrency();

   size_t vectorSize = 1024; // Pre-defined vectorsize, can be overwritten
   bool clearCaches = false;
   bool verbose = false;
//   if (argc > 3) { nrThreads = atoi(argv[3]); }
   if (argc > 4) { verbose = atoi(argv[4]); }
    std::unordered_set<std::string> q = {
//          "1h",
//           "1v",
           "1hv"
//	   "6h",
//           "6h",
//           "6hv"
//           "18v",
//	   "18h",
//           "18hv"
//           "3v",
//	   "3h"
//           "3hv"
   };
    std::cout<<"Running with "<<nrThreads<<" parallel threads"<<std::endl;
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
   if (q.count("1h")) {
      try {
	std::cout<<"q1,";
         // generate Typer code for Q1
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generatePureTyperQ1();

         // compile llvm
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

          auto start = gettime();
          // link library
          const std::string& path_to_lib =
                  hybrid::CompilationEngine::instance().linkQueryLib(
                          path_to_ll, useLLVM);
          auto end =  gettime();
          if (verbose) {
              std::cout
                      << "Compilation took "
                      << (end-start) * 1e3
                      << " milliseconds." << std::endl;
          }
          if (clearCaches) clearOsCaches();

          // run experiments
//         e.timeAndProfile(
  //           "q1 hyper     ", nrTuples(tpch, {"lineitem"}),
  //           [&]() {
                auto result = q1_hyper(tpch, nrThreads);
		auto anotherResult = q1_hyper_LLVM(tpch,path_to_ll, nrThreads);
                escape(&result);
  //           },
  //           repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }

   if (q.count("1v")) {
      e.timeAndProfile("q1 vectorwise", nrTuples(tpch, {"lineitem"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result =
                              q1_vectorwise(tpch, nrThreads, vectorSize);
                          escape(&result);
                       },
                       repetitions);
   }

   if (q.count("1hv")) {
      try {
         // generate Typer code for Q1
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generateHybridTyperQ1();

         // compile llvm
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

         // run experiments
//         e.timeAndProfile("q1 hybrid    ", nrTuples(tpch, {"lineitem"}),
//                          [&]() {
                             if (clearCaches) clearOsCaches();
                             auto result =
                                 q1_hybrid(tpch, nrThreads, vectorSize,
                                           path_to_ll, useLLVM, verbose);
                             escape(&result);
//                          },
//                          repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }

   if (q.count("1hv1")) {
      try {
         // generate Typer code for Q1
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generateHybridPartialTyperQ1();

         // compile llvm
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

         // run experiments
         e.timeAndProfile("q1 hybrid    ", nrTuples(tpch, {"lineitem"}),
                          [&]() {
                             if (clearCaches) clearOsCaches();
                             auto result =
                                 q1_hybridPartial(tpch, nrThreads, vectorSize,
                                           path_to_ll, useLLVM, verbose);
                             escape(&result);
                          },
                          repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }

   // Q6
   if (q.count("6h")) {
      try {
	std::cout<<"q6";
         // generate Typer code for Q6
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generatePureTyperQ6();

         // compile LLVM
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

          auto start = std::chrono::steady_clock::now();
          // link library
          const std::string& path_to_lib =
                  hybrid::CompilationEngine::instance().linkQueryLib(
                          path_to_ll, useLLVM);
          auto end = std::chrono::steady_clock::now();
          if (verbose) {
              std::cout
                      << "Compilation took "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(
                              end - start)
                              .count()
                      << " milliseconds." << std::endl;
          }

          // run experiments
         //e.timeAndProfile(
           //  "q6 hyper     ", tpch["lineitem"].nrTuples,
           //  [&]() {
                if (clearCaches) clearOsCaches();
                auto result = q6_hyper(tpch, nrThreads);
	        auto anotherResult = q6_hyper_LLVM(tpch, path_to_ll, nrThreads);
                escape(&result);
//             },
//             repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }

   if (q.count("6v")) {
       if (clearCaches) clearOsCaches();
      e.timeAndProfile("q6 vectorwise", tpch["lineitem"].nrTuples,
                       [&]() {
                          auto result =
                              q6_vectorwise(tpch, nrThreads, vectorSize);
                          escape(&result);
                       },
                       repetitions);
   }

    /*
     * Q6 is executed with tectorwise until the end of query pipeline. The thread for tectorwise is interuppted and tper gets executed once the compilation is done
     * Here, any point during interupt, tectorwise would have partially generated the results for the query and typer finishes the result set by processing the remaining input
     */

   if (q.count("6hv")) {
      try {
         // generate Typer code for Q6
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generateHybridTyperQ6();

         // compile LLVM
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

         // run experiments
//         e.timeAndProfile("q6 hybrid    ", tpch["lineitem"].nrTuples,
//                          [&]() {
                             if (clearCaches) clearOsCaches();
                             auto result =
                                 q6_hybrid(tpch, nrThreads, vectorSize,
                                           path_to_ll, useLLVM, verbose);
                             escape(&result);
//                          },
//                          repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }

    //Q18
   if (q.count("18h")) {
        std::cout<<"q18";

      try {
         // generate Typer code for Q18
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generatePureTyperQ18();

         // compile llvm
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

          auto start = std::chrono::steady_clock::now();
          // link library
          const std::string& path_to_lib =
                  hybrid::CompilationEngine::instance().linkQueryLib(
                          path_to_ll, useLLVM);
          auto end = std::chrono::steady_clock::now();
          if (verbose) {
              std::cout
                      << "Compilation took "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(
                              end - start)
                              .count()
                      << " milliseconds." << std::endl;
          }

          // run experiments
//         e.timeAndProfile(
//             "q18 hyper     ",
//             nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
//             [&]() {
                if (clearCaches) clearOsCaches();
                auto result = q18_hyper(tpch, nrThreads);
	        auto anotherResult = q18_hyper_LLVM(tpch, path_to_ll, nrThreads);
                escape(&result);
//             },
//             repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }



   if (q.count("18v")) {
      e.timeAndProfile(
          "q18 vectorwise",
          nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
          [&]() {
             if (clearCaches) clearOsCaches();
             auto result = q18_vectorwise(tpch, nrThreads, vectorSize);
             escape(&result);
          },
          repetitions);
   }

   if (q.count("18hv")) {

      try {
         // generate Typer code for Q18
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generateHybridTyperQ18();

         // compile llvm
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

          const std::string& path_to_cpp_new =
                  hybrid::CodeGenerator::instance().generateHybridAnotherTyperQ18();

          // compile llvm
          const std::string& path_to_ll_new =
                  hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp_new,
                                                                        useLLVM);

                std::cout<<"running q18"<<std::endl;
                if (clearCaches) clearOsCaches();
                auto result = q18_hybrid(tpch, nrThreads, vectorSize,
                                         path_to_ll_new, useLLVM, verbose);

//                if (clearCaches) clearOsCaches();
//                result = q18_another_hybrid(tpch, nrThreads, vectorSize,
//                                   path_to_ll_new, useLLVM, verbose);

                escape(&result);
      } catch (hybrid::HybridException& exc) {
         std::cout << exc.what() << std::endl;
      }
   }
    if (q.count("3v"))
        e.timeAndProfile(
                "q3 vectorwise", nrTuples(tpch, {"customer", "orders", "lineitem"}),
                [&]() {
                    if (clearCaches) clearOsCaches();
                    auto result = q3_vectorwise(tpch, nrThreads, vectorSize);
                    escape(&result);
                },
                repetitions);

   if (q.count("3h")) {
        std::cout<<"q3";
      try {
         // generate Typer code for Q18
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generatePureTyperQ3();

         // compile llvm
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

          auto start = std::chrono::steady_clock::now();
          // link library
          const std::string& path_to_lib =
                  hybrid::CompilationEngine::instance().linkQueryLib(
                          path_to_ll, useLLVM);
          auto end = std::chrono::steady_clock::now();
          if (verbose) {
              std::cout
                      << "Compilation took "
                      << std::chrono::duration_cast<std::chrono::milliseconds>(
                              end - start)
                              .count()
                      << " milliseconds." << std::endl;
          }

         // run experiments
//         e.timeAndProfile(
//             "q3 hyper     ",
//             nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
//             [&]() {
                if (clearCaches) clearOsCaches();
                auto result = q3_hyper(tpch, nrThreads);
	        auto anotherResult = q3_hyper_LLVM(tpch, path_to_ll, nrThreads);
                escape(&result);
  //           },
    //         repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }

    if (q.count("3hv")) {
        try {
            // generate Typer code for Q3
            const std::string& path_to_cpp =
                    hybrid::CodeGenerator::instance().generateHybridAnotherTyperQ3();

            // compile llvm
            bool useLLVM = true;
            const std::string& path_to_ll =
                    hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                          useLLVM);

            const std::string& path_to_cpp_old =
                    hybrid::CodeGenerator::instance().generateHybridTyperQ3();

            // compile llvm
            const std::string& path_to_ll_old =
                    hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp_old,
                                                                          useLLVM);

            const std::string& path_to_cpp_new =
                    hybrid::CodeGenerator::instance().generateHybridYetAnotherTyperQ3();

            // compile llvm
            const std::string& path_to_ll_new =
                    hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp_new,
                                                                          useLLVM);
            // run experiments
                        if (clearCaches) clearOsCaches();
                        auto result = q3_hybrid_1(tpch, nrThreads, vectorSize,
                                                path_to_ll, useLLVM, verbose);
                        escape(&result);
                        if (clearCaches) clearOsCaches();
                         result = q3_hybrid(tpch, nrThreads, vectorSize,
                                                path_to_ll_old, useLLVM, verbose);
                        escape(&result);

			if (clearCaches) clearOsCaches();
                         result = q3_hybrid_2(tpch, nrThreads, vectorSize,
                                                path_to_ll_new, useLLVM, verbose);
                        escape(&result);

        } catch (hybrid::HybridException& exc) {
            std::cout<<"There is some error on the processing"<<std::endl;
            std::cout<< exc.what() << std::endl;
        }
    }

    /**
     * hybrid execution using hybirdExecution class
     */

//    if (q.count("3hv")) {
//        try {
//            // generate Typer code for Q3
//            const std::string& path_to_cpp =
//                    hybrid::CodeGenerator::instance().generateHybridTyperQ3();
//
//            // compile llvm
//            /* Compilation parameter to pass */
//            bool useLLVM = true;
//            const std::string& path_to_ll =
//                    hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                          useLLVM); //Compiles CPP into LLVM code | this is outside the execution time
//                                                                                   //Final codegen will directly prepare code from IR to LLVM
//            /* TectorWise parameter to pass */
//            size_t tectorwiseTuples = nrTuples(tpch, {"customer"});
//
//            vectorwise::SharedStateManager shared;
//            Q3Builder builder(tpch, shared, vectorSize);
//            auto query = builder.getQuery();
//            std::unique_ptr<vectorwise::ResultWriter> printOp(
//                    static_cast<vectorwise::ResultWriter*>(query->rootOp.release()));
//            std::unique_ptr<vectorwise::HashGroup> finalAggregates(
//                    static_cast<vectorwise::HashGroup*>(printOp->child.release()));
//            std::unique_ptr<vectorwise::Project> projectExpression(
//                    static_cast<vectorwise::Project*>(finalAggregates->child.release()));
//            std::unique_ptr<vectorwise::Hashjoin> lineItemHashJoin(
//                    static_cast<vectorwise::Hashjoin*>(projectExpression->child.release()));
//            std::unique_ptr<vectorwise::Hashjoin> CustOrdHashJoin(
//                    static_cast<vectorwise::Hashjoin*>(lineItemHashJoin->left.release())); //parameter to pass
//
//            hybrid::connector type = hybrid::connector::hash_join;
//
//            /* Connection parameter required */
//            const std::string& LLVMfuncName = "_Z15hybrid_typer_q3RN7runtime8DatabaseEmRNS_7HashmapEm";
//
//            // run experiments
//            e.timeAndProfile(
//                    "q3 hybrid   ",
//                    tectorwiseTuples,
//                    [&]() {
//                        if (clearCaches) clearOsCaches();
//                        hybrid::HybridExecution execute;
//                        auto result =
//                                execute.compile_and_execute_hash_join(
//                                        tpch, nrThreads, verbose,
//                                        path_to_ll, useLLVM,
//                                        tectorwiseTuples, vectorSize, CustOrdHashJoin, LLVMfuncName
//                                );
////                                exit(0);
//                        escape(&result);
//                    },
//                    repetitions);
//        } catch (hybrid::HybridException& exc) {
//            std::cout<<"There is some error on the processing"<<std::endl;
//            std::cerr << exc.what() << std::endl;
//        }
//    }
//
//    if (q.count("1hv")) {
//        try {
//            // generate Typer code for Q1
//            const std::string& path_to_cpp =
//                    hybrid::CodeGenerator::instance().generateHybridTyperQ1();
//
//            // compile llvm
//            /* Compilation parameter to pass */
//            bool useLLVM = true;
//            const std::string& path_to_ll =
//                    hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
//                                                                          useLLVM); //Compiles CPP into LLVM code | this is outside the execution time
//            //Final codegen will directly prepare code from IR to LLVM
//            /* TectorWise parameter to pass */
//            size_t tectorwiseTuples = nrTuples(tpch, {"lineitem"});
//
//            vectorwise::SharedStateManager shared;
//            Q1Builder builder(tpch, shared, vectorSize);
//            auto query = builder.getQuery();
//            std::unique_ptr<vectorwise::ResultWriter> printOp(
//                    static_cast<vectorwise::ResultWriter*>(query->rootOp.release()));
//            std::unique_ptr<vectorwise::HashGroup> groupOp(
//                    static_cast<vectorwise::HashGroup*>(printOp->child.release()));
//
//            hybrid::connector type = hybrid::connector::hash_join;
//
//            /* Connection parameter required */
//            const std::string& LLVMfuncName = "_Z15hybrid_typer_q1RN7runtime8DatabaseEmmRSt13unordered_mapINSt6thread2idENS_16PartitionedDequeILm1024EEESt4hashIS4_ESt8equal_toIS4_ESaISt4pairIKS4_S6_EEE";
//
//            // run experiments
//            e.timeAndProfile(
//                    "q1 hybrid   ",
//                    tectorwiseTuples,
//                    [&]() {
//                        if (clearCaches) clearOsCaches();
//                        hybrid::HybridExecution execute;
//                        auto result =
//                                execute.compile_and_execute_hash_group(
//                                        tpch, nrThreads, verbose,
//                                        path_to_ll, useLLVM,
//                                        tectorwiseTuples, vectorSize, groupOp, LLVMfuncName
//                                );
//                        escape(&result);
//
//                    },
//                    repetitions);
//        } catch (hybrid::HybridException& exc) {
//            std::cout<<"There is some error on the processing"<<std::endl;
//            std::cerr << exc.what() << std::endl;
//        }
//    }

    scheduler.terminate();
   return 0;

}
