#include "benchmarks/tpch/Queries.hpp"
#include "common/runtime/Barrier.hpp"
#include "common/runtime/Hash.hpp"
#include "common/runtime/Query.hpp"
#include "common/runtime/Stack.hpp"
#include "common/runtime/Types.hpp"
#include "hybrid/compilation_engine.hpp"
#include "hybrid/hybrid_datatypes.hpp"
#include "hybrid/hybrid_exception.hpp"
#include "hybrid/shared_library.hpp"
#include "hyper/GroupBy.hpp"
#include "hyper/ParallelHelper.hpp"
#include "tbb/tbb.h"
#include "vectorwise/Operations.hpp"
#include "vectorwise/Operators.hpp"
#include "vectorwise/Primitives.hpp"
#include "vectorwise/QueryBuilder.hpp"
#include "vectorwise/VectorAllocator.hpp"

#include <iostream>
#include <unordered_map>

using namespace runtime;
using namespace std;

// -- TPC-H Query 18
// select
// c_name,
//   c_custkey,
//   o_orderkey,
//   o_orderdate,
//   o_totalprice,
//   sum(l_quantity)
// from
//   customer,
//   orders,
//   lineitem
// where
//   o_orderkey in (
//                  select
//                  l_orderkey
//                  from
//                  lineitem
//                  group by
//                  l_orderkey having
//                  sum(l_quantity) > 300
//                  )
//   and c_custkey = o_custkey
//   and o_orderkey = l_orderkey
// group by
//   c_name,
//   c_custkey,
//   o_orderkey,
//   o_orderdate,
//   o_totalprice

// Display query results
void printResultQ18(runtime::Query* result) {
   // display some results for debugging
   for (auto iter = result->result->begin(); iter != result->result->end();
        ++iter) {
      auto block = *iter;
      int32_t* c_custkey = reinterpret_cast<int32_t*>(
          block.data(result->result->getAttribute("c_custkey")));
      int32_t* o_orderkey = reinterpret_cast<int32_t*>(
          block.data(result->result->getAttribute("o_orderkey")));
      types::Date* o_orderdate = reinterpret_cast<types::Date*>(
          block.data(result->result->getAttribute("o_orderdate")));
      long* o_totalprice = reinterpret_cast<long*>(
          block.data(result->result->getAttribute("o_totalprice")));
      long* sum = reinterpret_cast<long*>(
          block.data(result->result->getAttribute("sum")));
      for (unsigned i = 0; i < block.size(); ++i) {
         std::cout << c_custkey[i] << " | " << o_orderkey[i] << " | "
                   << o_orderdate[i] << " | " << o_totalprice[i] << " | "
                   << sum[i] << std::endl;
      }
   }
}

int32_t extractKeyFromEntry(runtime::Hashmap::EntryHeader* entry) {
   auto t = reinterpret_cast<hybrid::Q18TWJoinTuple*>(entry);
   return t->c_custkey;
}

std::unique_ptr<runtime::Query> q18_hybrid(runtime::Database& db,
                                           size_t nrThreads, size_t vectorSize,
                                           const std::string& path_to_lib_src,
                                           bool fromLLVM, bool verbose) {
   using namespace vectorwise;
   using namespace std::chrono_literals;

   // 1. START COMPILING Q18 IN TYPER
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
         std::cerr << exc.what() << std::endl;
      }
   });

   // 2. WHILE Q18 IS COMPILING, START TECTORWISE
   auto start = std::chrono::steady_clock::now();
   unsigned trackedTables = 2;
   // processed fraction of relations of interest
   std::atomic<size_t>* processedTuples =
       new std::atomic<size_t>[trackedTables];
   for (unsigned i = 0; i < trackedTables; ++i) { processedTuples[i].store(0); }
   size_t* nrTuples = new size_t[trackedTables];
   nrTuples[0] = db["customer"].nrTuples;
   nrTuples[1] = db["lineitem"].nrTuples;

   WorkerGroup workers(nrThreads);
   vectorwise::SharedStateManager shared;

   std::unordered_map<int32_t, defs::hash_t> twHashFunction;

   std::unique_ptr<runtime::Query> result;
   workers.run([&]() {
      using runtime::Hashmap;
      Q18Builder builder(db, shared, vectorSize);
      auto query = builder.getQuery();
      std::unique_ptr<ResultWriter> printOp(
          static_cast<ResultWriter*>(query->rootOp.release()));
      std::unique_ptr<vectorwise::HashGroup> topAggr(
          static_cast<vectorwise::HashGroup*>(printOp->child.release()));
      std::unique_ptr<Hashjoin> topJoin(
          static_cast<Hashjoin*>(topAggr->child.release()));
      std::unique_ptr<Hashjoin> customerJoin(
          static_cast<Hashjoin*>(topJoin->left.release()));
      std::unique_ptr<Hashjoin> lineitemJoin(
          static_cast<Hashjoin*>(customerJoin->right.release()));
      std::unique_ptr<vectorwise::Select> lineitemSelect(
          static_cast<vectorwise::Select*>(lineitemJoin->left.release()));
      std::unique_ptr<vectorwise::HashGroup> lineitemAggr(
          static_cast<vectorwise::HashGroup*>(lineitemSelect->child.release()));

      // build customer hash table
      {
         size_t found = 0;
         // --- build phase 1: materialize ht entries
         for (auto n = customerJoin->left->next();
              n != EndOfStream && !typerLib; n = customerJoin->left->next()) {
            found += n;
            // build hashes
            customerJoin->buildHash.evaluate(n);
            // scatter hash, keys and values into ht entries
            auto alloc = runtime::this_worker->allocator.allocate(
                n * customerJoin->ht_entry_size);
            if (!alloc) throw std::runtime_error("malloc failed");
            customerJoin->allocations.push_back(std::make_pair(alloc, n));
            customerJoin->scatterStart =
                reinterpret_cast<decltype(customerJoin->scatterStart)>(alloc);
            customerJoin->buildScatter.evaluate(n);
            processedTuples[0].fetch_add(vectorSize);
         }

         // --- build phase 2: insert ht entries
         customerJoin->shared.found.fetch_add(found);
         barrier([&]() {
            auto globalFound = customerJoin->shared.found.load();
            if (globalFound) customerJoin->shared.ht.setSize(globalFound);
         });
         auto globalFound = customerJoin->shared.found.load();
         if (globalFound != 0) {
            insertAllEntriesHybrid(customerJoin->allocations,
                                   customerJoin->shared.ht,
                                   customerJoin->ht_entry_size, twHashFunction,
                                   extractKeyFromEntry);
         }
         if (processedTuples[0].load() > nrTuples[0]) {
            processedTuples[0].store(nrTuples[0]);
         }
         barrier(); // wait for all threads to finish build phase
      }

      // start lineitem grouping
      {
         runtime::Hashmap& ht = lineitemAggr->getHashTable();
         size_t maxFill = lineitemAggr->getMaxFill();
         using header_t = runtime::Hashmap::EntryHeader;
         size_t groups = 0;
         auto& spill = lineitemAggr->shared.spillStorage.local();
         auto entry_size = lineitemAggr->preAggregation.ht_entry_size;

         // stores thread-local aggregation results in shared store (i.e.
         // SharedStateManager from above)
         auto flushAndClear = [&]() INTERPRET_SEPARATE {
            assert(offsetof(header_t, next) + sizeof(header_t::next) ==
                   offsetof(header_t, hash));
            // flush ht entries into spillStorage
            for (auto& alloc : lineitemAggr->preAggregation.allocations) {
               for (auto entry = reinterpret_cast<header_t*>(alloc.first),
                         end = addBytes(entry, alloc.second * entry_size);
                    entry < end; entry = addBytes(entry, entry_size)) {
                  spill.push_back(&entry->hash, entry->hash);
               }
            }
            lineitemAggr->preAggregation.allocations.clear();
            lineitemAggr->preAggregation.clearHashtable(ht);
         };

         // process relation data in chunks
         for (pos_t n = lineitemAggr->child->next();
              n != EndOfStream && !typerLib; n = lineitemAggr->child->next()) {
            lineitemAggr->groupHash.evaluate(n);
            lineitemAggr->preAggregation.findGroups(n, ht);
            auto groupsCreated =
                lineitemAggr->preAggregation.createMissingGroups(ht, false);
            lineitemAggr->updateGroups.evaluate(n);
            groups += groupsCreated;
            if (groups >= maxFill) flushAndClear();
            processedTuples[1].fetch_add(vectorSize);
            //  FIXME: delay for debugging purposes
            // std::this_thread::sleep_for(1ms);
         }
         flushAndClear(); // flush remaining entries into spillStorage
         if (processedTuples[1].load() > nrTuples[1]) {
            processedTuples[1].store(nrTuples[1]);
         }
         barrier(); // Wait until all workers have finished phase 1
      }
   });

   auto end = std::chrono::steady_clock::now();
   if (verbose) {
      std::cout << "TW took "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         start)
                       .count()
                << " milliseconds to process " << processedTuples[0].load()
                << " and " << processedTuples[1].load() << " tuples."
                << std::endl;
   }

   // 3. PROCESS REMAINING TUPLES WITH TYPER + MERGE-IN TW'S AGGREGATION
   // RESULTS
   compilationThread.join();
   start = std::chrono::steady_clock::now();
   // load library
   if (!typerLib) {
      throw hybrid::HybridException("Could not load shared Typer library!");
   }

   // get compiled function
   const std::string& funcName =
       "_Z18compiled_typer_q18RN7runtime8DatabaseEmRNS_7HashmapERSt13unordered_"
       "mapIimSt4hashIiESt8equal_toIiESaISt4pairIKimEEERS4_INSt6thread2idENS_"
       "16PartitionedDequeILm1024EEES5_ISG_ES7_ISG_ESaIS9_IKSG_SI_EEEm";
   hybrid::CompiledTyperQ18 typer_q18 =
       typerLib.load()->getFunction<hybrid::CompiledTyperQ18>(funcName);
   if (!typer_q18) {
      throw hybrid::HybridException(
          "Could not find function for running Q1 in Typer!");
   }

   // compute typer result
   result = std::move(typer_q18(
       db, nrThreads, shared.get<Hashjoin::Shared>(6).ht, twHashFunction,
       shared.get<HashGroup::Shared>(3).spillStorage.threadData,
       processedTuples[1].load()));

   end = std::chrono::steady_clock::now();
   if (verbose) {
      std::cout << "Typer took "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         start)
                       .count()
                << " milliseconds to process "
                << (long)nrTuples[0] - (long)processedTuples[0].load()
                << " and "
                << (long)nrTuples[1] - (long)processedTuples[1].load()
                << " tuples." << std::endl;
   }

   // close shared library
   delete typerLib;
   delete[] processedTuples;

   //    printResultQ18(result.get());
   return result;
}

NOVECTORIZE std::unique_ptr<runtime::Query> q18_hyper(Database& db,
                                                      size_t nrThreads) {
   using namespace types;
   using namespace std;

   using hash = runtime::CRC32Hash;

   auto resources = initQuery(nrThreads);

   auto& li = db["lineitem"];

   auto l_orderkey = li["l_orderkey"].data<types::Integer>();
   auto l_quantity = li["l_quantity"].data<types::Numeric<12, 2>>();

   tbb::enumerable_thread_specific<
       Hashmapx<types::Integer, types::Numeric<12, 2>, hash, false>>
       groups;

   const auto zero = types::Numeric<12, 2>::castString("0.00");

   auto groupOp = make_GroupBy<types::Integer, types::Numeric<12, 2>, hash>(
       [](auto& acc, auto&& value) { acc += value; }, zero, nrThreads);

   // scan lineitem and group by l_orderkey
   tbb::parallel_for(tbb::blocked_range<size_t>(0, li.nrTuples, morselSize),
                     [&](const tbb::blocked_range<size_t>& r) {
                        auto locals = groupOp.preAggLocals();

                        for (size_t i = r.begin(), end = r.end(); i != end;
                             ++i) {
                           auto& group = locals.getGroup(l_orderkey[i]);
                           group += l_quantity[i];
                           // locals.consume(l_orderkey[i], l_quantity[i]);
                        }
                     });

   Hashset<types::Integer, hash> ht1;
   tbb::enumerable_thread_specific<runtime::Stack<decltype(ht1)::Entry>>
       entries1;
   const auto threeHundret = types::Numeric<12, 2>::castString("300");
   std::atomic<size_t> nrGroups;
   nrGroups = 0;
   groupOp.forallGroups([&](auto& groups) {
      auto& entries = entries1.local();
      size_t groupsFound = 0;
      for (auto block : groups)
         for (auto& group : block)
            if (group.v > threeHundret) {
               entries.emplace_back(ht1.hash(group.k), group.k);
               groupsFound++;
            }
      // TODO: reconsider this way of counting groups
      nrGroups.fetch_add(groupsFound);
   });

   ht1.setSize(nrGroups);
   parallel_insert(entries1, ht1);

   // build customer hashtable
   auto& cu = db["customer"];
   auto c_custkey = cu["c_custkey"].data<types::Integer>();
   auto c_name = cu["c_name"].data<types::Char<25>>();
   Hashmapx<types::Integer, types::Char<25>, hash> ht2;
   tbb::enumerable_thread_specific<runtime::Stack<decltype(ht2)::Entry>>
       entries2;

   PARALLEL_SCAN(cu.nrTuples, entries2, {
      entries.emplace_back(ht2.hash(c_custkey[i]), c_custkey[i], c_name[i]);
   });
   ht2.setSize(cu.nrTuples);
   parallel_insert(entries2, ht2);

   // build last hashtable
   Hashmapx<types::Integer,
            std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,
                       types::Char<25>>,
            hash>
       ht3;
   tbb::enumerable_thread_specific<runtime::Stack<decltype(ht3)::Entry>>
       entries3;

   auto& ord = db["orders"];
   auto o_orderkey = ord["o_orderkey"].data<types::Integer>();
   auto o_custkey = ord["o_custkey"].data<types::Integer>();
   auto o_orderdate = ord["o_orderdate"].data<types::Date>();
   auto o_totalprice = ord["o_totalprice"].data<types::Numeric<12, 2>>();
   // scan orders
   auto found = PARALLEL_SELECT(ord.nrTuples, entries3, {
      types::Char<25>* name;
      // check if it matches the order criteria and look up the customer name
      if (ht1.contains(o_orderkey[i]) && (name = ht2.findOne(o_custkey[i]))) {
         entries.emplace_back(
             ht3.hash(o_orderkey[i]), o_orderkey[i],
             make_tuple(o_custkey[i], o_orderdate[i], o_totalprice[i], *name));
         found++;
      }
   });
   ht3.setSize(found);
   parallel_insert(entries3, ht3);

   auto finalGroupOp = make_GroupBy<
       std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,
                  types::Char<25>, types::Integer>,
       types::Numeric<12, 2>, hash>(
       [](auto& acc, auto&& value) { acc += value; }, zero, nrThreads);

   // scan lineitem and group by l_orderkey
   tbb::parallel_for(
       tbb::blocked_range<size_t>(0, li.nrTuples, morselSize),
       [&](const tbb::blocked_range<size_t>& r) {
          auto locals = finalGroupOp.preAggLocals();

          for (size_t i = r.begin(), end = r.end(); i != end; ++i) {
             std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,
                        types::Char<25>>* v;
             if ((v = ht3.findOne(l_orderkey[i]))) {
                auto& group =
                    locals.getGroup(tuple_cat(*v, make_tuple(l_orderkey[i])));
                group += l_quantity[i];
             }
          }
       });

   auto& result = resources.query->result;
   auto namAttr = result->addAttribute("c_name", sizeof(types::Char<25>));
   auto ckyAttr = result->addAttribute("c_custkey", sizeof(types::Integer));
   auto okyAttr = result->addAttribute("o_orderkey", sizeof(types::Integer));
   auto datAttr = result->addAttribute("o_orderdate", sizeof(types::Date));
   auto totAttr =
       result->addAttribute("o_totalprice", sizeof(types::Numeric<12, 2>));
   auto sumAttr = result->addAttribute("sum", sizeof(types::Numeric<12, 2>));

   finalGroupOp.forallGroups([&](auto& groups) {
      // write aggregates to result
      auto n = groups.size();
      auto block = result->createBlock(n);
      auto name = reinterpret_cast<types::Char<25>*>(block.data(namAttr));
      auto cky = reinterpret_cast<types::Integer*>(block.data(ckyAttr));
      auto oky = reinterpret_cast<types::Integer*>(block.data(okyAttr));
      auto dat = reinterpret_cast<types::Date*>(block.data(datAttr));
      auto tot = reinterpret_cast<types::Numeric<12, 2>*>(block.data(totAttr));
      auto sum = reinterpret_cast<types::Numeric<12, 2>*>(block.data(sumAttr));
      for (auto block : groups)
         for (auto& group : block) {
            auto& k = group.k;
            *name++ = std::get<3>(k);
            *cky++ = get<0>(k);
            *oky++ = get<4>(k);
            *dat++ = get<1>(k);
            *tot++ = get<2>(k);
            *sum++ = group.v;
         }
      block.addedElements(n);
   });

   leaveQuery(nrThreads);
   //    printResultQ18(resources.query.get());
   return move(resources.query);
}

std::unique_ptr<Q18Builder::Q18> Q18Builder::getQuery() {
   using namespace vectorwise;

   auto result = Result();
   previous = result.resultWriter.shared.result->participate();

   auto r = make_unique<Q18>();
   auto customer = Scan("customer");
   auto lineitem = Scan("lineitem");
   HashGroup()
       .addKey(Column(lineitem, "l_orderkey"), primitives::hash_int32_t_col,
               primitives::keys_not_equal_int32_t_col,
               primitives::partition_by_key_int32_t_col,
               primitives::scatter_sel_int32_t_col,
               primitives::keys_not_equal_row_int32_t_col,
               primitives::partition_by_key_row_int32_t_col,
               primitives::scatter_sel_row_int32_t_col,
               primitives::gather_val_int32_t_col,
               Buffer(l_orderkey, sizeof(int32_t)))
       .addValue(Column(lineitem, "l_quantity"),
                 primitives::aggr_init_plus_int64_t_col,
                 primitives::aggr_plus_int64_t_col,
                 primitives::aggr_row_plus_int64_t_col,
                 primitives::gather_val_int64_t_col,
                 Buffer(l_quantity, sizeof(int64_t)));
   Select(
       Expression().addOp(BF(primitives::sel_greater_int64_t_col_int64_t_val),
                          Buffer(sel_orderkey, sizeof(pos_t)),
                          Buffer(l_quantity), Value(&r->qty_bound)));
   auto orders = Scan("orders");
   // FIXME: This should be a right semi join (, but for now, hashjoin may be
   // good enough)
   HashJoin(Buffer(orders_matches, sizeof(pos_t)))
       .addBuildKey(Buffer(l_orderkey), //
                    Buffer(sel_orderkey), primitives::hash_sel_int32_t_col,
                    primitives::scatter_sel_int32_t_col)
       .addProbeKey(Column(orders, "o_orderkey"), //
                    primitives::hash_int32_t_col,
                    primitives::keys_equal_int32_t_col);
   HashJoin(Buffer(customer_matches, sizeof(pos_t)))
       .setProbeSelVector(Buffer(orders_matches))
       .addBuildKey(Column(customer, "c_custkey"), primitives::hash_int32_t_col,
                    primitives::scatter_int32_t_col)
       .addProbeKey(Column(orders, "o_custkey"), Buffer(orders_matches),
                    primitives::hash_sel_int32_t_col,
                    primitives::keys_equal_int32_t_col)
       .addBuildValue(Column(customer, "c_name"),
                      primitives::scatter_Char_25_col,
                      Buffer(c_name, sizeof(types::Char<25>)),
                      primitives::gather_col_Char_25_col);
   auto lineitem2 = Scan("lineitem");
   HashJoin(Buffer(lineitem_matches, sizeof(pos_t)))
       .addBuildKey(Column(orders, "o_orderkey"), Buffer(customer_matches),
                    primitives::hash_sel_int32_t_col,
                    primitives::scatter_sel_int32_t_col)
       .addProbeKey(Column(lineitem2, "l_orderkey"),
                    primitives::hash_int32_t_col,
                    primitives::keys_equal_int32_t_col)
       .addBuildValue(Column(orders, "o_custkey"), Buffer(customer_matches),
                      primitives::scatter_sel_int32_t_col,
                      Buffer(o_custkey, sizeof(int32_t)),
                      primitives::gather_col_int32_t_col)
       .addBuildValue(Column(orders, "o_orderdate"), Buffer(customer_matches),
                      primitives::scatter_sel_Date_col,
                      Buffer(o_orderdate, sizeof(types::Date)),
                      primitives::gather_col_Date_col)
       .addBuildValue(Column(orders, "o_totalprice"), Buffer(customer_matches),
                      primitives::scatter_sel_int64_t_col,
                      Buffer(o_totalprice, sizeof(types::Numeric<12, 2>)),
                      primitives::gather_col_int64_t_col)
       .addBuildValue(Buffer(c_name), primitives::scatter_Char_25_col,
                      Buffer(c_name2, sizeof(types::Char<25>)),
                      primitives::gather_col_Char_25_col);
   HashGroup() //
       .pushKeySelVec(Buffer(lineitem_matches),
                      Buffer(lineitem_matches_grouped, sizeof(pos_t)))
       .addKey(Buffer(c_name2), primitives::hash_Char_25_col,
               primitives::keys_not_equal_Char_25_col,
               primitives::partition_by_key_Char_25_col,
               primitives::scatter_sel_Char_25_col,
               primitives::keys_not_equal_row_Char_25_col,
               primitives::partition_by_key_row_Char_25_col,
               primitives::scatter_sel_row_Char_25_col,
               primitives::gather_val_Char_25_col,
               Buffer(group_c_name, sizeof(types::Char<25>)))
       .addKey(Buffer(o_custkey), primitives::rehash_int32_t_col,
               primitives::keys_not_equal_int32_t_col,
               primitives::partition_by_key_int32_t_col,
               primitives::scatter_sel_int32_t_col,
               primitives::keys_not_equal_row_int32_t_col,
               primitives::partition_by_key_row_int32_t_col,
               primitives::scatter_sel_row_int32_t_col,
               primitives::gather_val_int32_t_col,
               Buffer(group_o_custkey, sizeof(int32_t)))
       .addKey(Buffer(o_orderdate), primitives::rehash_Date_col,
               primitives::keys_not_equal_Date_col,
               primitives::partition_by_key_Date_col,
               primitives::scatter_sel_Date_col,
               primitives::keys_not_equal_row_Date_col,
               primitives::partition_by_key_row_Date_col,
               primitives::scatter_sel_row_Date_col,
               primitives::gather_val_Date_col,
               Buffer(group_o_orderdate, sizeof(types::Date)))
       .addKey(Buffer(o_totalprice), primitives::rehash_int64_t_col,
               primitives::keys_not_equal_int64_t_col,
               primitives::partition_by_key_int64_t_col,
               primitives::scatter_sel_int64_t_col,
               primitives::keys_not_equal_row_int64_t_col,
               primitives::partition_by_key_row_int64_t_col,
               primitives::scatter_sel_row_int64_t_col,
               primitives::gather_val_int64_t_col,
               Buffer(group_o_totalprice, sizeof(int64_t)))
       .addKey(Column(lineitem2, "l_orderkey"), Buffer(lineitem_matches),
               primitives::rehash_sel_int32_t_col,
               primitives::keys_not_equal_sel_int32_t_col,
               primitives::partition_by_key_sel_int32_t_col,
               Buffer(lineitem_matches_grouped, sizeof(pos_t)),
               primitives::scatter_sel_int32_t_col,
               primitives::keys_not_equal_row_int32_t_col,
               primitives::partition_by_key_row_int32_t_col,
               primitives::scatter_sel_row_int32_t_col,
               primitives::gather_val_int32_t_col,
               Buffer(group_l_orderkey, sizeof(int32_t)))
       .addValue(Column(lineitem2, "l_quantity"), Buffer(lineitem_matches),
                 primitives::aggr_init_plus_int64_t_col,
                 primitives::aggr_sel_plus_int64_t_col,
                 primitives::aggr_row_plus_int64_t_col,
                 primitives::gather_val_int64_t_col,
                 Buffer(group_sum, sizeof(types::Numeric<12, 2>)));

   result.addValue("c_name", Buffer(group_c_name))
       .addValue("c_custkey", Buffer(group_o_custkey))
       .addValue("o_orderkey", Buffer(group_l_orderkey))
       .addValue("o_orderdate", Buffer(group_o_orderdate))
       .addValue("o_totalprice", Buffer(group_o_totalprice))
       .addValue("sum", Buffer(group_sum))
       .finalize();

   r->rootOp = popOperator();
   return r;
}

std::unique_ptr<runtime::Query> q18_vectorwise(Database& db, size_t nrThreads,
                                               size_t vectorSize) {
   using namespace vectorwise;
   WorkerGroup workers(nrThreads);
   // TODO: move into resources?
   vectorwise::SharedStateManager shared;

   std::unique_ptr<runtime::Query> result;
   workers.run([&]() {
      Q18Builder builder(db, shared, vectorSize);
      auto resources = builder.getQuery();
      resources->rootOp->next();
      auto leader = barrier();
      if (leader)
         result = move(dynamic_cast<ResultWriter*>(resources->rootOp.get())
                           ->shared.result);
   });

   //    printResultQ18(result.get());
   return result;
}

std::unique_ptr<Q18Builder::Q18> Q18Builder::getGroupQuery() {
   using namespace vectorwise;

   auto result = Result();
   previous = result.resultWriter.shared.result->participate();

   auto r = make_unique<Q18>();
   auto lineitem = Scan("lineitem");
   HashGroup()
       .addKey(Column(lineitem, "l_orderkey"), primitives::hash_int32_t_col,
               primitives::keys_not_equal_int32_t_col,
               primitives::partition_by_key_int32_t_col,
               primitives::scatter_sel_int32_t_col,
               primitives::keys_not_equal_row_int32_t_col,
               primitives::partition_by_key_row_int32_t_col,
               primitives::scatter_sel_row_int32_t_col,
               primitives::gather_val_int32_t_col,
               Buffer(l_orderkey, sizeof(int32_t)))
       .addValue(Column(lineitem, "l_quantity"),
                 primitives::aggr_init_plus_int64_t_col,
                 primitives::aggr_plus_int64_t_col,
                 primitives::aggr_row_plus_int64_t_col,
                 primitives::gather_val_int64_t_col,
                 Buffer(l_quantity, sizeof(int64_t)));
   Select(Expression()
              .addOp(BF(primitives::sel_greater_int64_t_col_int64_t_val),
                     Buffer(sel_orderkey, sizeof(pos_t)), Buffer(l_quantity),
                     Value(&r->qty_bound))
              .addOp(primitives::proj_sel_plus_int64_t_col_int64_t_val,
                     Buffer(sel_orderkey, sizeof(pos_t)),
                     Buffer(compact_quantity, sizeof(int64_t)),
                     Buffer(l_quantity), Value(&r->zero))
              .addOp(primitives::proj_sel_plus_int32_t_col_int32_t_val,
                     Buffer(sel_orderkey, sizeof(pos_t)),
                     Buffer(compact_l_orderkey, sizeof(int32_t)),
                     Buffer(l_orderkey), Value(&r->zero)));

   result.addValue("l_orderkey", Buffer(compact_l_orderkey))
       .addValue("sum", Buffer(compact_quantity))
       .finalize();

   r->rootOp = popOperator();
   return r;
}

std::unique_ptr<runtime::Query>
q18group_vectorwise(Database& db, size_t nrThreads, size_t vectorSize) {
   using namespace vectorwise;
   WorkerGroup workers(nrThreads);
   // TODO: move into resources?
   vectorwise::SharedStateManager shared;

   std::unique_ptr<runtime::Query> result;
   workers.run([&]() {
      Q18Builder builder(db, shared, vectorSize);
      auto resources = builder.getGroupQuery();
      resources->rootOp->next();
      auto leader = barrier();
      if (leader)
         result = move(dynamic_cast<ResultWriter*>(resources->rootOp.get())
                           ->shared.result);
   });

   //    printResultQ18(result.get());
   return result;
}
