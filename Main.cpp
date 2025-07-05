#include <iostream>
#include "LRUCache.h"

int main() {
  LRUCache<int, std::string> cache(3);
  cache.put(1, "10");
  cache.put(2, "two");
  if (auto v = cache.get(1))
    std::cout << *v << "\n";
  return 0;
}
