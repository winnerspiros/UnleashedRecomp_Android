#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <cstdlib>
#include <vector>
#include <memory>
#include <iostream>
#include <cstdio>

#include <stdafx.h>

class KernelObject;

// Define kernel memory pool BEFORE g_memory to ensure initialization order
static std::vector<uint8_t> kernel_memory_pool(0x10000);

#include "../kernel/memory.h"

Memory g_memory;

#include "../kernel/heap.h"

Heap g_userHeap;

Memory::Memory() {
    uint8_t* pool_ptr = kernel_memory_pool.data();
    base = pool_ptr - 0x80000000;
}

extern "C" void* MmGetHostAddress(uint32_t ptr) {
    return g_memory.Translate(ptr);
}

static size_t heap_offset = 0;

void* Heap::AllocPhysical(size_t size, size_t alignment) {
    size_t align_mask = alignment - 1;
    if (alignment == 0) align_mask = 0xF;

    heap_offset = (heap_offset + align_mask) & ~align_mask;

    if (heap_offset + size > kernel_memory_pool.size()) {
        return nullptr;
    }

    void* ptr = kernel_memory_pool.data() + heap_offset;
    heap_offset += size;
    return ptr;
}

void* Heap::Alloc(size_t size) {
    return AllocPhysical(size, 16);
}

void Heap::Free(void* ptr) {}
size_t Heap::Size(void* ptr) { return 0; }
void Heap::Init() {}

#include "../kernel/xdm.cpp"

struct TestObject : KernelObject {
    int value = 42;
};

TEST_CASE("IsKernelObject Check") {
    CHECK(IsKernelObject((uint32_t)0x80000000));
    CHECK(IsKernelObject((uint32_t)0x80000001));
    CHECK(IsKernelObject((uint32_t)0xFFFFFFFF));
    CHECK_FALSE(IsKernelObject((uint32_t)0x00000000));
}

TEST_CASE("KernelObject Creation and Validation") {
    heap_offset = 0;

    auto* obj = CreateKernelObject<TestObject>();
    REQUIRE(obj != nullptr);
    CHECK(obj->value == 42);

    uint32_t handle = GetKernelHandle(obj);
    CHECK((handle & 0x80000000) != 0);

    auto* retrieved = GetKernelObject<TestObject>(handle);
    CHECK(retrieved == obj);

    DestroyKernelObject(handle);
}

TEST_CASE("CreateKernelObject Arguments") {
    struct ArgsObject : KernelObject {
        int a, b;
        ArgsObject(int x, int y) : a(x), b(y) {}
    };

    auto* obj = CreateKernelObject<ArgsObject>(10, 20);
    REQUIRE(obj != nullptr);
    CHECK(obj->a == 10);
    CHECK(obj->b == 20);
}
