#pragma once

// Standard headers
#include <vector>
#include <string>
#include <array>
#include <tuple>
#include <unordered_set>
#include <algorithm>
#include <cstdint>
#include <memory>
#include <ranges>
#include <cstring>
#include <cassert>
#include <filesystem>
#include <atomic>
#include <thread>

// Project headers mocks/includes
#include <xbox.h>

#define XXH_INLINE_ALL
#include <xxhash.h>

#include <ankerl/unordered_dense.h>
#include <SDL.h>
#include "config.h"
#include "o1heap.h"
#include "ppc/ppc_recomp_shared.h"
#include "ppc/ppc_context.h"

// Minimal framework macros if needed
#ifndef STR
#define STR(x) #x
#endif

// Framework functions used by xam.cpp or headers
inline size_t StringHash(const std::string_view& str)
{
    return XXH3_64bits(str.data(), str.size());
}
