#pragma once
#include <memory>

namespace hybrid {
// Container for a shared library.
class SharedLibrary {
 public:
   // load a shared library
   static std::unique_ptr<SharedLibrary> load(const char* libPath);
   SharedLibrary(void* soLib);
   // get a symbol from the library
   void* getSymbol(const std::string& symbolName);
   // get a function from the library
   template <typename Function>
   Function getFunction(const std::string& funcName) {
      return reinterpret_cast<Function>(getSymbol(funcName));
   }
   ~SharedLibrary();

 private:
   void* mSharedLib;
};

} // namespace hybrid