#pragma once
#include <exception>
#include <string>

namespace hybrid {

class HybridException : std::exception {
 public:
   HybridException(const std::string& msg);
   const char* what();

 private:
   const std::string& mMessage;
};

}