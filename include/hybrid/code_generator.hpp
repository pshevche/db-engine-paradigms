#pragma once
#include <string>

namespace hybrid {

// Singleton that generates Typer code for TPC-H queries
class CodeGenerator {
 public:
   static CodeGenerator& instance();
   // generates cpp files with Typer code for execution of TPC-H queries
   const std::string generateTyperQ1();
   const std::string generateTyperQ6();

 private:
   CodeGenerator();
};

} // namespace hybrid