#pragma once
#include <cstdint>

typedef void PPCFunc;

#define PPC_MEMORY_SIZE 0x20000000

inline PPCFunc*& GetPPCFuncRef(void* base, uint32_t guest)
{
    static PPCFunc* f = nullptr;
    return f;
}

#define PPC_LOOKUP_FUNC(base, guest) GetPPCFuncRef(base, guest)
