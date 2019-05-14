#include "hybrid/shared_library.hpp"
#include <dlfcn.h>
#include <iostream>

namespace hybrid {

std::unique_ptr<SharedLibrary> SharedLibrary::load(const char* libPath) {
   auto soLib = dlopen(libPath, RTLD_NOW);
   auto error = dlerror();
   if (error) { return nullptr; }
   return std::make_unique<SharedLibrary>(soLib);
}

SharedLibrary::SharedLibrary(void* soLib) : mSharedLib(soLib) {}

void* SharedLibrary::getSymbol(const std::string& symbolName) {
   return dlsym(this->mSharedLib, symbolName.c_str());
}

SharedLibrary::~SharedLibrary() { dlclose(this->mSharedLib); }
} // namespace hybrid