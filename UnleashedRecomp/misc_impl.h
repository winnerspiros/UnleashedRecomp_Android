#pragma once

#include <chrono>
#include <cstdint>

// Internal implementation for testing purposes
inline uint32_t GetTickCountImpl_Internal()
{
    return uint32_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());
}
