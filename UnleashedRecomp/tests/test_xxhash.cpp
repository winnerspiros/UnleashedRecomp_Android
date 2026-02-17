#undef NDEBUG
#include <cassert>
#include <iostream>

// Include necessary headers
#include <xxhash.h>
#include <ankerl/unordered_dense.h>

// Include the header to be tested
#include "../xxHashMap.h"

int main() {
    // Test 1: Verify xxHash operator
    xxHash hasher;
    XXH64_hash_t hash_val = 123456789;
    assert(hasher(hash_val) == hash_val);
    std::cout << "xxHash operator test passed." << std::endl;

    // Test 2: Verify xxHashMap basic functionality
    xxHashMap<int> map;
    XXH64_hash_t key1 = 100;
    XXH64_hash_t key2 = 200;

    map[key1] = 1;
    map[key2] = 2;

    assert(map.size() == 2);
    assert(map[key1] == 1);
    assert(map[key2] == 2);

    assert(map.find(key1) != map.end());
    assert(map.find(key2) != map.end());
    assert(map.find(300) == map.end());

    std::cout << "xxHashMap functionality test passed." << std::endl;

    return 0;
}
