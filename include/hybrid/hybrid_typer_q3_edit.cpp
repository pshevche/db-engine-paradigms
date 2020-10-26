//
// Created by gurumurt on 12/09/19.
//

#include "hybrid/minimal_api.hpp"
using namespace runtime;
using namespace std;
using namespace types;
std::unique_ptr<runtime::Query> hybrid_typer_q3(
                                                    Database & db,
                                                    size_t nrThreads
                                                  ,runtime::Hashmap & twCustomerHT,
                                                  size_t firstTuple
                                                  ) {

    using namespace types;
    using namespace std;
    using hash = runtime::MurMurHash;
    auto resources = initQuery(nrThreads);
    // --- aggregates

    // --- constants
    auto c1 = types::Date::castString("1995-03-15");
    auto c2 = types::Date::castString("1995-03-15");
    string b = "BUILDING";
    auto c3 = types::Char<10>::castString(b.data(), b.size());

    auto& cu = db["customer"];
    auto& ord = db["orders"];
    auto& li = db["lineitem"];

    auto c_mktsegment = cu["c_mktsegment"].data<types::Char<10>>();
    auto c_custkey = cu["c_custkey"].data<types::Integer>();
    auto o_custkey = ord["o_custkey"].data<types::Integer>();
    auto o_orderkey = ord["o_orderkey"].data<types::Integer>();
    auto o_orderdate = ord["o_orderdate"].data<types::Date>();
    auto o_shippriority = ord["o_shippriority"].data<types::Integer>();
    auto l_orderkey = li["l_orderkey"].data<types::Integer>();
    auto l_shipdate = li["l_shipdate"].data<types::Date>();
    auto l_extendedprice =
            li["l_extendedprice"].data<types::Numeric<12, 2>>();
    auto l_discount = li["l_discount"].data<types::Numeric<12, 2>>();

    using range = tbb::blocked_range<size_t>;

    const auto add = [](const size_t& a, const size_t& b) {
        return a + b;
    };
    const size_t morselSize = 100000;

    // join and build second ht
    Hashmapx<types::Integer, std::tuple<types::Date, types::Integer>, hash> ht2;
    tbb::enumerable_thread_specific<runtime::Stack<decltype(ht2)::Entry>>
            entries2;
    auto found2 = tbb::parallel_reduce(
            range(0, ord.nrTuples, morselSize), 0,
            [&](
                    const tbb::blocked_range<size_t>& r, const size_t& f) {
                auto& entries = entries2.local();
                auto found = f;
                for (size_t i = r.begin(), end = r.end(); i != end; ++i)
                    if(o_orderdate[i] < c1) {
                        runtime::CRC32Hash h1;
                        const defs::hash_t seed = 902850234;
                        uint64_t output = h1(o_custkey[i],seed);
                        for (auto entry = twCustomerHT.find_chain_tagged(output); entry !=
                                                                                runtime::Hashmap::end(); entry = reinterpret_cast<runtime::Hashmap::EntryHeader *>(entry->next)) {

                            if (entry->hash == output) {
                                entries.emplace_back(
                                        ht2.hash(o_orderkey[i]), o_orderkey[i],
                                        make_tuple(o_orderdate[i], o_shippriority[i]));
                                found++;
                            }
                        }
                    }
                return found;
            },
            add);
    if(found2){
        ht2.setSize(found2);
        parallel_insert(entries2, ht2);
    }
    else{
        ht2.setSize(1);
        parallel_insert(entries2, ht2);
    }

    const auto one = types::Numeric<12, 2>::castString("1.00");
    const auto zero = types::Numeric<12, 4>::castString("0.00");

    tbb::enumerable_thread_specific<
            Hashmapx<std::tuple<types::Integer, types::Date, types::Integer>,
                    types::Numeric<12, 4>, hash, false>>
            groups;

    auto groupOp =
            make_GroupBy<std::tuple<types::Integer, types::Date, types::Integer>,
                    types::Numeric<12, 4>, hash>(
                    [](auto& acc, auto&& value) { acc += value; }, zero, nrThreads);

    // preaggregation
    tbb::parallel_for(
            tbb::blocked_range<size_t>(0, li.nrTuples, morselSize),
            [&](const tbb::blocked_range<size_t>& r) {
                auto locals = groupOp.preAggLocals();

                for (size_t i = r.begin(), end = r.end(); i != end; ++i) {
                    decltype(ht2)::value_type* v;
                    if (l_shipdate[i] > c2 && (v = ht2.findOne(l_orderkey[i]))) {
                        locals.consume(
                                make_tuple(l_orderkey[i], get<0>(*v), get<1>(*v)),
                                l_extendedprice[i] * (one - l_discount[i]));
                    }
                }
            });

    // --- output
    auto& result = resources.query->result;
    auto revAttr =
            result->addAttribute("revenue", sizeof(types::Numeric<12, 4>));
    auto orderAttr = result->addAttribute("l_orderkey", sizeof(types::Integer));
    auto dateAttr = result->addAttribute("o_orderdate", sizeof(types::Date));
    auto prioAttr =
            result->addAttribute("o_shippriority", sizeof(types::Integer));

    groupOp.forallGroups([&](auto& entries) {
        // write aggregates to result
        auto n = entries.size();
        auto block = result->createBlock(n);
        auto rev = reinterpret_cast<types::Numeric<12, 4>*>(block.data(revAttr));
        auto order = reinterpret_cast<types::Integer*>(block.data(orderAttr));
        auto date = reinterpret_cast<types::Date*>(block.data(dateAttr));
        auto prio = reinterpret_cast<types::Integer*>(block.data(prioAttr));
        for (auto block : entries)
            for (auto& entry : block) {
                *order++ = get<0>(entry.k);
                *date++ = get<1>(entry.k);
                *prio++ = get<2>(entry.k);
                *rev++ = entry.v;
            }
        block.addedElements(n);
    });

    leaveQuery(nrThreads);
    return move(resources.query);
}