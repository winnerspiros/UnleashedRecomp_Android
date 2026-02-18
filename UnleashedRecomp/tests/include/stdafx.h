#pragma once

#include <algorithm>
#include <mutex>
#include <filesystem>
#include <fstream>
#include <vector>
#include <string>
#include <cassert>
#include <cstdint>
#include <map>
#include <iostream>
#include <string_view>
#include <system_error>
#include <tuple>
#include <utility>
#include <type_traits>
#include <array>
#include <memory>
#include <thread>
#include <functional> // Added

#include <SDL.h>
#include <o1heap.h>
#include <toml++/toml.hpp>

// Forward declare PPCContext for function.h
struct PPCContext;

// Minimal Windows API definitions
#ifndef _WIN32
#include <sys/mman.h>

// Macros and types defined in xdm.h (which is included by file_system.cpp -> guest_thread.h -> xdm.h)
// We avoid defining them here to prevent redefinition warnings/errors.
// xdm.h defines: DWORD, LONG, LONGLONG, WORD, BYTE, BOOL, CHAR, FLOAT, HANDLE, HMODULE, LPVOID, LPCVOID, LPCSTR, LPSTR, LONG_PTR
// And constants: FALSE, TRUE, GENERIC_READ, ... INVALID_HANDLE_VALUE etc.
// And structs: LARGE_INTEGER, FILETIME, WIN32_FIND_DATAA.

// However, we might need MAX_PATH if xdm.h doesn't define it?
// xdm.h uses MAX_PATH in WIN32_FIND_DATAA definition?
// Let's check xdm.h content.
// It defines WIN32_FIND_DATAA with `char cFileName[260];` (hardcoded).
// It does NOT define MAX_PATH.
// So we should define MAX_PATH.

#define MAX_PATH 260

#endif
