#pragma once
#include <cstdint>
#include <mutex>
#include <algorithm>
#include <vector>
#include <cassert>
#include <cstring>
#include <type_traits>
#include <tuple>
#include <array>
#include <sys/mman.h>

#include "xbox.h"
#include "mutex.h"

// Mock o1heap
typedef struct O1HeapInstance O1HeapInstance;

#ifdef __cplusplus
extern "C" {
#endif
    O1HeapInstance* o1heapInit(void* const base, const size_t size);
    void* o1heapAllocate(O1HeapInstance* const handle, const size_t amount);
    void o1heapFree(O1HeapInstance* const handle, void* const pointer);
#ifdef __cplusplus
}
#endif

#define O1HEAP_ALIGNMENT 16

// Mock PPC structures
union PPCRegister {
    uint64_t u64;
    struct {
        uint32_t u32_pad; // Pad for alignment/overlap on Little Endian
        uint32_t u32;
    };
    double f64;
    float f32;
};

struct PPCContext {
    PPCRegister r0, r1, r2, r3, r4, r5, r6, r7, r8, r9, r10, r11, r12, r13, r14, r15, r16, r17, r18, r19, r20, r21, r22, r23, r24, r25, r26, r27, r28, r29, r30, r31;
    PPCRegister f0, f1, f2, f3, f4, f5, f6, f7, f8, f9, f10, f11, f12, f13, f14, f15, f16, f17, f18, f19, f20, f21, f22, f23, f24, f25, f26, f27, f28, f29, f30, f31;
    uint32_t fpscr;
    uint32_t cr;
    uint32_t xer;
    uint32_t lr;
    uint32_t ctr;
};

#define PPC_FUNC(name) void name(PPCContext& ctx, uint8_t* base)

// PPCFunc typedef required by memory.h
using PPCFunc = void(PPCContext&, uint8_t*);

// PPC_LOOKUP_FUNC required by memory.h
// Mock implementation that just returns nullptr or specific mock?
// memory.h: PPC_LOOKUP_FUNC(base, guest) = host;
// So it must return an lvalue (reference or pointer to pointer).
// We can cast memory at `base + guest` to `PPCFunc*`.
// Since we have a large allocated memory (4GB), this is safe if `guest` is valid.
#define PPC_LOOKUP_FUNC(base, guest) (*(PPCFunc**)((uint8_t*)base + guest))

// Fix NULL warnings
#undef NULL
#define NULL 0
