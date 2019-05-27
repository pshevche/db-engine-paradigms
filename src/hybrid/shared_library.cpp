#include "hybrid/shared_library.hpp"
#include <dlfcn.h>
#include <iostream>

namespace hybrid {

SharedLibrary* SharedLibrary::load(const std::string& libPath) {
   auto soLib = dlopen(libPath.c_str(), RTLD_NOW);
   auto error = dlerror();
   if (error) {
      std::cout << error << std::endl;
      return nullptr;
   }
   return new SharedLibrary(soLib);
}

SharedLibrary::SharedLibrary(void* soLib) : mSharedLib(soLib) {}

void* SharedLibrary::getSymbol(const std::string& symbolName) {
   return dlsym(this->mSharedLib, symbolName.c_str());
}

SharedLibrary::~SharedLibrary() { dlclose(this->mSharedLib); }
} // namespace hybrid