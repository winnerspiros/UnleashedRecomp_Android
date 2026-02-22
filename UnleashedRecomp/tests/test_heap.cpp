#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <vector>
#include <sys/mman.h>
#include <cassert>
#include <iostream>

// Include mock stdafx first to define types needed by memory.h
#include "mock/stdafx.h"

// Define PPC_MEMORY_SIZE large enough for Heap::Init
#define PPC_MEMORY_SIZE 0x100000000ULL

// Include memory.h to get Memory struct definition
#include "../kernel/memory.h"

// Define O1HeapInstance for our mock
struct O1HeapInstance {
    size_t offset;
    size_t capacity;
};

// Mock tracking
struct MockState {
    int initCalls = 0;
    int allocCalls = 0;
    int freeCalls = 0;
} g_mockState;

extern "C" {
    O1HeapInstance* o1heapInit(void* const base, const size_t size) {
        g_mockState.initCalls++;
        O1HeapInstance* instance = (O1HeapInstance*)base;
        instance->offset = sizeof(O1HeapInstance);
        // Align offset to 16 bytes
        instance->offset = (instance->offset + 15) & ~15;
        instance->capacity = size;
        return instance;
    }

    void* o1heapAllocate(O1HeapInstance* const handle, const size_t amount) {
        g_mockState.allocCalls++;

        size_t headerSize = 16; // 2 * sizeof(size_t)
        size_t totalNeeded = headerSize + amount;
        size_t alignedTotal = (totalNeeded + 15) & ~15;

        if (handle->offset + alignedTotal > handle->capacity) return nullptr;

        uint8_t* start = (uint8_t*)handle + handle->offset;
        handle->offset += alignedTotal;

        void* ptr = start + headerSize;

        // Write header for Heap::Size()
        // Heap::Size returns *((size_t*)ptr - 2) - O1HEAP_ALIGNMENT;
        // So we store amount + O1HEAP_ALIGNMENT
        size_t* h = (size_t*)ptr;
        h[-2] = amount + 16;

        return ptr;
    }

    void o1heapFree(O1HeapInstance* const handle, void* const pointer) {
        g_mockState.freeCalls++;
    }
}

Memory g_memory;

extern "C" void* MmGetHostAddress(uint32_t ptr) {
    return g_memory.Translate(ptr);
}

Memory::Memory() {
    // Allocate 4GB virtual space
    // MAP_NORESERVE prevents consuming physical memory
    base = (uint8_t*)mmap(nullptr, PPC_MEMORY_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    assert(base != MAP_FAILED);
}

// Include heap.cpp to test
#include "../kernel/heap.cpp"

// Define global heap instance (declared extern in heap.h)
Heap g_userHeap;

TEST_CASE("Heap::Init") {
    g_mockState = {};
    g_userHeap.Init();

    CHECK(g_mockState.initCalls == 2);
    CHECK(g_userHeap.heap != nullptr);
    CHECK(g_userHeap.physicalHeap != nullptr);
    CHECK(g_userHeap.heap->capacity == RESERVED_BEGIN - 0x20000);
    CHECK(g_userHeap.physicalHeap->capacity == 0x100000000 - RESERVED_END);
}

TEST_CASE("Heap::Alloc") {
    g_mockState = {};
    g_userHeap.Init();

    size_t size = 100;
    void* ptr = g_userHeap.Alloc(size);

    CHECK(g_mockState.allocCalls == 1);
    CHECK(ptr != nullptr);
    CHECK(g_userHeap.Size(ptr) == size);

    // Check range (Virtual Heap)
    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t base = (uintptr_t)g_memory.base;
    CHECK(addr >= base + 0x20000);
    CHECK(addr < base + RESERVED_BEGIN);
}

TEST_CASE("Heap::AllocPhysical") {
    g_mockState = {};
    g_userHeap.Init();

    size_t size = 100;
    size_t alignment = 128;
    void* ptr = g_userHeap.AllocPhysical(size, alignment);

    CHECK(ptr != nullptr);
    CHECK(((uintptr_t)ptr % alignment) == 0);

    // Check range (Physical Heap)
    uintptr_t addr = (uintptr_t)ptr;
    uintptr_t base = (uintptr_t)g_memory.base;
    CHECK(addr >= base + RESERVED_END);
}

TEST_CASE("Heap::Free") {
    g_mockState = {};
    g_userHeap.Init();

    void* ptr = g_userHeap.Alloc(100);
    g_userHeap.Free(ptr);
    CHECK(g_mockState.freeCalls == 1);

    g_mockState.freeCalls = 0;
    void* physPtr = g_userHeap.AllocPhysical(100, 16);
    g_userHeap.Free(physPtr);
    CHECK(g_mockState.freeCalls == 1);
}
