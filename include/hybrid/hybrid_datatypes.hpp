#ifndef HYBRID_DATATYPES
#define HYBRID_DATATYPES

#include "common/runtime/Types.hpp"
#include <tuple>

namespace hybrid {
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
} Q18TectorTuple;
} // namespace hybrid

#endif