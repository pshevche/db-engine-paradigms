#pragma once
#include <exception>
#include <string>

namespace hybrid {

// Custom exception for issues caused by hybrid approach
class HybridException : std::exception {
 public:
   HybridException(const std::string& msg);
   const char* what();

 private:
   const std::string& mMessage;
};

}