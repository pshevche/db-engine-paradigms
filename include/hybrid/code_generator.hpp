#pragma once
#include <string>

namespace hybrid {

// Singleton that generates Hyper code for TPC-H queries
class CodeGenerator {
 public:
    // get instance of the generator
   static CodeGenerator& instance();
   // generates cpp files with pure Hyper code
   const std::string generatePureTyperQ1();
   const std::string generatePureTyperQ6();
   const std::string generatePureTyperQ18();
   // generates cpp files with Hyper code for hybrid queries
   const std::string generateHybridTyperQ1();
   const std::string generateHybridTyperQ6();
   const std::string generateHybridTyperQ18();

 private:
   CodeGenerator();
};

} // namespace hybrid