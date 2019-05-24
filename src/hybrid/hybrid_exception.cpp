#include "hybrid/hybrid_exception.hpp"

namespace hybrid {

HybridException::HybridException(const std::string& msg) : mMessage(msg) {}

const char* HybridException::what() { return mMessage.c_str(); }

} // namespace hybrid
