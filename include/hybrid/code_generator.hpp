#pragma once
#include <string>

namespace hybrid {

// Singleton that generates Typer code for TPC-H queries
class CodeGenerator {
 public:
   static CodeGenerator& instance();
   // generates a cpp file with Typer code for execution of TPC-H Q6
   const std::string generateTyperQ6();

 private:
   CodeGenerator();
};

} // namespace hybrid