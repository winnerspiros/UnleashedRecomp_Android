#pragma once
#include <cstdint>
#include <cstddef>
#include <mutex>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cassert>
#include <tuple>
#include <type_traits>

// Include mock ppc definitions
#include <ppc/ppc_recomp_shared.h>

// Mock O1Heap
struct O1HeapInstance;
#define O1HEAP_ALIGNMENT 16

extern "C" {
    O1HeapInstance* o1heapInit(void* const base, const size_t size);
    void* o1heapAllocate(O1HeapInstance* const handle, const size_t amount);
    void o1heapFree(O1HeapInstance* const handle, void* const pointer);
}

// Mock Mutex (using std::mutex)
using Mutex = std::mutex;

// Define NOMINMAX to avoid conflicts with std::max
#define NOMINMAX

// Mock minimal Xbox definitions if needed
#include <xbox.h>
