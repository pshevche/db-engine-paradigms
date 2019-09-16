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

 private:
   CodeGenerator();
};

} // namespace hybrid