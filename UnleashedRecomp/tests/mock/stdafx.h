#pragma once

#include <cstdint>
#include <cassert>
#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <mutex>
#include <iostream>
#include <cstring>

// Include mock xbox.h for be<T>
#include "xbox.h"

#define NOMINMAX
#define PPC_MEMORY_SIZE 0xFFFFFFFF // 4GB

#ifndef _WIN32
// Minimal Windows types for compatibility if needed by xdm.h
typedef uint32_t DWORD;
typedef int32_t LONG;
typedef void* HANDLE;
#endif

// For memory.h
using PPCFunc = void(void*);
#define PPC_LOOKUP_FUNC(base, guest) (*(PPCFunc**)((uint8_t*)base + guest))

// For heap.h
typedef struct O1HeapInstance O1HeapInstance;
#define O1HEAP_ALIGNMENT 16

// For xdm.h
struct XDISPATCHER_HEADER {
    struct {
        uint32_t Flink;
        be<uint32_t> Blink;
    } WaitListHead;
};
