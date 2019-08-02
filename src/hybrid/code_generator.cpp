#include "hybrid/code_generator.hpp"
#include <fstream>
#include <sstream>

namespace hybrid {

CodeGenerator::CodeGenerator() {}

CodeGenerator& CodeGenerator::instance() {
   static CodeGenerator ce;
   return ce;
}

const std::string CodeGenerator::generateTyperQ1() {
   // generate code
   const std::string filename = "src/hybrid/typer_q1";
   const std::string cppName = filename + ".cpp";
   std::ofstream f(cppName);
   f << "#include \"hybrid/minimal_api.hpp\"\n";
   f << "using namespace runtime;\n";
   f << "using namespace std;\n";
   f << "using namespace types;\n";
   f << "std::unique_ptr<runtime::Query> compiled_typer_q1(Database& db, "
        "size_t nrThreads, size_t "
        "firstTuple, std::unordered_map<std::thread::id, "
        "runtime::PartitionedDeque<1024>>&"
        "twThreadData) {\n ";
   f << "\n// prepare query data\n";
   f << "types::Date c1 = types::Date::castString(\"1998-09-02\");";
   f << "types::Numeric<12, 2> one = types::Numeric<12, "
        "2>::castString(\"1.00\");";
   f << "auto& li = db[\"lineitem\"];";
   f << "auto l_returnflag = li[\"l_returnflag\"].data<types::Char<1>>();";
   f << "auto l_linestatus = li[\"l_linestatus\"].data<types::Char<1>>();";
   f << "auto l_extendedprice = "
        "li[\"l_extendedprice\"].data<types::Numeric<12, 2>>();";
   f << "auto l_discount = li[\"l_discount\"].data<types::Numeric<12, "
        "2>>();";
   f << "auto l_tax = li[\"l_tax\"].data<types::Numeric<12, 2>>();";
   f << "auto l_quantity = li[\"l_quantity\"].data<types::Numeric<12, "
        "2>>();";
   f << "auto l_shipdate = li[\"l_shipdate\"].data<types::Date>();";
   f << "\n\n// setup parallel execution + prepare result\n";
   f << "auto resources = initQuery(nrThreads);";
   f << "using hash = runtime::CRC32Hash;";
   f << "\n\n// define the aggregation\n";
   f << "auto groupOp = make_GroupBy<tuple<Char<1>, Char<1>>,";
   f << "tuple<Numeric<12, 2>, Numeric<12, 2>,";
   f << "Numeric<12, 4>, Numeric<12, 6>, int64_t>,";
   f << "hash>(";
   f << "[](auto& acc, auto&& value) {";
   f << "get<0>(acc) += get<0>(value);";
   f << "get<1>(acc) += get<1>(value);";
   f << "get<2>(acc) += get<2>(value);";
   f << "get<3>(acc) += get<3>(value);";
   f << "get<4>(acc) += get<4>(value);";
   f << "},";
   f << "make_tuple(Numeric<12, 2>(), Numeric<12, 2>(), Numeric<12, 4>(),";
   f << "Numeric<12, 6>(), int64_t(0)),";
   f << "nrThreads);";
   f << "\n\n// compute typer's partial thread-local hash tables\n";
   f << "tbb::parallel_for(";
   f << "tbb::blocked_range<size_t>(firstTuple, li.nrTuples, morselSize),";
   f << "[&](const tbb::blocked_range<size_t>& r) {";
   f << "auto locals = groupOp.preAggLocals();";
   f << "for (size_t i = r.begin(), end = r.end(); i != end; ++i) {";
   f << "if (l_shipdate[i] <= c1) {";
   f << "auto& group =";
   f << "locals.getGroup(make_tuple(l_returnflag[i], l_linestatus[i]));";
   f << "get<0>(group) += l_quantity[i];";
   f << "get<1>(group) += l_extendedprice[i];";
   f << "auto disc_price = l_extendedprice[i] * (one - l_discount[i]);";
   f << "get<2>(group) += disc_price;";
   f << "auto charge = disc_price * (one + l_tax[i]);";
   f << "get<3>(group) += charge;";
   f << "get<4>(group) += 1;";
   f << "}";
   f << "}";
   f << "});";
   f << "\n\n// merge tw's partial thread-local hash tables with typer's "
        "partial "
        "thread-local hash tables\n";
   f << "tbb::parallel_for_each(twThreadData.begin(), twThreadData.end(), "
        "[&](auto& threadPartitions) {auto locals = "
        "groupOp.preAggLocals();tbb::parallel_for_each(threadPartitions.second."
        "getPartitions().begin(),threadPartitions.second.getPartitions().end(),"
        " [&](auto& partition) {for (auto chunk = partition.first; chunk; "
        "chunk = chunk->next) {auto elementSize = "
        "threadPartitions.second.entrySize;auto nPart = partition.size(chunk, "
        "elementSize);auto data = chunk->template "
        "data<hybrid::Q1TectorTuple>();for (unsigned i "
        "= 0; i < "
        "nPart; ++i) {hybrid::Q1TectorTuple t "
        "=data[i];hybrid::Q1TyperKey key "
        "=std::make_tuple(types::Char<1>::build(t.returnflag),types::Char<1>::"
        "build(t.linestatus));hybrid::Q1TyperValue value = "
        "std::make_tuple(types::Numeric<12, 2>(t.sum_qty),types::Numeric<12, "
        "2>(t.sum_base_price),types::Numeric<12, "
        "4>(t.sum_disc_price),types::Numeric<12, 6>(t.sum_charge), "
        "t.count_order);locals.consume(key, value);}}});});";
   f << "\n\n// materialize final result\n";
   f << "auto& result = resources.query->result;";
   f << "auto retAttr = result->addAttribute(\"l_returnflag\", "
        "sizeof(Char<1>));";
   f << "auto statusAttr = result->addAttribute(\"l_linestatus\", "
        "sizeof(Char<1>));";
   f << "auto qtyAttr = result->addAttribute(\"sum_qty\", "
        "sizeof(Numeric<12, "
        "2>));";
   f << "auto base_priceAttr = result->addAttribute(\"sum_base_price\", "
        "sizeof(Numeric<12, 2>));";
   f << "auto disc_priceAttr = result->addAttribute(\"sum_disc_price\", "
        "sizeof(Numeric<12, 2>));";
   f << "auto chargeAttr = result->addAttribute(\"sum_charge\", "
        "sizeof(Numeric<12, 2>));";
   f << "auto count_orderAttr = result->addAttribute(\"count_order\", "
        "sizeof(int64_t));";
   f << "groupOp.forallGroups([&](runtime::Stack<decltype(groupOp)::group_t>"
        "& "
        "/*auto&*/ entries) {";
   f << "auto n = entries.size();";
   f << "auto block = result->createBlock(n);";
   f << "auto ret = reinterpret_cast<Char<1>*>(block.data(retAttr));";
   f << "auto status = reinterpret_cast<Char<1>*>(block.data(statusAttr));";
   f << "auto qty = reinterpret_cast<Numeric<12, 2>*>(block.data(qtyAttr));";
   f << "auto base_price = reinterpret_cast<Numeric<12, "
        "2>*>(block.data(base_priceAttr));";
   f << "auto disc_price = reinterpret_cast<Numeric<12, "
        "4>*>(block.data(disc_priceAttr));";
   f << "auto charge = reinterpret_cast<Numeric<12, "
        "6>*>(block.data(chargeAttr));";
   f << "auto count_order = "
        "reinterpret_cast<int64_t*>(block.data(count_orderAttr));";
   f << "for (auto block : entries)";
   f << "for (auto& entry : block) {";
   f << "*ret++ = get<0>(entry.k);";
   f << "*status++ = get<1>(entry.k);";
   f << "*qty++ = get<0>(entry.v);";
   f << "*base_price++ = get<1>(entry.v);";
   f << "*disc_price++ = get<2>(entry.v);";
   f << "*charge++ = get<3>(entry.v);";
   f << " *count_order++ = get<4>(entry.v);";
   f << "}";
   f << "block.addedElements(n);";
   f << "});";
   f << "\n\n// join threads\n";
   f << "leaveQuery(nrThreads);";
   f << "return move(resources.query);";
   f << "}\n";
   f.close();

   // format code
   std::string format_command = std::string("astyle -q ") + cppName;
   system(format_command.c_str());

   return filename;
}

const std::string CodeGenerator::generateTyperQ6() {
   // generate code
   const std::string filename = "src/hybrid/typer_q6";
   const std::string cppName = filename + ".cpp";
   std::ofstream f(cppName);
   f << "#include \"hybrid/minimal_api.hpp\"\n";
   f << "using namespace runtime;\n";
   f << "using namespace std;\n";
   f << "Relation compiled_typer_q6(Database& db, size_t nrThreads, size_t "
        "firstTuple, int64_t twRevenue) {\n";
   f << "Relation result;\n";
   f << "result.insert(\"revenue\", make_unique<algebra::Numeric>(12,4));\n";
   f << "auto c1 = types::Date::castString(\"1994-01-01\");\n";
   f << "auto c2 = types::Date::castString(\"1995-01-01\");\n";
   f << "auto c3 = types::Numeric<12, 2>::castString(\"0.05\");\n";
   f << "auto c4 = types::Numeric<12, 2>::castString(\"0.07\");\n";
   f << "auto c5 = types::Integer(24);\n";
   f << "types::Numeric<12, 4> revenue = 0;\n";
   f << "auto& rel = db[\"lineitem\"];\n";
   f << "auto l_shipdate_col = rel[\"l_shipdate\"].data<types::Date>();\n";
   f << "auto l_quantity_col = rel[\"l_quantity\"].data<types::Numeric<12, "
        "2>>();\n";
   f << "auto l_extendedprice_col = "
        "rel[\"l_extendedprice\"].data<types::Numeric<12,2 >>();\n";
   f << "auto l_discount_col = rel[\"l_discount\"].data<types::Numeric<12,"
        "2>>();\n";
   f << "revenue = tbb::parallel_reduce(\n";
   f << "tbb::blocked_range<size_t>(firstTuple, rel.nrTuples),\n";
   f << "types::Numeric<12, 4>(0),\n";
   f << "[&](const tbb::blocked_range<size_t>& r,\n";
   f << "const types::Numeric<12, 4>& s) {\n";
   f << "auto revenue = s;\n";
   f << "for (size_t i = r.begin(), end = r.end(); i != end; ++i) {\n";
   f << "auto& l_shipdate = l_shipdate_col[i];\n";
   f << "auto& l_quantity = l_quantity_col[i];\n";
   f << "auto& l_extendedprice = l_extendedprice_col[i];\n";
   f << "auto& l_discount = l_discount_col[i];\n";
   f << "if ((l_shipdate >= c1) & (l_shipdate < c2) & (l_quantity < c5) &\n";
   f << "(l_discount >= c3) & (l_discount <= c4)) {\n";
   f << "revenue += l_extendedprice * l_discount;\n";
   f << "}\n";
   f << "}\n";
   f << "return revenue;\n";
   f << "},\n";
   f << "[](const types::Numeric<12, 4>& x, const types::Numeric<12, 4>& y) "
        "{\n";
   f << "return x + y;\n";
   f << "});\n";
   f << "revenue += types::Numeric<12, 4>(twRevenue);";
   f << "auto& rev = "
        "result[\"revenue\"].typedAccessForChange<types::Numeric<12,4>>();"
        "\n";
   f << "rev.reset(1);\n";
   f << "rev.push_back(revenue);\n";
   f << "result.nrTuples = 1;\n";
   f << "return result;\n";
   f << "}\n";
   f.close();

   // format code
   std::string format_command = std::string("astyle -q ") + cppName;
   system(format_command.c_str());

   return filename;
}

const std::string CodeGenerator::generateTyperQ18() {
   // generate code
   const std::string filename = "src/hybrid/typer_q18";
   const std::string cppName = filename + ".cpp";
   std::ofstream f(cppName);
   f << "#include \"hybrid/minimal_api.hpp\"\n";
   f << "using namespace runtime;\n";
   f << "using namespace std;\n";
   f << "using namespace types;\n";
   f << "std::unique_ptr<runtime::Query> compiled_typer_q18(Database& db, "
        "size_t nrThreads, size_t "
        "firstTuple, std::unordered_map<std::thread::id, "
        "runtime::PartitionedDeque<1024>>&"
        "twThreadData) {\n ";
   f << "using namespace types;\n"
        "using namespace std;\n"

        "using hash = runtime::CRC32Hash;\n"

        "auto resources = initQuery(nrThreads);\n"

        "auto& li = db[\"lineitem\"];\n"

        "auto l_orderkey = li[\"l_orderkey\"].data<types::Integer>();\n"
        "auto l_quantity = li[\"l_quantity\"].data<types::Numeric<12, 2>>();\n"

        "tbb::enumerable_thread_specific<"
        "Hashmapx<types::Integer, types::Numeric<12, 2>, hash, false>>"
        "groups;"

        "const auto zero = types::Numeric<12, 2>::castString(\"0.00\");"

        "auto groupOp = make_GroupBy<types::Integer, types::Numeric<12, 2>, "
        "hash>("
        "[](auto& acc, auto&& value) { acc += value; }, zero, nrThreads);"

        "\n// scan lineitem and group by l_orderkey\n"
        "tbb::parallel_for(tbb::blocked_range<size_t>(0, li.nrTuples, "
        "morselSize),"
        "[&](const tbb::blocked_range<size_t>& r) {"
        "auto locals = groupOp.preAggLocals();"
        "for (size_t i = r.begin(), end = r.end(); i != end;"
        "++i) {"
        "auto& group = locals.getGroup(l_orderkey[i]);"
        "group += l_quantity[i];"
        "\n// locals.consume(l_orderkey[i], l_quantity[i]);\n"
        "}"
        "});"
        "Hashset<types::Integer, hash> ht1;"
        "tbb::enumerable_thread_specific<runtime::Stack<decltype(ht1)::Entry>>"
        "entries1;"
        "const auto threeHundret = types::Numeric<12, 2>::castString(\"300\");"
        "std::atomic<size_t> nrGroups;"
        "nrGroups = 0;"
        "groupOp.forallGroups([&](auto& groups) {"
        "auto& entries = entries1.local();"
        "size_t groupsFound = 0;"
        "for (auto block : groups)"
        "for (auto& group : block)"
        "if (group.v > threeHundret) {"
        "entries.emplace_back(ht1.hash(group.k), group.k);"
        "groupsFound++;"
        "}"
        "\n// TODO: reconsider this way of counting groups\n"
        "nrGroups.fetch_add(groupsFound);"
        "});"
        "ht1.setSize(nrGroups);"
        "parallel_insert(entries1, ht1);"
        "\n// build customer hashtable\n"
        "auto& cu = db[\"customer\"];"
        "auto c_custkey = cu[\"c_custkey\"].data<types::Integer>();"
        "auto c_name = cu[\"c_name\"].data<types::Char<25>>();"
        "Hashmapx<types::Integer, types::Char<25>, hash> ht2;"
        "tbb::enumerable_thread_specific<runtime::Stack<decltype(ht2)::Entry>>"
        "entries2;"

        "PARALLEL_SCAN(cu.nrTuples, entries2, {"
        "entries.emplace_back(ht2.hash(c_custkey[i]), c_custkey[i], c_name[i]);"
        "});"
        "ht2.setSize(cu.nrTuples);"
        "parallel_insert(entries2, ht2);"

        "\n// build last hashtable\n"
        "Hashmapx<types::Integer,"
        "std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,"
        "types::Char<25>>,"
        "hash>"
        "ht3;"
        "tbb::enumerable_thread_specific<runtime::Stack<decltype(ht3)::Entry>>"
        "entries3;"
        "auto& ord = db[\"orders\"];"
        "auto o_orderkey = ord[\"o_orderkey\"].data<types::Integer>();"
        "auto o_custkey = ord[\"o_custkey\"].data<types::Integer>();"
        "auto o_orderdate = ord[\"o_orderdate\"].data<types::Date>();"
        "auto o_totalprice = ord[\"o_totalprice\"].data<types::Numeric<12, "
        "2>>();"

        "\n// scan orders\n"
        "auto found = PARALLEL_SELECT(ord.nrTuples, entries3, {"
        "types::Char<25>* name;"
        "\n// check if it matches the order criteria and look up the customer "
        "name\n"
        "if (ht1.contains(o_orderkey[i]) && (name = "
        "ht2.findOne(o_custkey[i]))) {"
        "entries.emplace_back("
        "ht3.hash(o_orderkey[i]), o_orderkey[i],"
        "make_tuple(o_custkey[i], o_orderdate[i], o_totalprice[i], *name));"
        "found++;"
        "}"
        "});"
        "ht3.setSize(found);"
        "parallel_insert(entries3, ht3);"
        "auto finalGroupOp = make_GroupBy<"
        "std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,"
        "types::Char<25>, types::Integer>,"
        "types::Numeric<12, 2>, hash>("
        "[](auto& acc, auto&& value) { acc += value; }, zero, nrThreads);"

        "\n// scan lineitem and group by l_orderkey\n"
        "tbb::parallel_for("
        "tbb::blocked_range<size_t>(firstTuple, li.nrTuples, morselSize),"
        "[&](const tbb::blocked_range<size_t>& r) {"
        "auto locals = finalGroupOp.preAggLocals();"
        "for (size_t i = r.begin(), end = r.end(); i != end; ++i) {"
        "std::tuple<types::Integer, types::Date, types::Numeric<12, 2>,"
        "types::Char<25>>* v;"
        "if ((v = ht3.findOne(l_orderkey[i]))) {"
        "auto& group ="
        "locals.getGroup(tuple_cat(*v, make_tuple(l_orderkey[i])));"
        "group += l_quantity[i];"
        "}"
        "}"
        "});"

        "\n\n// merge tw's partial thread-local hash tables with typer's "
        "partial "
        "thread-local hash tables\n"
        "tbb::parallel_for_each(twThreadData.begin(), twThreadData.end(), "
        "[&](auto& threadPartitions) {auto locals = "
        "finalGroupOp.preAggLocals();tbb::parallel_for_each(threadPartitions."
        "second."
        "getPartitions().begin(),threadPartitions.second.getPartitions().end(),"
        " [&](auto& partition) {for (auto chunk = partition.first; chunk; "
        "chunk = chunk->next) {auto elementSize = "
        "threadPartitions.second.entrySize;auto nPart = partition.size(chunk,"
        "elementSize);"

        "auto data = chunk->template data<hybrid::Q18TectorTuple>();"
        "for (unsigned i = 0; i < nPart; ++i) {"
        "hybrid::Q18TectorTuple t = data[i];"
        "hybrid::Q18TyperKey key = std::make_tuple("
        "types::Integer(t.c_custkey), types::Date(t.o_orderdate),"
        "types::Numeric<12, 2>(t.o_totalprice),"
        "types::Char<25>::build(t.c_name),"
        "types::Integer(t.o_orderkey));"
        "hybrid::Q18TyperValue value = types::Numeric<12, 2>(t.sum);"
        "locals.consume(key, value);}}});});"

        "auto& result = resources.query->result;"
        "auto namAttr = result->addAttribute(\"c_name\", "
        "sizeof(types::Char<25>));"
        "auto ckyAttr = result->addAttribute(\"c_custkey\", "
        "sizeof(types::Integer));"
        "auto okyAttr = result->addAttribute(\"o_orderkey\", "
        "sizeof(types::Integer));"
        "auto datAttr = result->addAttribute(\"o_orderdate\", "
        "sizeof(types::Date));"
        "auto totAttr ="
        "result->addAttribute(\"o_totalprice\", sizeof(types::Numeric<12, 2>));"
        "auto sumAttr = result->addAttribute(\"sum\", "
        "sizeof(types::Numeric<12, 2>));"

        "finalGroupOp.forallGroups([&](auto& groups) {"
        "\n// write aggregates to result\n"
        "auto n = groups.size();"
        "auto block = result->createBlock(n);"
        "auto name = reinterpret_cast<types::Char<25>*>(block.data(namAttr));"
        "auto cky = reinterpret_cast<types::Integer*>(block.data(ckyAttr));"
        "auto oky = reinterpret_cast<types::Integer*>(block.data(okyAttr));"
        "auto dat = reinterpret_cast<types::Date*>(block.data(datAttr));"
        "auto tot = reinterpret_cast<types::Numeric<12, "
        "2>*>(block.data(totAttr));"
        "auto sum = reinterpret_cast<types::Numeric<12, "
        "2>*>(block.data(sumAttr));"
        "for (auto block : groups)"
        "for (auto& group : block) {"
        "auto& k = group.k;"
        "*name++ = std::get<3>(k);"
        "*cky++ = get<0>(k);"
        "*oky++ = get<4>(k);"
        "*dat++ = get<1>(k);"
        "*tot++ = get<2>(k);"
        "*sum++ = group.v;"
        "}"
        "block.addedElements(n);"
        "});"
        "leaveQuery(nrThreads);"
        "return move(resources.query);";
   f << "}\n";
   f.close();

   // format code
   std::string format_command = std::string("astyle -q ") + cppName;
   system(format_command.c_str());

   return filename;
}
} // namespace hybrid