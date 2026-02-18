#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <cstdint>
#include <vector>
#include <cstring>
#include <iostream>
#include <memory>

// Define necessary macros for XenonUtils/ppc_context.h
// Note: These macros are typically generated or defined in project-specific headers that might be missing in standalone test builds.
// We define them here to match the values used in guest_stack_var_test.cpp and expected by ppc_context.h.
#ifndef PPC_MEMORY_SIZE
#define PPC_MEMORY_SIZE 0x100000000ull
#endif

#define PPC_IMAGE_BASE 0x82000000
#define PPC_IMAGE_SIZE 0x1000000
#define PPC_CODE_BASE 0x82000000
#define PPC_CONFIG_H_INCLUDED

// Include PPCContext definition from tools via include path
#include <ppc_context.h>

// Define PPC_LOOKUP_FUNC required by Memory definition if not defined by ppc_context.h
#ifndef PPC_LOOKUP_FUNC
#define PPC_LOOKUP_FUNC(x, y) *(PPCFunc**)(x + PPC_IMAGE_BASE + PPC_IMAGE_SIZE + (uint64_t(uint32_t(y) - PPC_CODE_BASE) * 2))
#endif

// Include ArgTranslator
#include "../kernel/function.h"

// Implementation of Memory methods for test
// We use a global vector to manage memory lifetime to avoid leaks reported in review.
static std::vector<uint8_t> g_memory_storage;

Memory g_memory;

Memory::Memory() {
    // Resize storage if empty (to support multiple test runs if needed, though constructor runs once for global)
    if (g_memory_storage.empty()) {
        g_memory_storage.resize(1024 * 1024); // 1MB buffer
    }
    base = g_memory_storage.data();
    std::memset(base, 0, g_memory_storage.size());
}

// Mock MmGetHostAddress used by xbox.h/memory.h
extern "C" void* MmGetHostAddress(uint32_t ptr) {
    return g_memory.Translate(ptr);
}

TEST_CASE("ArgTranslator::GetRegister") {
    PPCContext ctx{};
    ctx.r3.u64 = 10;
    ctx.r4.u64 = 20;
    ctx.r5.u64 = 30;
    ctx.r6.u64 = 40;
    ctx.r7.u64 = 50;
    ctx.r8.u64 = 60;
    ctx.r9.u64 = 70;
    ctx.r10.u64 = 80;

    CHECK(ArgTranslator::GetRegister<0>(ctx) == 10);
    CHECK(ArgTranslator::GetRegister<1>(ctx) == 20);
    CHECK(ArgTranslator::GetRegister<2>(ctx) == 30);
    CHECK(ArgTranslator::GetRegister<3>(ctx) == 40);
    CHECK(ArgTranslator::GetRegister<4>(ctx) == 50);
    CHECK(ArgTranslator::GetRegister<5>(ctx) == 60);
    CHECK(ArgTranslator::GetRegister<6>(ctx) == 70);
    CHECK(ArgTranslator::GetRegister<7>(ctx) == 80);
}

TEST_CASE("ArgTranslator::GetFloatRegister") {
    PPCContext ctx{};
    ctx.f1.f64 = 1.0;
    ctx.f2.f64 = 2.0;
    ctx.f3.f64 = 3.0;
    ctx.f4.f64 = 4.0;
    ctx.f5.f64 = 5.0;
    ctx.f6.f64 = 6.0;
    ctx.f7.f64 = 7.0;
    ctx.f8.f64 = 8.0;
    ctx.f9.f64 = 9.0;
    ctx.f10.f64 = 10.0;
    ctx.f11.f64 = 11.0;
    ctx.f12.f64 = 12.0;
    ctx.f13.f64 = 13.0;

    CHECK(ArgTranslator::GetFloatRegister<0>(ctx) == 1.0);
    CHECK(ArgTranslator::GetFloatRegister<1>(ctx) == 2.0);
    CHECK(ArgTranslator::GetFloatRegister<2>(ctx) == 3.0);
    CHECK(ArgTranslator::GetFloatRegister<3>(ctx) == 4.0);
    CHECK(ArgTranslator::GetFloatRegister<4>(ctx) == 5.0);
    CHECK(ArgTranslator::GetFloatRegister<5>(ctx) == 6.0);
    CHECK(ArgTranslator::GetFloatRegister<6>(ctx) == 7.0);
    CHECK(ArgTranslator::GetFloatRegister<7>(ctx) == 8.0);
    CHECK(ArgTranslator::GetFloatRegister<8>(ctx) == 9.0);
    CHECK(ArgTranslator::GetFloatRegister<9>(ctx) == 10.0);
    CHECK(ArgTranslator::GetFloatRegister<10>(ctx) == 11.0);
    CHECK(ArgTranslator::GetFloatRegister<11>(ctx) == 12.0);
    CHECK(ArgTranslator::GetFloatRegister<12>(ctx) == 13.0);
}

TEST_CASE("ArgTranslator::GetIntegerArgumentValue (Stack)") {
    PPCContext ctx{};
    // Use g_memory.base as stack
    uint8_t* stack_base = g_memory.base;
    uint32_t stack_ptr = 0x1000; // Offset in base
    ctx.r1.u32 = stack_ptr;

    // Arg 8 (index 0 on stack)
    // Offset = 0x54 + (0 * 8) = 0x54
    uint32_t offset8 = 0x54;
    // Arg 9 (index 1 on stack)
    // Offset = 0x54 + (1 * 8) = 0x5C
    uint32_t offset9 = 0x5C;

    // Write values to stack.
    // Memory uses Big Endian storage (be<T>).
    // GetIntegerArgumentValue interprets it as Big Endian.
    // So writing directly via reinterpret_cast<be<uint64_t>*> ensures correct endianness conversion.

    // Arg 8: 64-bit value
    uint64_t val8 = 0x1234567890ABCDEF;
    *reinterpret_cast<be<uint64_t>*>(stack_base + stack_ptr + offset8) = val8;

    // Arg 9: 32-bit value
    uint32_t val9 = 0xCAFEBABE;
    // For 32-bit values, function.h reads at offset + 4.
    *reinterpret_cast<be<uint32_t>*>(stack_base + stack_ptr + offset9 + 4) = val9;

    // Verify
    CHECK(ArgTranslator::GetIntegerArgumentValue<8, uint64_t>(ctx, stack_base) == val8);
    CHECK(ArgTranslator::GetIntegerArgumentValue<9, uint32_t>(ctx, stack_base) == val9);
}

TEST_CASE("ArgTranslator::SetIntegerArgumentValue") {
    PPCContext ctx{};
    ArgTranslator::SetIntegerArgumentValue(ctx, nullptr, 0, 100);
    CHECK(ctx.r3.u64 == 100);

    ArgTranslator::SetIntegerArgumentValue(ctx, nullptr, 7, 200);
    CHECK(ctx.r10.u64 == 200);
}
