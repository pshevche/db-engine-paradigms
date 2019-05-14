#pragma once
#include <iostream>

namespace hybrid {

// handles all compilation related issues
class CompilationEngine {
 public:
   // just for testing
   const char* compileHelloWorld();
   // compile hyper code for TPC-H Q6
   const std::string& compileQ6();
};

} // namespace hybrid