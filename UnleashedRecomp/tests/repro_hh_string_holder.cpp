#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include <utility> // for std::move

// Define dependencies for standalone test
#include "SWA.inl"

// Define global memory instance required by SWA.inl / hhSharedString.inl
Memory g_memory;

// Mock __HH_ALLOC and __HH_FREE implementation
static size_t g_alloc_size = 0;
static int g_alloc_count = 0;

extern "C" void* __HH_ALLOC(size_t size) {
    g_alloc_size = size;
    g_alloc_count++;
    return std::malloc(size);
}

extern "C" void __HH_FREE(void* ptr) {
    std::free(ptr);
}

// Include the target header
// Ensure we pick up the OVERRIDDEN header which is in UnleashedRecomp/api_overrides
#include <Hedgehog/Base/Type/detail/hhStringHolder.h>
#include <Hedgehog/Base/Type/hhSharedString.h>

int main() {
    std::cout << "Starting SStringHolder::Make tests..." << std::endl;

    // Test 1: Normal allocation
    {
        const char* str = "hello";
        size_t len = std::strlen(str);
        // Reset stats
        g_alloc_size = 0;
        g_alloc_count = 0;

        auto* holder = Hedgehog::Base::SStringHolder::Make(str, len);

        if (holder == nullptr) {
            std::cerr << "Test 1 Failed: Allocation returned nullptr" << std::endl;
            return 1;
        }

        if (g_alloc_count != 1) {
            std::cerr << "Test 1 Failed: Allocation count mismatch" << std::endl;
            return 1;
        }

        size_t expected_size = sizeof(Hedgehog::Base::SStringHolder::RefCount) + 1 + len;
        if (g_alloc_size != expected_size) {
            std::cerr << "Test 1 Failed: Allocation size mismatch. Expected " << expected_size << ", got " << g_alloc_size << std::endl;
            return 1;
        }

        if (std::strcmp(holder->aStr, str) != 0) {
            std::cerr << "Test 1 Failed: String content mismatch" << std::endl;
            return 1;
        }

        holder->Release();
        std::cout << "Test 1 (Normal Allocation) Passed." << std::endl;
    }

    // Test 2: Huge allocation (simulated overflow)
    {
        size_t len = (size_t)(uint32_t)-1;

        g_alloc_size = 0;
        g_alloc_count = 0;

        auto* holder = Hedgehog::Base::SStringHolder::Make("fake", len);

        if (holder != nullptr) {
            std::cerr << "Test 2 Failed: Huge allocation should return nullptr" << std::endl;
            return 1;
        }

        if (g_alloc_count != 0) {
            std::cerr << "Test 2 Failed: Huge allocation should not call __HH_ALLOC" << std::endl;
            return 1;
        }

        std::cout << "Test 2 (Overflow Protection) Passed." << std::endl;
    }

    // Test 3: CSharedString Crash Checks
    {
        // Default constructor
        {
            Hedgehog::Base::CSharedString s;
            if (s.get() != nullptr) {
                 std::cerr << "Test 3 Failed: Default constructor not null" << std::endl;
                 return 1;
            }
            // Destructor runs here. If it crashes, test fails.
        }
        std::cout << "Test 3 (Default Constructor) Passed." << std::endl;

        // Copy constructor with empty
        {
            Hedgehog::Base::CSharedString s1;
            Hedgehog::Base::CSharedString s2(s1);
            if (s2.get() != nullptr) {
                std::cerr << "Test 3 Failed: Copy empty failed" << std::endl;
                return 1;
            }
        }
        std::cout << "Test 3 (Copy Empty) Passed." << std::endl;

        // Move constructor with empty
        {
            Hedgehog::Base::CSharedString s1;
            Hedgehog::Base::CSharedString s2(std::move(s1));
            if (s2.get() != nullptr) {
                std::cerr << "Test 3 Failed: Move empty failed" << std::endl;
                return 1;
            }
        }
        std::cout << "Test 3 (Move Empty) Passed." << std::endl;

        // Normal string usage
        {
            Hedgehog::Base::CSharedString s("test");
            if (s.empty()) {
                std::cerr << "Test 3 Failed: Normal string empty" << std::endl;
                return 1;
            }
            if (std::strcmp(s.c_str(), "test") != 0) {
                std::cerr << "Test 3 Failed: Normal string content mismatch" << std::endl;
                return 1;
            }
        }
        std::cout << "Test 3 (Normal String) Passed." << std::endl;
    }

    std::cout << "All tests passed!" << std::endl;
    return 0;
}
