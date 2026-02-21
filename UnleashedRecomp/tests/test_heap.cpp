#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <mutex>
#include <vector>
#include <cstring>
#include <algorithm>
#include <iostream>
#include <sys/mman.h>
#include <exception>

// Mock dependencies
#include "stdafx.h"

// Define PPCFunc and macro
using PPCFunc = void(void*);
#ifndef PPC_LOOKUP_FUNC
#define PPC_LOOKUP_FUNC(base, guest) (*(PPCFunc**)((uint8_t*)base + guest))
#endif

// Define PPC_MEMORY_SIZE for memory.h (4GB for mapping high addresses)
#define PPC_MEMORY_SIZE 0x100000000ULL

#include "kernel/memory.h"

// Implement Memory using mmap
Memory g_memory;

Memory::Memory() {
    // 4GB + extra
    size_t size = 0x100000000ULL + 0x10000;
    // Use MAP_NORESERVE to avoid committing swap
    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (ptr == MAP_FAILED) {
        std::cerr << "mmap failed" << std::endl;
        std::terminate();
    }
    base = (uint8_t*)ptr;
}

// Memory::Translate is implicitly defined in header, consistent with mmap base.

extern "C" void* MmGetHostAddress(uint32_t ptr) {
    return g_memory.Translate(ptr);
}

// Define g_userHeap
#include "kernel/heap.h"
Heap g_userHeap;

// Mock O1Heap
struct O1HeapInstance {
    uint8_t* nextPtr;
    uint8_t* endPtr;
};

struct AllocationInfo {
    void* ptr;
    size_t size;
    O1HeapInstance* heap;
};
std::vector<AllocationInfo> g_allocations;
std::mutex g_allocMutex;

extern "C" {
    O1HeapInstance* o1heapInit(void* const base, const size_t size) {
        auto* instance = reinterpret_cast<O1HeapInstance*>(base);
        uintptr_t start = reinterpret_cast<uintptr_t>(base) + sizeof(O1HeapInstance);
        start = (start + 15) & ~15;
        instance->nextPtr = reinterpret_cast<uint8_t*>(start);
        instance->endPtr = reinterpret_cast<uint8_t*>(base) + size;
        return instance;
    }

    void* o1heapAllocate(O1HeapInstance* const handle, const size_t amount) {
        std::lock_guard<std::mutex> lock(g_allocMutex);

        size_t alignment = O1HEAP_ALIGNMENT; // 16
        size_t headerSize = 2 * sizeof(size_t);

        uintptr_t current = reinterpret_cast<uintptr_t>(handle->nextPtr);
        uintptr_t ptrAddr = (current + headerSize + (alignment - 1)) & ~(alignment - 1);

        if (ptrAddr + amount > reinterpret_cast<uintptr_t>(handle->endPtr)) {
            return nullptr;
        }

        void* ptr = reinterpret_cast<void*>(ptrAddr);

        // Write header for Heap::Size
        size_t* header = static_cast<size_t*>(ptr) - 2;
        *header = amount + alignment;

        handle->nextPtr = reinterpret_cast<uint8_t*>(ptr) + amount;

        g_allocations.push_back({ptr, amount, handle});
        return ptr;
    }

    void o1heapFree(O1HeapInstance* const handle, void* const pointer) {
        std::lock_guard<std::mutex> lock(g_allocMutex);
        auto it = std::find_if(g_allocations.begin(), g_allocations.end(),
            [pointer](const AllocationInfo& a) { return a.ptr == pointer; });

        if (it != g_allocations.end()) {
            if (it->heap != handle) {
                 // Log error
            }
            g_allocations.erase(it);
        }
    }
}

// Include source
#include "../kernel/heap.cpp"

// Tests
TEST_CASE("Heap::Init") {
    g_userHeap.Init();
    CHECK(g_userHeap.heap != nullptr);
    CHECK(g_userHeap.physicalHeap != nullptr);
    CHECK(g_userHeap.heap != g_userHeap.physicalHeap);
}

TEST_CASE("Heap::Alloc and Free") {
    g_userHeap.Init();
    g_allocations.clear();

    void* ptr = g_userHeap.Alloc(128);
    CHECK(ptr != nullptr);

    CHECK(g_allocations.size() == 1);
    CHECK(g_allocations[0].heap == g_userHeap.heap);
    CHECK(g_allocations[0].size == 128);

    g_userHeap.Free(ptr);
    CHECK(g_allocations.empty());
}

TEST_CASE("Heap::AllocPhysical") {
    g_userHeap.Init();
    g_allocations.clear();

    size_t size = 100;
    size_t alignment = 256;
    void* ptr = g_userHeap.AllocPhysical(size, alignment);

    CHECK(ptr != nullptr);
    CHECK((reinterpret_cast<uintptr_t>(ptr) & (alignment - 1)) == 0);

    CHECK(g_allocations.size() == 1);
    CHECK(g_allocations[0].heap == g_userHeap.physicalHeap);
    CHECK(g_allocations[0].size >= size + alignment);

    g_userHeap.Free(ptr);
    CHECK(g_allocations.empty());
}

TEST_CASE("Heap::Size") {
    g_userHeap.Init();
    g_allocations.clear(); // Clear old allocs

    void* ptr = g_userHeap.Alloc(128);
    CHECK(g_userHeap.Size(ptr) == 128);
    g_userHeap.Free(ptr);
}
