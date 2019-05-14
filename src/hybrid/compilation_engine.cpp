#include "hybrid/compilation_engine.hpp"
#include <fstream>
#include <iostream>

namespace hybrid {

const char* CompilationEngine::compileHelloWorld() {
   const std::string& cppName = "hello_world.cpp";
   const char* libName = "./libhello.so";
   const std::string& cmd =
       "g++ -shared " + cppName + " -o " + libName + " -fPIC";
   // generate code
   std::ofstream f(cppName);
   f << "#include<iostream>\nvoid hello_world() { std::cout << \"Hello "
        "World!\" << std::endl; }\n";
   f.close();
   // compile library
   int ret = system(cmd.c_str());
   if (ret) { return nullptr; }
   return libName;
}
const std::string& CompilationEngine::compileQ6() {
   // TODO: replace with actual Q6 code
   return this->compileHelloWorld();
}

} // namespace hybrid