#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <cstdint>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <algorithm>

// Mock PPCContext definition
#define PPC_CONFIG_H_INCLUDED
#include "ppc_context.h" // XenonUtils version via include path

// Redefine PPC_LOOKUP_FUNC to be simple for testing
#undef PPC_LOOKUP_FUNC
#define PPC_LOOKUP_FUNC(base, guest) (*(PPCFunc**)((uint8_t*)base + guest))

// Include kernel/memory.h
#include "UnleashedRecomp/kernel/memory.h"

// Global memory instance
Memory g_memory;

// Mock MmGetHostAddress
extern "C" void* MmGetHostAddress(uint32_t ptr) {
    return g_memory.Translate(ptr);
}

// Implement Memory constructor for test environment
Memory::Memory() {
    static std::vector<uint8_t> buffer(0x10000 + 4096);
    base = buffer.data();
}

// Mock Allocator
size_t g_allocOffset = 0x2000;
void* MockAlloc(size_t size) {
    size = (size + 15) & ~15;
    if (g_allocOffset + size > 0x10000) return nullptr;
    void* ptr = g_memory.base + g_allocOffset;
    g_allocOffset += size;
    return ptr;
}
void MockFree(void*) {}

#define __HH_ALLOC MockAlloc
#define __HH_FREE MockFree

// Include required headers for CSharedString etc
#include "Hedgehog/Base/Type/hhSharedString.h"

// Include target header
#include "Hedgehog/Universe/Engine/hhUpdateUnit.h"
#include "Hedgehog/Universe/Engine/hhMessageActor.h"

thread_local PPCContext g_ppcContextInstance;

class TestUpdateUnit : public Hedgehog::Universe::CUpdateUnit
{
public:
    TestUpdateUnit() : CUpdateUnit() {}
};

class MockUpdateUnit : public Hedgehog::Universe::CUpdateUnit
{
public:
    bool updateParallelCalled = false;

    void UpdateParallel(const Hedgehog::Universe::SUpdateInfo& in_rUpdateInfo) override {
        updateParallelCalled = true;
    }
};

struct MockSUpdateInfo
{
    be<float> DeltaTime;
    be<uint32_t> Frame;
    void* CategoryIgnored;
};

TEST_CASE("CUpdateUnit::ExecuteParallelJob calls UpdateParallel") {
    // Initialize PPCContext pointer
    SetPPCContext(g_ppcContextInstance);
    // Initialize stack pointer to end of memory
    g_ppcContextInstance.r1.u32 = 0x10000 - 256;

    MockUpdateUnit unit;

    MockSUpdateInfo mockInfo;
    mockInfo.DeltaTime = 0.16f;
    mockInfo.Frame = 1;
    mockInfo.CategoryIgnored = nullptr; // Valid pointer or null

    unit.ExecuteParallelJob(reinterpret_cast<const Hedgehog::Universe::SUpdateInfo&>(mockInfo));

    CHECK(unit.updateParallelCalled);
}

class TestMessageActor : public Hedgehog::Universe::CMessageActor
{
public:
    void ExecuteParallelJob(const Hedgehog::Universe::SUpdateInfo&) override {}
};

TEST_CASE("CMessageActor instantiation") {
     // Initialize PPCContext pointer
    SetPPCContext(g_ppcContextInstance);
    g_ppcContextInstance.r1.u32 = 0x10000 - 256;

    TestMessageActor actor;
    CHECK(true);
}

namespace Hedgehog::Base
{
    CObject::CObject() {}
    CObject::CObject(const swa_null_ctor&) {}
    void* CObject::operator new(const size_t in_Size) { return MockAlloc(in_Size); }
    void* CObject::operator new(const size_t in_Size, const size_t in_Align) { return MockAlloc(in_Size); }
    void CObject::operator delete(void* in_pMem) { MockFree(in_pMem); }
    void* CObject::operator new(const size_t in_Size, void* in_pObj) { return in_pObj; }
    void* CObject::operator new(const size_t in_Size, const size_t in_Align, void* in_pObj) { return in_pObj; }
    void CObject::operator delete(void* in_pMem, void* in_pObj) {}
}
