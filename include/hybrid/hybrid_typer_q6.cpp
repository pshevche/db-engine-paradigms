#include "hybrid/minimal_api.hpp"
using namespace runtime;
using namespace std;
Relation hybrid_typer_q6(Database& db, size_t nrThreads, size_t firstTuple, size_t columnID, int64_t* twRevenue) {

    Relation result;
    result.insert("revenue", make_unique<algebra::Numeric>(12,4));
    auto c1 = types::Date::castString("1994-01-01");
    auto c2 = types::Date::castString("1995-01-01");
    auto c3 = types::Numeric<12, 2>::castString("0.05");
    auto c4 = types::Numeric<12, 2>::castString("0.07");
    auto c5 = types::Integer(24);

    auto& rel = db["lineitem"];
    auto l_shipdate_col = rel["l_shipdate"].data<types::Date>();
    auto l_quantity_col = rel["l_quantity"].data<types::Numeric<12, 2>>();
    auto l_extendedprice_col = rel["l_extendedprice"].data<types::Numeric<12,2 >>();
    auto l_discount_col = rel["l_discount"].data<types::Numeric<12,2>>();

    types::Numeric<12, 4> revenue(twRevenue[columnID]);//initialize the aggregates with partial results from vectorized execution

    //In case all the values are processed in vectorized execution itself
    if(firstTuple == rel.nrTuples) {
        auto& rev = result["revenue"].typedAccessForChange<types::Numeric<12,4>>();
        rev.reset(1);
        rev.push_back(revenue);
        result.nrTuples = 1;
        return result;
    }

    revenue += tbb::parallel_reduce(
                   tbb::blocked_range<size_t>(firstTuple, rel.nrTuples),
                   types::Numeric<12, 4>(0),
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
                revenue += l_extendedprice * l_discount;
            }
        }
        return revenue;
    },
    [](const types::Numeric<12, 4>& x, const types::Numeric<12, 4>& y) {
        return x + y;
    });
    auto& rev = result["revenue"].typedAccessForChange<types::Numeric<12,4>>();
    rev.reset(1);
    rev.push_back(revenue);
    result.nrTuples = 1;
    return result;
}