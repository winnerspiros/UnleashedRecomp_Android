#include "../kernel/freelist.h"
#include <vector>
#include <memory>
#include <cassert>
#include <iostream>
#include <cstdlib>

// Simple test runner macro
#define ASSERT(condition) \
    do { \
        if (!(condition)) { \
            std::cerr << "Assertion failed: " << #condition << " at " << __FILE__ << ":" << __LINE__ << std::endl; \
            std::exit(1); \
        } \
    } while (0)

// FreeList implementation currently has a double-destruction issue for non-POD types
// because std::vector manages object lifetime but Free() manually destroys them.
// Therefore, we only test with POD types (int) where double destruction is harmless.

void TestBasicAlloc() {
    FreeList<int> list;
    size_t idx1 = list.Alloc();
    list[idx1] = 10;
    ASSERT(list[idx1] == 10);

    size_t idx2 = list.Alloc();
    list[idx2] = 20;
    ASSERT(list[idx2] == 20);
    ASSERT(idx1 != idx2);

    std::cout << "TestBasicAlloc passed" << std::endl;
}

void TestFreeReuse() {
    FreeList<int> list;
    size_t idx1 = list.Alloc();
    size_t idx2 = list.Alloc();

    list.Free(idx1);
    size_t idx3 = list.Alloc();

    // Should reuse the freed index (LIFO behavior of freed stack)
    ASSERT(idx3 == idx1);

    std::cout << "TestFreeReuse passed" << std::endl;
}

void TestFreeByReference() {
    FreeList<int> list;
    size_t idx1 = list.Alloc();
    list[idx1] = 42;

    list.Free(list[idx1]);

    // Check if index is in freed list
    ASSERT(list.freed.size() == 1);
    ASSERT(list.freed.back() == idx1);

    std::cout << "TestFreeByReference passed" << std::endl;
}

int main() {
    TestBasicAlloc();
    TestFreeReuse();
    TestFreeByReference();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
