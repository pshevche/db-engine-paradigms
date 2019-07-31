#pragma once
#include "vectorwise/Operators.hpp"

namespace hybrid {
using namespace vectorwise;
class HybridHashJoin : public Hashjoin {
 public:
   HybridHashJoin(Shared& sm) : Hashjoin(sm) {}
   virtual size_t next() override;
};
} // namespace hybrid
