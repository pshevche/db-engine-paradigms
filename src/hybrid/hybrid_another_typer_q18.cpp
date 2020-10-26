/**
 * parallel version - Q18
 */

#include "hybrid/minimal_api.hpp"
using namespace runtime;
using namespace std;
using namespace types;
std::unique_ptr<runtime::Query> hybrid_typer_q18(Database & db, size_t nrThreads, runtime::Hashmap & twCustomerHT,runtime::Hashmap & twLineitemHT, size_t firstTuple) {

    auto total_time = std::chrono::steady_clock::now();
    auto q3_hyper_start = std::chrono::steady_clock::now();
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
    }, zero, nrThreads);

    auto p1_start = std::chrono::steady_clock::now();

    auto p2_start = std::chrono::steady_clock::now();

    auto p3_start = std::chrono::steady_clock::now();


    auto p4_start = std::chrono::steady_clock::now();

// build last hashtable
    Hashmapx<types::Integer,std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,types::Char<25>>,hash>ht3;
    tbb::enumerable_thread_specific<runtime::Stack<decltype(ht3)::Entry>>entries3;
    auto& ord = db["orders"];
    auto o_orderkey = ord["o_orderkey"].data<types::Integer>();
    auto o_custkey = ord["o_custkey"].data<types::Integer>();
    auto o_orderdate = ord["o_orderdate"].data<types::Date>();
    auto o_totalprice =ord["o_totalprice"].data<types::Numeric<12, 2>>();
    Hashmapx<types::Integer, types::Char<25>, hash> ht2;

    const auto add = [](const size_t& a, const size_t& b) {
        return a + b;
    };

    auto& cu = db["customer"];
    auto c_name = cu["c_name"].data<types::Char<25>>();

// Probe both tables from vectorwise here
    auto time_merge_start = std::chrono::steady_clock::now();
    using range = tbb::blocked_range<size_t>;
    auto found0 = tbb::parallel_reduce(
                      range(0, ord.nrTuples, morselSize), 0,
                      [&](
    const tbb::blocked_range<size_t>& r, const size_t& f) {
        auto& entries = entries3.local();
        auto found = f;
        for (size_t i = r.begin(), end = r.end(); i != end; ++i) {

            types::Char<25>* name;
            runtime::CRC32Hash h0;
            const defs::hash_t seed = 902850234;
            uint64_t output1 = h0(o_orderkey[i],seed);
//                    for (auto entry = twLineitemHT.find_chain_tagged(output1);
//                    entry != runtime::Hashmap::end(); entry = reinterpret_cast<runtime::Hashmap::EntryHeader*>(entry->next)) {

//                        runtime::CRC32Hash h1;
//                        const defs::hash_t seed = 902850234;
//                        uint64_t output2 = h1(o_custkey[i],seed);
//                        for (auto entry = twCustomerHT.find_chain_tagged(output2); entry != runtime::Hashmap::end(); entry = reinterpret_cast<runtime::Hashmap::EntryHeader*>(entry->next)) {
//                            auto t = reinterpret_cast<hybrid::Q18TWJoinTuple*>(entry);
//                            if (types::Integer(t->c_custkey) == o_custkey[i]) {
//                                name = &c_name[i];
//                            }
//                        }
//                        if (name != nullptr) {
//                            entries.emplace_back(ht3.hash(o_orderkey[i]), o_orderkey[i],make_tuple(o_custkey[i], o_orderdate[i],o_totalprice[i], *name));
//                            found++;
//                        }
        }
//                }
        return found;
    }, add);

    std::cout<<"h343"<<std::endl;
//    auto found = PARALLEL_SELECT(ord.nrTuples, entries3, {char* name;
//// check if it matches the order criteria and look up the customer name
//        runtime::CRC32Hash h1;
//        const defs::hash_t seed = 902850234;
//        auto hash1 =  h1(o_orderkey[i],seed);
//        for (auto entry = twLineitemHT.find_chain_tagged(hash1); entry != runtime::Hashmap::end(); entry = reinterpret_cast<runtime::Hashmap::EntryHeader*>(entry->next)) {
//            auto hash  =  h1(o_custkey[i],seed); //= ht2.hash(o_custkey[i]);
//            for (auto entry = twCustomerHT.find_chain_tagged(hash); entry != runtime::Hashmap::end(); entry = reinterpret_cast<runtime::Hashmap::EntryHeader*>(entry->next)) {
//                auto t = reinterpret_cast<hybrid::Q18TWJoinTuple*>(entry);
//                if (types::Integer(t->c_custkey) == o_custkey[i]) {
//                    name = t->c_name;
//                }
//            }
//            if (name != nullptr) {
//                entries.emplace_back(ht3.hash(o_orderkey[i]), o_orderkey[i],make_tuple(o_custkey[i], o_orderdate[i],o_totalprice[i], types::Char<25>::build(name)));
//                found++;
//            }
//        }
//    });
    ht3.setSize(found0);
    parallel_insert(entries3, ht3);
    std::cout<<"here"<<std::endl;
    auto p5_start = std::chrono::steady_clock::now();

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
    auto total_time_end = std::chrono::steady_clock::now();
    std::cout
            << "P1: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(p2_start - p1_start)
            .count()<<","
            << "Merge aggregate: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(p3_start - p2_start)
            .count()<<","
            << "P2: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(p4_start - p3_start)
            .count()<<","
            << "Merge hashing (+ P3): "
            << std::chrono::duration_cast<std::chrono::milliseconds>(p5_start
                    - p4_start)
            .count()<<","
            << "P4: "
            << std::chrono::duration_cast<std::chrono::milliseconds>(total_time_end -
                    p5_start)
            .count()<<","
            << "Total time "
            << std::chrono::duration_cast<std::chrono::milliseconds>(
                total_time_end  - total_time)
            .count()
            << " milliseconds." << std::endl;
    leaveQuery(nrThreads);
    return move(resources.query);
}