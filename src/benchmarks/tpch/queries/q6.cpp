#include "benchmarks/tpch/Queries.hpp"
#include "common/runtime/Types.hpp"
#include "hybrid/compilation_engine.hpp"
#include "hybrid/shared_library.hpp"
#include "tbb/tbb.h"
#include "vectorwise/Operations.hpp"
#include "vectorwise/Operators.hpp"
#include "vectorwise/Primitives.hpp"
#include "vectorwise/QueryBuilder.hpp"
#include "vectorwise/VectorAllocator.hpp"
#include <chrono>
#include <future>
#include <iostream>

using namespace runtime;
typedef Relation (*CompiledTyperQuery)(Database&, size_t, size_t);

// Execute hybrid of Typer and Tectorwise
Relation q6_hybrid(Database& db, size_t nrThreads, size_t vectorSize) {
   using namespace vectorwise;
   using namespace std::chrono_literals;

   // start compiling typer
   std::atomic<hybrid::SharedLibrary*> typerLib(nullptr);
   std::thread compilationThread([&typerLib] {
      auto start = std::chrono::steady_clock::now();
      hybrid::CompilationEngine::instance().precompileHeader();
      const char* libPath =
          hybrid::CompilationEngine::instance().compileQ6CPP();
      if (!libPath) {
         std::cerr << "Compilation failed!" << std::endl;
         std::exit(1);
      }
      typerLib = hybrid::SharedLibrary::load(libPath);
      auto end = std::chrono::steady_clock::now();
      std::cout << "Compilation took "
                << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         start)
                       .count()
                << " milliseconds." << std::endl;
   });

   // final result
   Relation result;
   result.insert("revenue", std::make_unique<algebra::Numeric>(12, 4));

   // load queried tables
   auto& rel = db["lineitem"];

   // init Tectorwise
   GlobalPool pool;
   vectorwise::SharedStateManager shared;
   Q6Builder queryBuilder(db, shared, vectorSize);
   queryBuilder.previous = this_worker->allocator.setSource(&pool);
   auto query = queryBuilder.getQuery();
   std::unique_ptr<vectorwise::FixedAggr> topAggr(
       static_cast<vectorwise::FixedAggr*>(query->rootOp.release()));
   size_t pos = 0;

   // execute tectorwise
   auto start = std::chrono::steady_clock::now();
   size_t processedTuples = 0;
   while (processedTuples < rel.nrTuples && !typerLib) {
      pos = topAggr->child->next();
      if (pos == EndOfStream) { break; }
      topAggr->aggregates.evaluate(pos);
      processedTuples += vectorSize;
      //   add delay to test typer
      //   std::this_thread::sleep_for(1ms);
   }
   std::cout << "TW processed " << processedTuples << " tuples out of "
             << rel.nrTuples << std::endl;
   auto end = std::chrono::steady_clock::now();
   std::cout << "TW took "
             << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                      start)
                    .count()
             << " milliseconds." << std::endl;

   // store TW results
   auto& revenue =
       result["revenue"].typedAccessForChange<types::Numeric<12, 4>>();
   revenue.reset(1);
   auto agg = types::Numeric<12, 4>(query->aggregator);
   revenue.push_back(agg);
   result.nrTuples = 1;

   // process remaining tuples with typer
   compilationThread.join();
   start = std::chrono::steady_clock::now();
   if (processedTuples < rel.nrTuples) {
      // load library
      if (!typerLib) {
         std::cerr << "Could not load shared Typer library!" << std::endl;
         std::exit(1);
      }
      // get compiled function
      const std::string& funcName =
          "_Z17compiled_typer_q6RN7runtime8DatabaseEmm";
      CompiledTyperQuery typer_q6 =
          typerLib.load()->getFunction<CompiledTyperQuery>(funcName);
      if (!typer_q6) {
         std::cerr << "Could not find function for running Q6 in Typer!"
                   << std::endl;
         std::exit(1);
      }
      // compute typer result
      Relation typer_result = typer_q6(db, nrThreads, processedTuples);

      // merge results
      auto& typerRevenue =
          typer_result["revenue"].typedAccessForChange<types::Numeric<12, 4>>();
      for (size_t i = 0; i < revenue.size(); ++i) {
         revenue[i] += typerRevenue[i];
      }
   }
   end = std::chrono::steady_clock::now();
   std::cout << "Typer took "
             << std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                      start)
                    .count()
             << " milliseconds." << std::endl;

   delete typerLib;
   return result;
}

using namespace std;
NOVECTORIZE Relation q6_hyper(Database& db, size_t /*nrThreads*/) {
   Relation result;
   result.insert("revenue", make_unique<algebra::Numeric>(12, 4));

   // --- constants
   auto c1 = types::Date::castString("1994-01-01");
   auto c2 = types::Date::castString("1995-01-01");
   auto c3 = types::Numeric<12, 2>::castString("0.05");
   auto c4 = types::Numeric<12, 2>::castString("0.07");
   auto c5 = types::Integer(24);

   // --- aggregates
   types::Numeric<12, 4> revenue = 0;

   // --- scan
   auto& rel = db["lineitem"];
   auto l_shipdate_col = rel["l_shipdate"].data<types::Date>();
   auto l_quantity_col = rel["l_quantity"].data<types::Numeric<12, 2>>();
   auto l_extendedprice_col =
       rel["l_extendedprice"].data<types::Numeric<12, 2>>();
   auto l_discount_col = rel["l_discount"].data<types::Numeric<12, 2>>();

   revenue = tbb::parallel_reduce(
       tbb::blocked_range<size_t>(0, rel.nrTuples), types::Numeric<12, 4>(0),
       [&](const tbb::blocked_range<size_t>& r,
           const types::Numeric<12, 4>& s) {
          auto revenue = s;
          for (size_t i = r.begin(), end = r.end(); i != end; ++i) {
             auto& l_shipdate = l_shipdate_col[i];
             auto& l_quantity = l_quantity_col[i];
             auto& l_extendedprice = l_extendedprice_col[i];
             auto& l_discount = l_discount_col[i];

             if ((l_shipdate >= c1) & (l_shipdate < c2) & (l_quantity < c5) &
                 (l_discount >= c3) & (l_discount <= c4)) {
                // --- aggregation
                revenue += l_extendedprice * l_discount;
             }
          }
          return revenue;
       },
       [](const types::Numeric<12, 4>& x, const types::Numeric<12, 4>& y) {
          return x + y;
       });

   // --- output
   auto& rev = result["revenue"].typedAccessForChange<types::Numeric<12, 4>>();
   rev.reset(1);
   rev.push_back(revenue);
   result.nrTuples = 1;
   return result;
}

unique_ptr<Q6Builder::Q6> Q6Builder::getQuery() {
   using namespace vectorwise;
   // --- constants
   auto res = make_unique<Q6>();
   auto& consts = *res;
   enum { sel_a, sel_b, result_project };

   assert(db["lineitem"]["l_shipdate"].type->rt_size() == sizeof(consts.c2));
   assert(db["lineitem"]["l_quantity"].type->rt_size() == sizeof(consts.c5));
   assert(db["lineitem"]["l_discount"].type->rt_size() == sizeof(consts.c3));
   assert(db["lineitem"]["l_extendedprice"].type->rt_size() == sizeof(int64_t));

   auto lineitem = Scan("lineitem");
   Select(
       (Expression()                                        //
            .addOp(conf.sel_less_int32_t_col_int32_t_val(), //
                   Buffer(sel_a, sizeof(pos_t)),            //
                   Column(lineitem, "l_shipdate"),          //
                   Value(&consts.c2)))
           .addOp(conf.selsel_greater_equal_int32_t_col_int32_t_val(), //
                  Buffer(sel_a, sizeof(pos_t)),                        //
                  Buffer(sel_b, sizeof(pos_t)),                        //
                  Column(lineitem, "l_shipdate"),                      //
                  Value(&consts.c1))
           .addOp(
               conf.selsel_less_int64_t_col_int64_t_val(), //
                                                           // .addOp(primitives::selsel_less_int64_t_col_int64_t_val_bf,
                                                           // //
               Buffer(sel_b, sizeof(pos_t)),   //
               Buffer(sel_a, sizeof(pos_t)),   //
               Column(lineitem, "l_quantity"), //
               Value(&consts.c5))
           .addOp(
               conf.selsel_greater_equal_int64_t_col_int64_t_val(), //
                                                                    //.addOp(primitives::selsel_greater_equal_int64_t_col_int64_t_val_bf,
                                                                    ////
               Buffer(sel_a, sizeof(pos_t)),   //
               Buffer(sel_b, sizeof(pos_t)),   //
               Column(lineitem, "l_discount"), //
               Value(&consts.c3))
           .addOp(conf.selsel_less_equal_int64_t_col_int64_t_val(), //
                  Buffer(sel_b, sizeof(pos_t)),                     //
                  Buffer(sel_a, sizeof(pos_t)),                     //
                  Column(lineitem, "l_discount"),                   //
                  Value(&consts.c4)));
   Project().addExpression(
       Expression() //
           .addOp(primitives::proj_sel_both_multiplies_int64_t_col_int64_t_col,
                  Buffer(sel_a),                           //
                  Buffer(result_project, sizeof(int64_t)), //
                  Column(lineitem, "l_discount"),
                  Column(lineitem, "l_extendedprice")));
   FixedAggregation(Expression() //
                        .addOp(primitives::aggr_static_plus_int64_t_col,
                               Value(&consts.aggregator), //
                               Buffer(result_project)));
   res->rootOp = popOperator();
   assert(operatorStack.size() == 0);
   return res;
}

Relation q6_vectorwise(Database& db, size_t nrThreads, size_t vectorSize) {
   using namespace vectorwise;

   std::atomic<size_t> n;
   runtime::Relation result;
   vectorwise::SharedStateManager shared;
   WorkerGroup workers(nrThreads);
   GlobalPool pool;
   std::atomic<int64_t> aggr;
   aggr = 0;
   n = 0;
   workers.run([&]() {
      Q6Builder b(db, shared, vectorSize);
      b.previous = this_worker->allocator.setSource(&pool);
      auto query = b.getQuery();
      auto n_ = query->rootOp->next();
      if (n_) {
         aggr.fetch_add(query->aggregator);
         n.fetch_add(n_);
      }

      auto leader = barrier();
      if (leader) {
         result.insert("revenue", make_unique<algebra::Numeric>(12, 4));
         if (n.load()) {
            auto& sum =
                result["revenue"].template typedAccessForChange<int64_t>();
            sum.reset(1);
            auto a = aggr.load();
            sum.push_back(a);
            result.nrTuples = 1;
         }
      }
   });

   return result;
}
