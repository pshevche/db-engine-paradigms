#include "benchmarks/tpch/Queries.hpp"
#include "common/runtime/Hash.hpp"
#include "common/runtime/Types.hpp"
#include "hybrid/compilation_engine.hpp"
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
#include <deque>
#include <iostream>

using namespace runtime;
using namespace std;
using vectorwise::primitives::Char_1;
using vectorwise::primitives::hash_t;

//  select
//    l_returnflag,
//    l_linestatus,
//    sum(l_quantity) as sum_qty,
//    sum(l_extendedprice) as sum_base_price,
//    sum(l_extendedprice * (1 - l_discount)) as sum_disc_price,
//    sum(l_extendedprice * (1 - l_discount) * (1 + l_tax)) as sum_charge,
//    avg(l_quantity) as avg_qty,
//    avg(l_extendedprice) as avg_price,
//    avg(l_discount) as avg_disc,
//    count(*) as count_order
//  from
//    lineitem
//  where
//    l_shipdate <= date '1998-12-01' - interval '90' day
//  group by
//    l_returnflag,
//    l_linestatus

typedef std::unique_ptr<runtime::Query> (*CompiledTyperQuery)(Database&, size_t,
                                                              size_t);
std::unique_ptr<runtime::Query> q1_hybrid(Database& db, size_t nrThreads,
                                          size_t vectorSize,
                                          const std::string& path_to_lib_src,
                                          bool fromLLVM) {
   using namespace vectorwise;
   using namespace std::chrono_literals;

   // 0. INFO ABOUT QUERY
   const std::string groupAttributes[2] = {"l_returnflag", "l_linestatus"};
   const std::string otherAttributes[5] = {"sum_qty", "sum_base_price",
                                           "sum_disc_price", "sum_charge",
                                           "count_order"};

   // 1. START COMPILING Q1 IN TYPER
   std::atomic<hybrid::SharedLibrary*> typerLib(nullptr);
   std::thread compilationThread([&typerLib, &path_to_lib_src, &fromLLVM] {
      try {
         auto start = std::chrono::steady_clock::now();
         // link library
         const std::string& path_to_lib =
             hybrid::CompilationEngine::instance().linkQueryLib(path_to_lib_src,
                                                                fromLLVM);
         // open library
         typerLib = hybrid::SharedLibrary::load(path_to_lib + ".so");
         auto end = std::chrono::steady_clock::now();
         std::cout << "Compilation took "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(
                          end - start)
                          .count()
                   << " milliseconds." << std::endl;
      } catch (hybrid::HybridException& exc) {
         std::cerr << exc.what() << std::endl;
      }
   });

   // 2. WHILE Q1 IS COMPILING, START TECTORWISE
   auto start = std::chrono::steady_clock::now();
   std::atomic<size_t> processedTuples(0);
   WorkerGroup workers(nrThreads);
   vectorwise::SharedStateManager shared;

   std::unique_ptr<runtime::Query> result;
   workers.run([&]() {
      Q1Builder builder(db, shared, vectorSize);
      auto query = builder.getQuery();
      std::unique_ptr<ResultWriter> topOp(
          static_cast<ResultWriter*>(query->rootOp.release()));
      // organize results in the relation
      for (pos_t pos = topOp->child->next(); pos != EndOfStream;
           pos = topOp->child->next()) {
         // assure that enough space is available in current block to fit result
         // of all buffers
         auto currentBlock = topOp->getCurrentBlock();
         if (currentBlock.spaceRemaining() < pos)
            currentBlock = topOp->shared.result->result->createBlock(pos);
         auto blockSize = currentBlock.size();
         for (const auto& input : topOp->inputs)
            // copy data from intermediate buffers into result relation
            std::memcpy(addBytes(currentBlock.data(input.attribute),
                                 input.elementSize * blockSize),
                        input.data, pos * input.elementSize);
         // update result relation size
         currentBlock.addedElements(pos);
         processedTuples.fetch_add(vectorSize);
      }

      // sync results
      auto leader = barrier();
      if (leader)
         result = move(dynamic_cast<ResultWriter*>(topOp.get())->shared.result);
   });

   // display some results for debugging
   for (auto twIter = result->result->begin(); twIter != result->result->end();
        ++twIter) {
      auto twBlock = *twIter;
      char* tw_groupattr1 = reinterpret_cast<char*>(
          twBlock.data(result->result->getAttribute(groupAttributes[0])));
      char* tw_groupattr2 = reinterpret_cast<char*>(
          twBlock.data(result->result->getAttribute(groupAttributes[1])));
      // if a match is found, combine aggregation results
      long* tw_sum_qty = reinterpret_cast<long*>(
          twBlock.data(result->result->getAttribute(otherAttributes[0])));
      for (unsigned i = 0; i < twBlock.size(); ++i) {
         std::cout << tw_groupattr1[i] << " | " << tw_groupattr2[i] << " | "
                   << tw_sum_qty[i] << std::endl;
      }
   }

   auto end = std::chrono::steady_clock::now();
   std::cout << "TW took "
             << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                      start)
                    .count()
             << " milliseconds to process " << processedTuples.load()
             << " tuples." << std::endl;

   // 3. PROCESS REMAINING TUPLES WITH TYPER
   compilationThread.join();
   start = std::chrono::steady_clock::now();
   size_t nrTuples = db["lineitem"].nrTuples;
   if (processedTuples.load() < nrTuples) {
      // load library
      if (!typerLib) {
         throw hybrid::HybridException("Could not load shared Typer library!");
      }

      // get compiled function
      const std::string& funcName =
          "_Z17compiled_typer_q1RN7runtime8DatabaseEmm";
      CompiledTyperQuery typer_q1 =
          typerLib.load()->getFunction<CompiledTyperQuery>(funcName);
      if (!typer_q1) {
         throw hybrid::HybridException(
             "Could not find function for running Q1 in Typer!");
      }

      // compute typer result
      //   std::unique_ptr<runtime::Query> typer_result =
      //       typer_q1(db, nrThreads, processedTuples.load());
      std::unique_ptr<runtime::Query> typer_result = typer_q1(db, nrThreads, 0);

      // display some results for debugging
      for (auto typerIter = typer_result->result->begin();
           typerIter != typer_result->result->end(); ++typerIter) {
         auto typerBlock = *typerIter;
         char* typer_groupattr1 = reinterpret_cast<char*>(typerBlock.data(
             typer_result->result->getAttribute(groupAttributes[0])));
         char* typer_groupattr2 = reinterpret_cast<char*>(typerBlock.data(
             typer_result->result->getAttribute(groupAttributes[1])));
         // if a match is found, combine aggregation results
         long* typer_sum_qty = reinterpret_cast<long*>(typerBlock.data(
             typer_result->result->getAttribute(otherAttributes[0])));
         for (unsigned i = 0; i < typerBlock.size(); ++i) {
            std::cout << typer_groupattr1[i] << " | " << typer_groupattr2[i]
                      << " | " << typer_sum_qty[i] << std::endl;
         }
      }

      // merge results
      const std::string groupAttr1 = groupAttributes[0];
      const std::string groupAttr2 = groupAttributes[1];
      // for each block of the TW result
      for (auto twIter = result->result->begin();
           twIter != result->result->end(); ++twIter) {
         auto twBlock = *twIter;
         // for each tuple in the block
         for (unsigned i = 0; i < twBlock.size(); ++i) {
            // get values of the grouping attr in TW result
            char tw_groupattr1 = reinterpret_cast<char*>(
                twBlock.data(result->result->getAttribute(groupAttr1)))[i];
            char tw_groupattr2 = reinterpret_cast<char*>(
                twBlock.data(result->result->getAttribute(groupAttr2)))[i];
            // and find a matching tuple in the blocks of the Typer result
            for (auto typerIter = typer_result->result->begin();
                 typerIter != typer_result->result->end(); ++typerIter) {
               auto typerBlock = *typerIter;
               for (unsigned j = 0; j < typerBlock.size(); ++j) {
                  // get values of the grouping attr in Typer result
                  char typer_groupattr1 =
                      reinterpret_cast<char*>(typerBlock.data(
                          typer_result->result->getAttribute(groupAttr1)))[j];
                  char typer_groupattr2 =
                      reinterpret_cast<char*>(typerBlock.data(
                          typer_result->result->getAttribute(groupAttr2)))[j];

                  // if a match is found
                  if (tw_groupattr1 == typer_groupattr1 &&
                      tw_groupattr2 == typer_groupattr2) {
                     // aggregate over all remaining attributes
                     for (unsigned k = 0; k < 5; ++k) {
                        long* tw_attr = reinterpret_cast<long*>(twBlock.data(
                            result->result->getAttribute(otherAttributes[k])));
                        long* typer_attr = reinterpret_cast<long*>(
                            typerBlock.data(typer_result->result->getAttribute(
                                otherAttributes[k])));
                        tw_attr[i] += typer_attr[j];
                     }
                  }
               }
            }
         }
      }
   }

   // display some results for debugging
   for (auto twIter = result->result->begin(); twIter != result->result->end();
        ++twIter) {
      auto twBlock = *twIter;
      char* tw_groupattr1 = reinterpret_cast<char*>(
          twBlock.data(result->result->getAttribute(groupAttributes[0])));
      char* tw_groupattr2 = reinterpret_cast<char*>(
          twBlock.data(result->result->getAttribute(groupAttributes[1])));
      // if a match is found, combine aggregation results
      long* tw_sum_qty = reinterpret_cast<long*>(
          twBlock.data(result->result->getAttribute(otherAttributes[0])));
      for (unsigned i = 0; i < twBlock.size(); ++i) {
         std::cout << tw_groupattr1[i] << " | " << tw_groupattr2[i] << " | "
                   << tw_sum_qty[i] << std::endl;
      }
   }

   end = std::chrono::steady_clock::now();
   std::cout << "Typer took "
             << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                      start)
                    .count()
             << " milliseconds to process "
             << (long)nrTuples - (long)processedTuples.load() << " tuples."
             << std::endl;

   // close shared library
   delete typerLib;

   return result;
}

NOVECTORIZE std::unique_ptr<runtime::Query> q1_hyper(Database& db,
                                                     size_t nrThreads) {
   using namespace types;
   using namespace std;
   types::Date c1 = types::Date::castString("1998-09-02");
   types::Numeric<12, 2> one = types::Numeric<12, 2>::castString("1.00");
   auto& li = db["lineitem"];
   auto l_returnflag = li["l_returnflag"].data<types::Char<1>>();
   auto l_linestatus = li["l_linestatus"].data<types::Char<1>>();
   auto l_extendedprice = li["l_extendedprice"].data<types::Numeric<12, 2>>();
   auto l_discount = li["l_discount"].data<types::Numeric<12, 2>>();
   auto l_tax = li["l_tax"].data<types::Numeric<12, 2>>();
   auto l_quantity = li["l_quantity"].data<types::Numeric<12, 2>>();
   auto l_shipdate = li["l_shipdate"].data<types::Date>();

   auto resources = initQuery(nrThreads);

   using hash = runtime::CRC32Hash;

   auto groupOp = make_GroupBy<tuple<Char<1>, Char<1>>,
                               tuple<Numeric<12, 2>, Numeric<12, 2>,
                                     Numeric<12, 4>, Numeric<12, 6>, int64_t>,
                               hash>(
       [](auto& acc, auto&& value) {
          get<0>(acc) += get<0>(value);
          get<1>(acc) += get<1>(value);
          get<2>(acc) += get<2>(value);
          get<3>(acc) += get<3>(value);
          get<4>(acc) += get<4>(value);
       },
       make_tuple(Numeric<12, 2>(), Numeric<12, 2>(), Numeric<12, 4>(),
                  Numeric<12, 6>(), int64_t(0)),
       nrThreads);

   tbb::parallel_for(
       tbb::blocked_range<size_t>(0, li.nrTuples, morselSize),
       [&](const tbb::blocked_range<size_t>& r) {
          auto locals = groupOp.preAggLocals();
          for (size_t i = r.begin(), end = r.end(); i != end; ++i) {
             if (l_shipdate[i] <= c1) {
                auto& group = locals.getGroup(
                    make_tuple(l_returnflag[i], l_linestatus[i]));

                get<0>(group) += l_quantity[i];
                get<1>(group) += l_extendedprice[i];
                auto disc_price = l_extendedprice[i] * (one - l_discount[i]);
                get<2>(group) += disc_price;
                auto charge = disc_price * (one + l_tax[i]);
                get<3>(group) += charge;
                get<4>(group) += 1;
             }
          }
       });

   auto& result = resources.query->result;
   auto retAttr = result->addAttribute("l_returnflag", sizeof(Char<1>));
   auto statusAttr = result->addAttribute("l_linestatus", sizeof(Char<1>));
   auto qtyAttr = result->addAttribute("sum_qty", sizeof(Numeric<12, 2>));
   auto base_priceAttr =
       result->addAttribute("sum_base_price", sizeof(Numeric<12, 2>));
   auto disc_priceAttr =
       result->addAttribute("sum_disc_price", sizeof(Numeric<12, 2>));
   auto chargeAttr = result->addAttribute("sum_charge", sizeof(Numeric<12, 2>));
   auto count_orderAttr = result->addAttribute("count_order", sizeof(int64_t));

   groupOp.forallGroups(
       [&](runtime::Stack<decltype(groupOp)::group_t>& /*auto&*/ entries) {
          auto n = entries.size();
          auto block = result->createBlock(n);
          auto ret = reinterpret_cast<Char<1>*>(block.data(retAttr));
          auto status = reinterpret_cast<Char<1>*>(block.data(statusAttr));
          auto qty = reinterpret_cast<Numeric<12, 2>*>(block.data(qtyAttr));
          auto base_price =
              reinterpret_cast<Numeric<12, 2>*>(block.data(base_priceAttr));
          auto disc_price =
              reinterpret_cast<Numeric<12, 4>*>(block.data(disc_priceAttr));
          auto charge =
              reinterpret_cast<Numeric<12, 6>*>(block.data(chargeAttr));
          auto count_order =
              reinterpret_cast<int64_t*>(block.data(count_orderAttr));
          for (auto block : entries)
             for (auto& entry : block) {
                *ret++ = get<0>(entry.k);
                *status++ = get<1>(entry.k);
                *qty++ = get<0>(entry.v);
                *base_price++ = get<1>(entry.v);
                *disc_price++ = get<2>(entry.v);
                *charge++ = get<3>(entry.v);
                *count_order++ = get<4>(entry.v);
             }
          block.addedElements(n);
       });

   leaveQuery(nrThreads);
   return move(resources.query);
}

std::unique_ptr<Q1Builder::Q1> Q1Builder::getQuery() {
   using namespace vectorwise;
   auto result = Result();
   previous = result.resultWriter.shared.result->participate();

   auto r = make_unique<Q1>();
   auto lineitem = Scan("lineitem");
   Select(Expression().addOp(BF(primitives::sel_less_equal_Date_col_Date_val),
                             Buffer(sel_date, sizeof(pos_t)),
                             Column(lineitem, "l_shipdate"), Value(&r->c1)));
   Project()
       .addExpression(
           Expression()
               .addOp(conf.proj_sel_minus_int64_t_val_int64_t_col(),
                      Buffer(sel_date),
                      Buffer(result_proj_minus, sizeof(int64_t)),
                      Value(&r->one), Column(lineitem, "l_discount"))
               .addOp(conf.proj_multiplies_sel_int64_t_col_int64_t_col(),
                      Buffer(sel_date), Buffer(disc_price, sizeof(int64_t)),
                      Column(lineitem, "l_extendedprice"),
                      Buffer(result_proj_minus, sizeof(int64_t))))
       .addExpression(Expression()
                          .addOp(conf.proj_sel_plus_int64_t_col_int64_t_val(),
                                 Buffer(sel_date),
                                 Buffer(result_proj_plus, sizeof(int64_t)),
                                 Column(lineitem, "l_tax"), Value(&r->one))
                          .addOp(conf.proj_multiplies_int64_t_col_int64_t_col(),
                                 Buffer(charge, sizeof(int64_t)),
                                 Buffer(disc_price, sizeof(int64_t)),
                                 Buffer(result_proj_plus, sizeof(int64_t))));
   HashGroup()
       .pushKeySelVec(Buffer(sel_date), Buffer(sel_date_grouped, sizeof(pos_t)))
       .addKey(Column(lineitem, "l_returnflag"), Buffer(sel_date),
               primitives::hash_sel_Char_1_col,
               primitives::keys_not_equal_sel_Char_1_col,
               primitives::partition_by_key_sel_Char_1_col,
               Buffer(sel_date_grouped, sizeof(pos_t)),
               primitives::scatter_sel_Char_1_col,
               primitives::keys_not_equal_row_Char_1_col,
               primitives::partition_by_key_row_Char_1_col,
               primitives::scatter_sel_row_Char_1_col,
               primitives::gather_val_Char_1_col,
               Buffer(returnflag, sizeof(Char_1)))
       .addKey(Column(lineitem, "l_linestatus"), Buffer(sel_date),
               primitives::rehash_sel_Char_1_col,
               primitives::keys_not_equal_sel_Char_1_col,
               primitives::partition_by_key_sel_Char_1_col,
               Buffer(sel_date_grouped, sizeof(pos_t)),
               primitives::scatter_sel_Char_1_col,
               primitives::keys_not_equal_row_Char_1_col,
               primitives::partition_by_key_row_Char_1_col,
               primitives::scatter_sel_row_Char_1_col,
               primitives::gather_val_Char_1_col,
               Buffer(linestatus, sizeof(Char_1)))
       .padToAlign(sizeof(types::Numeric<12, 4>))
       .addValue(Buffer(disc_price), primitives::aggr_init_plus_int64_t_col,
                 primitives::aggr_plus_int64_t_col,
                 primitives::aggr_row_plus_int64_t_col,
                 primitives::gather_val_int64_t_col,
                 Buffer(sum_disc_price, sizeof(types::Numeric<12, 4>)))
       .addValue(Buffer(charge), primitives::aggr_init_plus_int64_t_col,
                 primitives::aggr_plus_int64_t_col,
                 primitives::aggr_row_plus_int64_t_col,
                 primitives::gather_val_int64_t_col,
                 Buffer(sum_charge, sizeof(types::Numeric<12, 4>)))
       .addValue(Column(lineitem, "l_quantity"), Buffer(sel_date),
                 primitives::aggr_init_plus_int64_t_col,
                 primitives::aggr_sel_plus_int64_t_col,
                 primitives::aggr_row_plus_int64_t_col,
                 primitives::gather_val_int64_t_col,
                 Buffer(sum_qty, sizeof(types::Numeric<12, 2>)))
       .addValue(Column(lineitem, "l_extendedprice"), Buffer(sel_date),
                 primitives::aggr_init_plus_int64_t_col,
                 primitives::aggr_sel_plus_int64_t_col,
                 primitives::aggr_row_plus_int64_t_col,
                 primitives::gather_val_int64_t_col,
                 Buffer(sum_base_price, sizeof(types::Numeric<12, 2>)))
       .addValue(Buffer(charge, sizeof(uint64_t)),
                 primitives::aggr_init_plus_int64_t_col,
                 primitives::aggr_count_star,
                 primitives::aggr_row_plus_int64_t_col,
                 primitives::gather_val_int64_t_col,
                 Buffer(count_order, sizeof(uint64_t)));

   result.addValue("l_returnflag", Buffer(returnflag))
       .addValue("l_linestatus", Buffer(linestatus))
       .addValue("sum_qty", Buffer(sum_qty))
       .addValue("sum_base_price", Buffer(sum_base_price))
       .addValue("sum_disc_price", Buffer(sum_disc_price))
       .addValue("sum_charge", Buffer(sum_charge))
       .addValue("count_order", Buffer(count_order))
       .finalize();

   // TODO: add averages
   r->rootOp = popOperator();
   return r;
}

std::unique_ptr<runtime::Query> q1_vectorwise(Database& db, size_t nrThreads,
                                              size_t vectorSize) {
   using namespace vectorwise;
   WorkerGroup workers(nrThreads);
   vectorwise::SharedStateManager shared;

   std::unique_ptr<runtime::Query> result;
   workers.run([&]() {
      Q1Builder builder(db, shared, vectorSize);
      auto query = builder.getQuery();
      /* auto found = */ query->rootOp->next();
      auto leader = barrier();
      if (leader)
         result = move(
             dynamic_cast<ResultWriter*>(query->rootOp.get())->shared.result);
   });

   return result;
}
