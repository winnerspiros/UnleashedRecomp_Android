#pragma once
#include <cstdint>
#include <cstring>
#include <atomic>

// Mock be<T> (Big Endian)
template <typename T>
struct be {
    T value;
    be() = default;
    be(T v) : value(v) {}
    operator T() const { return value; }
    be& operator=(T v) { value = v; return *this; }
    bool operator==(T v) const { return value == v; }
    bool operator!=(T v) const { return value != v; }
};

// Mock xpointer
template <typename T>
struct xpointer {
    T* ptr;
    xpointer() : ptr(nullptr) {}
    xpointer(T* p) : ptr(p) {}
    xpointer(std::nullptr_t) : ptr(nullptr) {}

    operator T*() const { return ptr; }
    T* operator->() const { return ptr; }
    T& operator[](size_t idx) const { return ptr[idx]; }
};

// Mock Memory result proxy to handle void* to T* conversion implicitly
struct MapVirtualProxy {
    void* ptr;
    template <typename T> operator T*() const { return static_cast<T*>(ptr); }
    // operator uint32_t() const { return (uint32_t)(size_t)ptr; }
    // We prefer pointer conversion for xpointer<T>.
};

// Mock g_memory if referenced in headers
struct Memory {
    MapVirtualProxy MapVirtual(void* ptr) { return {ptr}; }
    void* Translate(uint32_t offset) { return (void*)(size_t)offset; }
};
extern Memory g_memory;

// Mock __HH_ALLOC and __HH_FREE if they are macros/inline functions
extern "C" void* __HH_ALLOC(size_t size);
extern "C" void __HH_FREE(void* ptr);
