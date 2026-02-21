#pragma once
#include <cstdint>

template<typename T>
struct be {
    T val;
    operator T() const { return val; }
    be& operator=(T v) { val = v; return *this; }
};
