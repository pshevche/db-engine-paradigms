#pragma once
#include <string>

namespace hybrid {

// Singleton that generates Typer code for TPC-H queries
class CodeGenerator {
 public:
   static CodeGenerator& instance();
   // generates cpp files with Typer code for execution of TPC-H queries
   const std::string generatePureTyperQ1();
   const std::string generatePureTyperQ6();
   const std::string generatePureTyperQ18();
   // generates cpp files with Typer code for execution of TPC-H queries
   const std::string generateHybridTyperQ1();
   const std::string generateHybridTyperQ6();
   const std::string generateHybridTyperQ18();

 private:
   CodeGenerator();
};

} // namespace hybrid