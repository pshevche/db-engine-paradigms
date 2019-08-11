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
   auto repetitions = atoi(argv[1]);
   size_t nrThreads = std::thread::hardware_concurrency();
   size_t vectorSize = 1024;
   bool clearCaches = false;
   bool verbose = false;
   if (argc > 3) { nrThreads = atoi(argv[3]); }
   if (argc > 4) { verbose = atoi(argv[4]); }

   std::unordered_set<std::string> q = {"18hv"};

   if (auto v = std::getenv("vectorSize")) vectorSize = atoi(v);
   if (auto v = std::getenv("SIMDhash")) conf.useSimdHash = atoi(v);
   if (auto v = std::getenv("SIMDjoin")) conf.useSimdJoin = atoi(v);
   if (auto v = std::getenv("SIMDsel")) conf.useSimdSel = atoi(v);
   if (auto v = std::getenv("SIMDproj")) conf.useSimdProj = atoi(v);
   if (auto v = std::getenv("clearCaches")) clearCaches = atoi(v);
   if (auto v = std::getenv("q")) {
      using namespace std;
      istringstream iss((string(v)));
      q.clear();
      copy(istream_iterator<string>(iss), istream_iterator<string>(),
           insert_iterator<decltype(q)>(q, q.begin()));
   }

   tbb::task_scheduler_init scheduler(nrThreads);

   // precompile header for all evaluations
   hybrid::CompilationEngine::instance().precompileAPIHeader();

   // Q1
   if (q.count("1h")) {
      e.timeAndProfile("q1 hyper     ", nrTuples(tpch, {"lineitem"}),
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result = q1_hyper(tpch, nrThreads);
                          escape(&result);
                       },
                       repetitions);
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
             hybrid::CodeGenerator::instance().generateTyperQ1();

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
                                 q1_hybrid(tpch, nrThreads, vectorSize,
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
      e.timeAndProfile("q6 hyper     ", tpch["lineitem"].nrTuples,
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result = q6_hyper(tpch, nrThreads);
                          escape(&result);
                       },
                       repetitions);
   }

   if (q.count("6v")) {
      e.timeAndProfile("q6 vectorwise", tpch["lineitem"].nrTuples,
                       [&]() {
                          if (clearCaches) clearOsCaches();
                          auto result =
                              q6_vectorwise(tpch, nrThreads, vectorSize);
                          escape(&result);
                       },
                       repetitions);
   }

   if (q.count("6hv")) {
      try {
         // generate Typer code for Q6
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generateTyperQ6();

         // compile LLVM
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

         // run experiments
         e.timeAndProfile("q6 hybrid    ", tpch["lineitem"].nrTuples,
                          [&]() {
                             if (clearCaches) clearOsCaches();
                             auto result =
                                 q6_hybrid(tpch, nrThreads, vectorSize,
                                           path_to_ll, useLLVM, verbose);
                             escape(&result);
                          },
                          repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }

   // Q18
   if (q.count("18h")) {
      e.timeAndProfile(
          "q18 hyper     ",
          nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
          [&]() {
             if (clearCaches) clearOsCaches();
             auto result = q18_hyper(tpch, nrThreads);
             escape(&result);
          },
          repetitions);
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
             hybrid::CodeGenerator::instance().generateTyperQ18();

         // compile llvm
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);

         // run experiments
         e.timeAndProfile(
             "q18 hybrid   ",
             nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
             [&]() {
                if (clearCaches) clearOsCaches();
                auto result = q18_hybrid(tpch, nrThreads, vectorSize,
                                         path_to_ll, useLLVM, verbose);
                escape(&result);
             },
             repetitions);
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   }

   scheduler.terminate();
   return 0;
}
