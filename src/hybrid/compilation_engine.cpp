#include "hybrid/compilation_engine.hpp"
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <sstream>

namespace hybrid {

const char* CompilationEngine::precompileHeader() {
   std::string path_to_minimal_api_header = "include/hybrid/minimal_api.hpp";
   std::string path_to_precompiled_header = path_to_minimal_api_header + ".gch";

   bool rebuild_precompiled_header = false;

   if (!boost::filesystem::exists(path_to_precompiled_header)) {
      rebuild_precompiled_header = true;
   } else {
      std::time_t last_access_pch =
          boost::filesystem::last_write_time(path_to_precompiled_header);
      std::time_t last_access_header =
          boost::filesystem::last_write_time(path_to_minimal_api_header);
      /* pre-compiled header outdated? */
      if (last_access_header > last_access_pch) {
         std::cout << "Pre-compiled header '" << path_to_precompiled_header
                   << "' is outdated!" << std::endl;
         rebuild_precompiled_header = true;
      }
   }

   if (rebuild_precompiled_header) {
      std::cout
          << "Precompiled Header not found! Building Precompiled Header now..."
          << std::endl;
      std::stringstream precompile_header;

      precompile_header << "g++ -g -O3 -fpic " << path_to_minimal_api_header
                        << " -I "
                        << "include/ -o " << path_to_precompiled_header
                        << std::endl;
      auto ret = system(precompile_header.str().c_str());
      if (ret != 0) {
         std::cout << "Compilation of precompiled header failed!" << std::endl;
         return nullptr;
      } else {
         std::cout << "Compilation of precompiled header successful!"
                   << std::endl;
      }
   }
   return path_to_precompiled_header.c_str();
}

const char* CompilationEngine::compileQ6() {
   // generate code
   const std::string& cppName = "src/hybrid/typer_q6.cpp";
   std::ofstream f(cppName);
   f << "#include \"hybrid/minimal_api.hpp\"\n";
   f << "using namespace runtime;\n";
   f << "using namespace std;\n";
   f << "Relation compiled_typer_q6(Database& db, size_t nrThreads, size_t "
        "firstTuple) {\n";
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
   f << "auto& rev = "
        "result[\"revenue\"].typedAccessForChange<types::Numeric<12,4>>();"
        "\n";
   f << "rev.reset(1);\n";
   f << "rev.push_back(revenue);\n";
   f << "result.nrTuples = 1;\n";
   f << "return result;\n";
   f << "}\n";
   f.close();

   // compile shared library
   std::string path_to_minimal_api_header = "include/hybrid/minimal_api.hpp";
   const char* libName = "src/hybrid/libtyperq6.so";
   std::stringstream compile_cmd;
   compile_cmd << "g++ -Wno-parentheses-equality -g -O3 -include "
               << path_to_minimal_api_header << " -I include/ -c -fpic ";
   compile_cmd << cppName << " -o " << cppName << ".o";
   int ret = system(compile_cmd.str().c_str());
   if (ret) {
      std::cerr << "Could not compile query code!" << std::endl;
      return nullptr;
   }

   // link library
   std::stringstream linking_command;
   linking_command << "g++ -shared " << cppName << ".o -o " << libName
                   << std::endl;
   ret = system(linking_command.str().c_str());
   if (ret != 0) {
      std::cerr << "Linking failed!" << std::endl;
      return nullptr;
   }
   return libName;
}

} // namespace hybrid