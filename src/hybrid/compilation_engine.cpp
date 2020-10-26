#include "hybrid/compilation_engine.hpp"
#include "hybrid/hybrid_exception.hpp"
#include <boost/filesystem/operations.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

namespace hybrid {

CompilationEngine::CompilationEngine() {}

CompilationEngine& CompilationEngine::instance() {
   static CompilationEngine ce;
   return ce;
}

const std::string CompilationEngine::precompileAPIHeader() {
   // init
   const std::string path_to_minimal_api_header =
       "include/hybrid/minimal_api.hpp";
   const std::string path_to_precompiled_header =
       path_to_minimal_api_header + ".pch";

   bool rebuild_precompiled_header = false;

   // check if the API header has to be updated
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

   // recompile the API header if needed
   if (rebuild_precompiled_header) {
      std::cout
          << "Precompiled Header not found! Building Precompiled Header now..."
          << std::endl;
      std::stringstream precompile_header;

      precompile_header << "clang++ -march=native -std=c++14 -g -O3 -fpic "
                        << path_to_minimal_api_header << " -I "
                        << "include/ -o " << path_to_precompiled_header
                        << std::endl;
      auto ret = system(precompile_header.str().c_str());
      if (ret) {
         throw HybridException("Compilation of precompiled header failed!");
      } else {
         std::cout << "Compilation of precompiled header successful!"
                   << std::endl;
      }
   }

   // return path to the header
   return path_to_precompiled_header;
}

const std::string
CompilationEngine::compileQueryCPP(const std::string& filename, bool toLLVM) {
   // init
   const std::string path_to_minimal_api_header =
       "include/hybrid/minimal_api.hpp";
   const std::string path_to_cpp = filename + ".cpp";
   std::string path_to_target;

   std::stringstream compile_cmd;
   if (!toLLVM) {
      // compile cpp into object files
      path_to_target = filename + ".o";
      compile_cmd << "clang++ -march=native -std=c++14 -g -O3 -include "
                  << path_to_minimal_api_header << " -I include/ -c -fpic";
      compile_cmd << path_to_cpp << " -o " << path_to_target;
   } else {
      // compile cpp into llvm
      path_to_target = filename + ".ll";
      compile_cmd << "clang++ -march=native -std=c++14 -g -O3 -include "
                  << path_to_minimal_api_header
                  << " -I include/ -emit-llvm -S -c -fpic ";
      compile_cmd << path_to_cpp << " -o " << path_to_target;
   }

   int ret = system(compile_cmd.str().c_str());
   if (ret) { throw HybridException("Could not compile query code!"); }

   // return path to cpp
   return filename;
}

const std::string CompilationEngine::linkQueryLib(const std::string& filename,
                                                  bool fromLLVM) {
   std::stringstream linking_command;
   const std::string path_to_lib = filename + ".so";
   if (!fromLLVM) {
      // link library from object files
      const std::string& path_to_obj = filename + ".o";
      linking_command << "clang++ -shared " << path_to_obj << " -o "
                      << path_to_lib;
   } else {
      // link library from llvm
      const std::string& path_to_ll = filename + ".ll";
      linking_command << "clang++ -O3 -fpic -shared " << path_to_ll << " -o "
                      << path_to_lib;
   }

   int ret = system(linking_command.str().c_str());
   if (ret) { throw HybridException("Couldn't link the shared library!"); }

   // return path to the shared library
   return filename;
}

} // namespace hybrid
