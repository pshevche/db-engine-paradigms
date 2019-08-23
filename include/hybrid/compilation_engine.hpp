#pragma once
#include <string>

namespace hybrid {

// Singleton to handle all compilation related issues
class CompilationEngine {
 public:
   // return an instance of the engine
   static CompilationEngine& instance();
   // compile .pch file to speed up future compilations
   const std::string precompileAPIHeader();
   // compile .cpp query code into either .o or .ll files
   const std::string compileQueryCPP(const std::string& filename, bool toLLVM);
   // link .o or .ll file into a shared library
   const std::string linkQueryLib(const std::string& filename, bool fromLLVM);

 private:
   CompilationEngine();
};

} // namespace hybrid