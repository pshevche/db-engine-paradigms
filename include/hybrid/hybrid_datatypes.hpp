#pragma once
#include "common/runtime/Database.hpp"
#include "common/runtime/Hashmap.hpp"
#include "common/runtime/Types.hpp"
#include <tuple>

namespace hybrid {


//Pure Q1
typedef std::unique_ptr<runtime::Query> (*PureTyperQ1)(runtime::Database&, size_t);

//Pure Q3
typedef std::unique_ptr<runtime::Query> (*PureTyperQ3)(runtime::Database&, size_t);

//Pure Q6
typedef runtime::Relation (*PureTyperQ6)(runtime::Database&, size_t);

//Pure Q18
typedef std::unique_ptr<runtime::Query> (*PureTyperQ18)(runtime::Database&, size_t);


// Q1
typedef std::unique_ptr<runtime::Query> (*CompiledTyperQ1)(
    runtime::Database&, size_t, size_t,
    std::unordered_map<std::thread::id, runtime::PartitionedDeque<1024>>&);

// partial Q1
typedef std::unique_ptr<runtime::Query> (*CompiledPartialTyperQ1)(
    runtime::Database&, size_t, size_t);

typedef std::tuple<types::Char<1>, types::Char<1>> Q1TyperKey;

typedef std::tuple<types::Numeric<12, 2>, types::Numeric<12, 2>,
                   types::Numeric<12, 4>, types::Numeric<12, 6>, int64_t> Q1TyperValue;

typedef struct {
   long idk; // PS: I don't know what this entry means, it doesn't correspond to
             // any of the expected values --> this is the next pointer value in the EntryHeader. This can be ignored if we cast the hash value directly
   char returnflag[1];
   char linestatus[1];
   long sum_disc_price;
   long sum_charge;
   long sum_qty;
   long sum_base_price;
   long count_order;
} Q1TectorTuple;

//Q3
typedef struct {
    long idk; // PS: I don't know what this entry means, it doesn't correspond to
    // any of the expected values --> this is the next pointer value in the EntryHeader. This can be ignored if we cast the hash value directly
    int l_orderkey;
    types::Date o_orderdate;
    int o_shippriority;
    long revenue;

} Q3TectorTuple;

// Q6
typedef runtime::Relation (*CompiledTyperQ6)(runtime::Database&, size_t, size_t, size_t, int64_t*);

// Q18
typedef std::unique_ptr<runtime::Query> (*CompiledTyperQ18)(
    runtime::Database&, size_t, runtime::Hashmap&,
    std::unordered_map<std::thread::id, runtime::PartitionedDeque<1024>>&,
    size_t);

    typedef std::unique_ptr<runtime::Query> (*CompiledAnotherTyperQ18)(
            runtime::Database&, size_t, runtime::Hashmap&,runtime::Hashmap&,
            size_t);

typedef struct __attribute__((packed)) {
   uint8_t idk[8];
   int32_t l_orderkey;
   int64_t l_quantity;
   uint8_t idk2[4];
} Q18TWAggrTuple;

typedef struct __attribute__((packed)) { //48 Bytes in total
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

//Adding information for Q3
    typedef std::unique_ptr<runtime::Query> (*CompiledAnotherTyperQ3)(
            runtime::Database&, size_t,
            runtime::Hashmap &,
            runtime::Hashmap &,
            size_t
    );

//Adding information for Q3
    typedef std::unique_ptr<runtime::Query> (*CompiledYetAnotherTyperQ3)(
            runtime::Database&, size_t,
            runtime::Hashmap &,
            std::unordered_map<std::thread::id, runtime::PartitionedDeque<1024>> &,
            size_t
    );

//Generic hash join connector
typedef std::unique_ptr<runtime::Query> (*GenericHashJoin)(
        runtime::Database&, size_t, runtime::Hashmap&, size_t
        );

//Generic hash group connector
typedef std::unique_ptr<runtime::Query> (*GenericHashGroup)(
        runtime::Database&, size_t, size_t,
        std::unordered_map<std::thread::id, runtime::PartitionedDeque<1024>>&
        );

//Generic hash join and hash group connector
typedef std::unique_ptr<runtime::Query> (*GenericHashJoinGroup)(
        runtime::Database&, size_t, runtime::Hashmap&, size_t, runtime::Hashmap&, size_t
        );

//Generic fixed aggregation connector
//TODO: change the return type from runtime::Relation to std::unique_ptr<runtime::Query>
typedef runtime::Relation (*GenericGroup)(
            runtime::Database&, size_t, size_t, int64_t
);

//typedef struct __attribute__((packed)) {
//
//    long idk[1]; //This is because the hash is packed as 64 byte values <- 4 bytes
//    int32_t custkey; //remaining 4 bytes here
//    long idk2[4];
//} Q3TWJoinTuple;
//
//typedef struct __attribute__((packed)) {
//    long zero; //4
//    char idk2[8];// 8
//    int32_t custkey; //4
//    long idk3[2];//8
//} Q3TWJoinTupleExtended;

} // namespace hybrid
