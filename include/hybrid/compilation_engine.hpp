#pragma once
#include <iostream>

namespace hybrid {

// handles all compilation related issues
class CompilationEngine {
 public:
   static CompilationEngine& instance();
   // compile hyper code for TPC-H Q6
   const char* precompileHeader();
   const char* compileQ6CPP();
   const char* compileQ6LL();

 private:
   CompilationEngine();
};

} // namespace hybrid