#pragma once
#include <cstdint>
#include <cstddef>
#include <map>

struct Memory
{
    uint8_t* base;
    std::map<size_t, void*> mapped_addresses;

    void* Translate(size_t offset) const noexcept
    {
        if (mapped_addresses.count(offset))
            return mapped_addresses.at(offset);
        return base + offset;
    }
};
extern Memory g_memory;
