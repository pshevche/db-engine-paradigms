#include "hybrid/minimal_api.hpp"
using namespace runtime;
using namespace std;
using namespace types;
std::unique_ptr<runtime::Query> pure_typer_q18(Database& db, size_t nrThreads) {
    using namespace types;
    using namespace std;
    using hash = runtime::CRC32Hash;
    auto resources = initQuery(nrThreads);
    auto& li = db["lineitem"];
    auto l_orderkey = li["l_orderkey"].data<types::Integer>();
    auto l_quantity = li["l_quantity"].data<types::Numeric<12, 2>>();
    tbb::enumerable_thread_specific<Hashmapx<types::Integer, types::Numeric<12, 2>, hash, false>>groups;
    const auto zero = types::Numeric<12, 2>::castString("0.00");
    auto groupOp = make_GroupBy<types::Integer, types::Numeric<12, 2>, hash>([](auto& acc, auto&& value) {
        acc += value;
    }, zero, nrThreads);// scan lineitem and group by l_orderkey
    tbb::parallel_for(tbb::blocked_range<size_t>(0, li.nrTuples, morselSize),[&](const tbb::blocked_range<size_t>& r) {
        auto locals = groupOp.preAggLocals();
        for (size_t i = r.begin(), end = r.end(); i != end; ++i) {
            auto& group = locals.getGroup(l_orderkey[i]);
            group += l_quantity[i];// locals.consume(l_orderkey[i], l_quantity[i]);
        }
    });
    Hashset<types::Integer, hash> ht1;
    tbb::enumerable_thread_specific<runtime::Stack<decltype(ht1)::Entry>>entries1;
    const auto threeHundret = types::Numeric<12, 2>::castString("300");
    std::atomic<size_t> nrGroups;
    nrGroups = 0;
    groupOp.forallGroups([&](auto& groups) {
        auto& entries = entries1.local();
        size_t groupsFound = 0;
        for (auto block : groups)for (auto& group : block)if (group.v > threeHundret) {
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
    tbb::enumerable_thread_specific<runtime::Stack<decltype(ht2)::Entry>>entries2;
    PARALLEL_SCAN(cu.nrTuples, entries2, {entries.emplace_back(ht2.hash(c_custkey[i]), c_custkey[i], c_name[i]);});
    ht2.setSize(cu.nrTuples);
    parallel_insert(entries2, ht2);
// build last hashtable
    Hashmapx<types::Integer,std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,types::Char<25>>,hash>ht3;
    tbb::enumerable_thread_specific<runtime::Stack<decltype(ht3)::Entry>>entries3;
    auto& ord = db["orders"];
    auto o_orderkey = ord["o_orderkey"].data<types::Integer>();
    auto o_custkey = ord["o_custkey"].data<types::Integer>();
    auto o_orderdate = ord["o_orderdate"].data<types::Date>();
    auto o_totalprice = ord["o_totalprice"].data<types::Numeric<12, 2>>();
// scan orders
    auto found = PARALLEL_SELECT(ord.nrTuples, entries3, {types::Char<25>* name;
// check if it matches the order criteria and look up the customer name
    if (ht1.contains(o_orderkey[i]) && (name = ht2.findOne(o_custkey[i]))) {
    entries.emplace_back(ht3.hash(o_orderkey[i]), o_orderkey[i],make_tuple(o_custkey[i], o_orderdate[i], o_totalprice[i], *name));
        found++;
    }
                                                         });
    ht3.setSize(found);
    parallel_insert(entries3, ht3);
    auto finalGroupOp = make_GroupBy<std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,types::Char<25>, types::Integer>,types::Numeric<12, 2>, hash>([](auto& acc, auto&& value) {
        acc += value;
    }, zero, nrThreads);
// scan lineitem and group by l_orderkey
    tbb::parallel_for(tbb::blocked_range<size_t>(0, li.nrTuples, morselSize),[&](const tbb::blocked_range<size_t>& r) {
        auto locals = finalGroupOp.preAggLocals();
        for (size_t i = r.begin(), end = r.end(); i != end; ++i) {
            std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,types::Char<25>>* v;
            if ((v = ht3.findOne(l_orderkey[i]))) {
                auto& group =locals.getGroup(tuple_cat(*v, make_tuple(l_orderkey[i])));
                group += l_quantity[i];
            }
        }
    });
    auto& result = resources.query->result;
    auto namAttr = result->addAttribute("c_name", sizeof(types::Char<25>));
    auto ckyAttr = result->addAttribute("c_custkey", sizeof(types::Integer));
    auto okyAttr = result->addAttribute("o_orderkey", sizeof(types::Integer));
    auto datAttr = result->addAttribute("o_orderdate", sizeof(types::Date));
    auto totAttr =result->addAttribute("o_totalprice", sizeof(types::Numeric<12, 2>));
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
        for (auto block : groups)for (auto& group : block) {
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
    return move(resources.query);
}