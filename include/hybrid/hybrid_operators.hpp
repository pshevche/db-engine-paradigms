#pragma once
#include "vectorwise/Operators.hpp"

namespace hybrid {

const size_t IgnoreValue = std::numeric_limits<size_t>::max();

using namespace vectorwise;
class HybridHashJoin : public Hashjoin {
 public:
   HybridHashJoin(Shared& sm) : Hashjoin(sm) {}
   virtual size_t next() override;
};
} // namespace hybrid
