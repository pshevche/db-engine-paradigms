#pragma once
#include <string>

namespace hybrid {

// Singleton that generates Hyper code for TPC-H queries
class CodeGenerator {
 public:
    // get instance of the generator
   static CodeGenerator& instance();
   // generates cpp files with Typer code for execution of TPC-H queries
   const std::string generateTyperQ1();
   const std::string generateTyperQ6();
   const std::string generateTyperQ18();
   const std::string generateTyperQ3();

    const std::string generatePureTyperQ1();
    const std::string generatePureTyperQ6();
    const std::string generatePureTyperQ18();
//    const std::string generatePureTyperQ3();
//
    const std::string generateHybridTyperQ1();
    const std::string generateHybridTyperQ6();
    const std::string generateHybridTyperQ18();
    const std::string generateHybridTyperQ3();

 private:
   CodeGenerator();
};

} // namespace hybrid