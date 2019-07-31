#include "hybrid/hybrid_operators.hpp"

namespace hybrid {

using runtime::barrier;

template <typename T, typename HT>
void INTERPRET_SEPARATE insertAllEntries(T& allocations, HT& ht,
                                         size_t ht_entry_size) {
   for (auto& block : allocations) {
      auto start =
          reinterpret_cast<runtime::Hashmap::EntryHeader*>(block.first);
      ht.insertAll_tagged(start, block.second, ht_entry_size);
   }
}

size_t HybridHashJoin::next() {
   using runtime::Hashmap;
   // --- build
   if (!consumed) {
      size_t found = 0;
      // --- build phase 1: materialize ht entries
      for (auto n = left->next(); n != EndOfStream; n = left->next()) {
         found += n;
         // build hashes
         buildHash.evaluate(n);
         // scatter hash, keys and values into ht entries
         auto alloc =
             runtime::this_worker->allocator.allocate(n * ht_entry_size);
         if (!alloc) throw std::runtime_error("malloc failed");
         allocations.push_back(std::make_pair(alloc, n));
         scatterStart = reinterpret_cast<decltype(scatterStart)>(alloc);
         buildScatter.evaluate(n);
      }

      // --- build phase 2: insert ht entries
      shared.found.fetch_add(found);
      barrier([&]() {
         auto globalFound = shared.found.load();
         if (globalFound) shared.ht.setSize(globalFound);
      });
      auto globalFound = shared.found.load();
      if (globalFound == 0) {
         consumed = true;
         return EndOfStream;
      }
      insertAllEntries(allocations, shared.ht, ht_entry_size);
      consumed = true;
      barrier(); // wait for all threads to finish build phase
   }
   // --- lookup
   while (true) {
      if (cont.nextProbe >= cont.numProbes) {
         cont.numProbes = right->next();
         cont.nextProbe = 0;
         if (cont.numProbes == EndOfStream) return EndOfStream;
         probeHash.evaluate(cont.numProbes);
      }
      // create join pair vectors with matching hashes (Entry*, pos), where
      // Entry* is for the build side, pos a selection index to the right side
      auto n = (this->*join)();
      // check key equality and remove non equal keys from join result
      n = keyEquality.evaluate(n);
      if (n == 0) continue;
      // materialize build side
      buildGather.evaluate(n);
      return n;
   }
}
} // namespace hybrid