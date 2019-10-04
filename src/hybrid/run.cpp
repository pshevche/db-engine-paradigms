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
#include <fstream>

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


void redirectError(){
//        Re-directing errors to a error file
    std::ofstream output("DispatcherErrorLog.txt");
    std::streambuf* p_cerrbuffer=std::cerr.rdbuf();
    std::cerr.rdbuf(output.rdbuf());
}


void print_typer_source_code(){

    std::cout<< "#include \"hybrid/minimal_api.hpp\"\n"
                  "using namespace runtime;"
                  "using namespace std;"
                  "using namespace types;"
                  "std::unique_ptr<runtime::Query> compiled_typer_q18("
                  "Database & db, size_t nrThreads, runtime::Hashmap & twCustomerHT,"
                  "std::unordered_map<int32_t, defs::hash_t>& twHashFunction,"
                  "std::unordered_map<std::thread::id, runtime::PartitionedDeque<1024>>& "
                  "twAggrRes, "
                  "size_t firstTuple) {"
                  "using namespace types;"
                  "using namespace std;"
                  "using hash = runtime::MurMurHash;"
                  "auto resources = initQuery(nrThreads);"
                  "auto& li = db[\"lineitem\"];"
                  "auto l_orderkey = li[\"l_orderkey\"].data<types::Integer>();"
                  "auto l_quantity = li[\"l_quantity\"].data<types::Numeric<12, 2>>();"
                  "tbb::enumerable_thread_specific<"
                  "Hashmapx<types::Integer, types::Numeric<12, 2>, hash, false>>"
                  "groups;"
                  "const auto zero = types::Numeric<12, 2>::castString(\"0.00\");"
                  "auto groupOp = make_GroupBy<types::Integer, types::Numeric<12, 2>, "
                  "hash>("
                  "[](auto& acc, auto&& value) { acc += value; }, zero, nrThreads);"
                  "\n// scan lineitem and group by l_orderkey\n"
                  "tbb::parallel_for("
                  "tbb::blocked_range<size_t>(firstTuple, li.nrTuples, morselSize),"
                  "[&](const tbb::blocked_range<size_t>& r) {"
                  "auto locals = groupOp.preAggLocals();"
                  "for (size_t i = r.begin(), end = r.end(); i != end; ++i) {"
                  "auto& group = locals.getGroup(l_orderkey[i]);"
                  "group += l_quantity[i];"
                  "\n// locals.consume(l_orderkey[i], l_quantity[i]);\n"
                  "}"
                  "});"
                  "\n// merge tw's partial thread-local hash tables with typer's "
                  "partial\n"
                  "\n// thread-local hash tables\n"
                  "tbb::parallel_for_each("
                  "twAggrRes.begin(), twAggrRes.end(), [&](auto& threadPartitions) {"
                  "auto locals = groupOp.preAggLocals();"
                  "tbb::parallel_for_each("
                  "threadPartitions.second.getPartitions().begin(),"
                  "threadPartitions.second.getPartitions().end(),"
                  "[&](auto& partition) {"
                  "for (auto chunk = partition.first; chunk;"
                  "chunk = chunk->next) {"
                  "auto elementSize = threadPartitions.second.entrySize;"
                  "auto nPart = partition.size(chunk, elementSize);"
                  "auto data ="
                  "chunk->template data<hybrid::Q18TWAggrTuple>();"
                  "for (unsigned i = 0; i < nPart; ++i) {"
                  "hybrid::Q18TWAggrTuple t = data[i];"
                  "types::Integer key = types::Integer(t.l_orderkey);"
                  "types::Numeric<12, 2> value ="
                  "types::Numeric<12, 2>(t.l_quantity);"
                  "auto& group = locals.getGroup(key);"
                  "group += value;"
                  "}"
                  "}"
                  "});"
                  "});"
                  "Hashset<types::Integer, hash> ht1;"
                  "tbb::enumerable_thread_specific<runtime::Stack<decltype(ht1)::Entry>>"
                  "entries1;"
                  "const auto threeHundret = types::Numeric<12, 2>::castString(\"300\");"
                  "std::atomic<size_t> nrGroups;"
                  "nrGroups = 0;"
                  "groupOp.forallGroups([&](auto& groups) {"
                  "auto& entries = entries1.local();"
                  "size_t groupsFound = 0;"
                  "for (auto block : groups)"
                  "for (auto& group : block)"
                  "if (group.v > threeHundret) {"
                  "entries.emplace_back(ht1.hash(group.k), group.k);"
                  "groupsFound++;"
                  "}"
                  "\n// TODO: reconsider this way of counting groups\n"
                  "nrGroups.fetch_add(groupsFound);"
                  "});"
                  "ht1.setSize(nrGroups);"
                  "parallel_insert(entries1, ht1);"
                  "\n// build last hashtable\n"
                  "Hashmapx<types::Integer,"
                  "std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,"
                  "types::Char<25>>,"
                  "hash>"
                  "ht3;"
                  "tbb::enumerable_thread_specific<runtime::Stack<decltype(ht3)::Entry>>"
                  "entries3;"
                  "auto& ord = db[\"orders\"];"
                  "auto o_orderkey = ord[\"o_orderkey\"].data<types::Integer>();"
                  "auto o_custkey = ord[\"o_custkey\"].data<types::Integer>();"
                  "auto o_orderdate = ord[\"o_orderdate\"].data<types::Date>();"
                  "auto o_totalprice ="
                  "ord[\"o_totalprice\"].data<types::Numeric<12, 2>>();"
                  "\n// scan orders\n"
                  "\n// typer ht is used only for hashing\n"
                  "auto found = PARALLEL_SELECT(ord.nrTuples, entries3, {"
                  "char* name;"
                  "\n// check if it matches the order criteria and look up the "
                  "customer name\n"
                  "if (ht1.contains(o_orderkey[i])) {"
                  "auto hash = twHashFunction[o_custkey[i].value];"
                  "for (auto entry = twCustomerHT.find_chain_tagged(hash);"
                  "entry != runtime::Hashmap::end();"
                  "entry = reinterpret_cast<runtime::Hashmap::EntryHeader*>("
                  "entry->next)) {"
                  "auto t = reinterpret_cast<hybrid::Q18TWJoinTuple*>(entry);"
                  "if (types::Integer(t->c_custkey) == o_custkey[i]) {"
                  "name = t->c_name;"
                  "}"
                  "}"
                  "if (name != nullptr) {"
                  "entries.emplace_back(ht3.hash(o_orderkey[i]), o_orderkey[i],"
                  "make_tuple(o_custkey[i], o_orderdate[i],"
                  "o_totalprice[i], types::Char<25>::build(name)));"
                  "found++;"
                  "}"
                  "}"
                  "});"
                  "ht3.setSize(found);"
                  "parallel_insert(entries3, ht3);"
                  "auto finalGroupOp = make_GroupBy<"
                  "std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,"
                  "types::Char<25>, types::Integer>,"
                  "types::Numeric<12, 2>, hash>("
                  "[](auto& acc, auto&& value) { acc += value; }, zero, nrThreads);"
                  "\n// scan lineitem and group by l_orderkey\n"
                  "tbb::parallel_for("
                  "tbb::blocked_range<size_t>(0, li.nrTuples, morselSize),"
                  "[&](const tbb::blocked_range<size_t>& r) {"
                  "auto locals = finalGroupOp.preAggLocals();"
                  "for (size_t i = r.begin(), end = r.end(); i != end; ++i) {"
                  "std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,"
                  "types::Char<25>>* v;"
                  "if ((v = ht3.findOne(l_orderkey[i]))) {"
                  "auto& group ="
                  "locals.getGroup(tuple_cat(*v, make_tuple(l_orderkey[i])));"
                  "group += l_quantity[i];"
                  "}"
                  "}"
                  "});"
                  "auto& result = resources.query->result;"
                  "auto namAttr = result->addAttribute(\"c_name\", "
                  "sizeof(types::Char<25>));"
                  "auto ckyAttr = result->addAttribute(\"c_custkey\", "
                  "sizeof(types::Integer));"
                  "auto okyAttr = result->addAttribute(\"o_orderkey\", "
                  "sizeof(types::Integer));"
                  "auto datAttr = result->addAttribute(\"o_orderdate\", "
                  "sizeof(types::Date));"
                  "auto totAttr ="
                  "result->addAttribute(\"o_totalprice\", sizeof(types::Numeric<12, "
                  "2>));"
                  "auto sumAttr = result->addAttribute(\"sum\", "
                  "sizeof(types::Numeric<12, 2>));"
                  "finalGroupOp.forallGroups([&](auto& groups) {"
                  "\n// write aggregates to result\n"
                  "auto n = groups.size();"
                  "auto block = result->createBlock(n);"
                  "auto name = reinterpret_cast<types::Char<25>*>(block.data(namAttr));"
                  "auto cky = reinterpret_cast<types::Integer*>(block.data(ckyAttr));"
                  "auto oky = reinterpret_cast<types::Integer*>(block.data(okyAttr));"
                  "auto dat = reinterpret_cast<types::Date*>(block.data(datAttr));"
                  "auto tot = reinterpret_cast<types::Numeric<12, "
                  "2>*>(block.data(totAttr));"
                  "auto sum = reinterpret_cast<types::Numeric<12, "
                  "2>*>(block.data(sumAttr));"
                  "for (auto block : groups)"
                  "for (auto& group : block) {"
                  "auto& k = group.k;"
                  "*name++ = std::get<3>(k);"
                  "*cky++ = get<0>(k);"
                  "*oky++ = get<4>(k);"
                  "*dat++ = get<1>(k);"
                  "*tot++ = get<2>(k);"
                  "*sum++ = group.v;"
                  "}"
                  "block.addedElements(n);"
                  "});"
                  "leaveQuery(nrThreads);"
                  "return move(resources.query);"
                  "}";
    exit(0);
}

int main(int argc, char* argv[]) {

//    print_typer_source_code();
//  redirectError();
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

   std::unordered_set<std::string> q = {"1h",  "1v",  "1hv", "6h",  "6v",
                                        "6hv", "18h", "18v", "18hv", "3hv"};

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
   std::cout<<"Compilation of header done"<<std::endl;
   // Q1
//   if (q.count("1h")) {
//       std::cout<<"Compilation of header done"<<std::endl;
//      e.timeAndProfile("q1 hyper     ", nrTuples(tpch, {"lineitem"}),
//                       [&]() {
//                          if (clearCaches) clearOsCaches();
//                          auto result = q1_hyper(tpch, nrThreads);
//                          escape(&result);
//                       },
//                       repetitions);
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

   if (q.count("1hv")) {
      try {
          std::cout<<"executing hybrid"<<std::endl;
         // generate Typer code for Q1
         const std::string& path_to_cpp =
             hybrid::CodeGenerator::instance().generateHybridTyperQ1();

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
       std::cout<<"1hv done"<<std::endl;
   }

//   // Q6
//   if (q.count("6h")) {
//      e.timeAndProfile("q6 hyper     ", tpch["lineitem"].nrTuples,
//                       [&]() {
//                          if (clearCaches) clearOsCaches();
//                          auto result = q6_hyper(tpch, nrThreads);
//                          escape(&result);
//                       },
//                       repetitions);
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
//
//   // Q18
//   if (q.count("18h")) {
//      e.timeAndProfile(
//          "q18 hyper     ",
//          nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
//          [&]() {
//             if (clearCaches) clearOsCaches();
//             auto result = q18_hyper(tpch, nrThreads);
//             escape(&result);
//          },
//          repetitions);
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
    std::cout<<"Running Q18 hybrid"<<std::endl;
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
         std::cerr << "Error in exec: " << exc.what() << std::endl;
      }
   }

//   // Q18
//   if (q.count("18h")) {
//      e.timeAndProfile(
//          "q18 hyper     ",
//          nrTuples(tpch, {"customer", "lineitem", "orders", "lineitem"}),
//          [&]() {
//             if (clearCaches) clearOsCaches();
//             auto result = q18_hyper(tpch, nrThreads);
//             escape(&result);
//          },
//          repetitions);
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

std::cout<<"Running q3"<<std::endl;
   if (q.count("3hv")) {
      try {
         // generate Typer code for Q18
         const std::string& path_to_cpp =
                 hybrid::CodeGenerator::instance().generateHybridTyperQ3();

         // compile llvm
         bool useLLVM = true;
         const std::string& path_to_ll =
             hybrid::CompilationEngine::instance().compileQueryCPP(path_to_cpp,
                                                                   useLLVM);
          std::cout<<"running q3 experiments"<<std::endl;
         // run experiments
         e.timeAndProfile(
             "q3 hybrid   ",
             nrTuples(tpch, {"customer", "orders"}),
             [&]() {
                if (clearCaches) clearOsCaches();
                auto result = q3_hybrid(tpch, nrThreads, vectorSize,
                                         path_to_ll, useLLVM, verbose);
                escape(&result);
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
