#pragma once
#include <iostream>

namespace hybrid {

// handles all compilation related issues
class CompilationEngine {
 public:
   // compile hyper code for TPC-H Q6
   const char* compileQ6();
};

} // namespace hybrid