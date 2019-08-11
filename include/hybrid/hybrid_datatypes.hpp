#pragma once
#include "common/runtime/Database.hpp"
#include "common/runtime/Types.hpp"
#include <tuple>

namespace hybrid {

// Q1
typedef std::unique_ptr<runtime::Query> (*CompiledTyperQ1)(
    runtime::Database&, size_t, size_t,
    std::unordered_map<std::thread::id, runtime::PartitionedDeque<1024>>&);
typedef std::tuple<types::Char<1>, types::Char<1>> Q1TyperKey;
typedef std::tuple<types::Numeric<12, 2>, types::Numeric<12, 2>,
                   types::Numeric<12, 4>, types::Numeric<12, 6>, int64_t>
    Q1TyperValue;

typedef struct {
   long idk; // PS: I don't know what this entry means, it doesn't correspond to
             // any of the expected values
   char returnflag[1];
   char linestatus[1];
   long sum_disc_price;
   long sum_charge;
   long sum_qty;
   long sum_base_price;
   long count_order;
} Q1TectorTuple;

// Q6
typedef runtime::Relation (*CompiledTyperQ6)(runtime::Database&, size_t, size_t,
                                             int64_t);

// Q18
typedef std::unique_ptr<runtime::Query> (*CompiledTyperQ18)(
    runtime::Database&, size_t, size_t,
    std::unordered_map<std::thread::id, runtime::PartitionedDeque<1024>>&);
typedef std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,
                   types::Char<25>, types::Integer>
    Q18TyperKey;
typedef types::Numeric<12U, 2U> Q18TyperValue;

typedef struct __attribute__((packed)) {
   int8_t idk1[9];
   char c_name[25];
   int32_t c_custkey;
   int32_t o_orderdate;
   int64_t o_totalprice;
   int32_t o_orderkey;
   int64_t sum;
   int8_t idk2[2];
} Q18TectorTuple;
} // namespace hybrid
