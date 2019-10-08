#pragma once
#include "common/runtime/Database.hpp"
#include "common/runtime/Hashmap.hpp"
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
    runtime::Database&, size_t, runtime::Hashmap&,
    std::unordered_map<std::thread::id, runtime::PartitionedDeque<1024>>&,
    size_t);

typedef struct __attribute__((packed)) {
   uint8_t idk[8];
   int32_t l_orderkey;
   int64_t l_quantity;
   uint8_t idk2[4];
} Q18TWAggrTuple;

typedef struct __attribute__((packed)) {
   uint8_t idk1[16];
   int32_t c_custkey;
   uint8_t idk2[1];
   char c_name[25];
   uint8_t idk3[2];
} Q18TWJoinTuple;

//Adding information for Q3
    typedef std::unique_ptr<runtime::Query> (*CompiledTyperQ3)(
            runtime::Database&, size_t, runtime::Hashmap&,
            size_t
            );

    typedef struct __attribute__((packed)) {

        int8_t hash[8];
        int32_t orderdate;
        int32_t dummy;
        int32_t custkey;

    } Q3TWJoinTuple;

} // namespace hybrid
