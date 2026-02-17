#include <iostream>
#include <vector>
#include <functional>
#include <string>
#include <stdexcept>
#include <thread>
#include <chrono>

#include "misc_funcs.h"
// ByteSwap is needed to interpret the results.
// It is available via xbox.h (included by misc_funcs.h)

// Minimal test framework
int g_TestsPassed = 0;
int g_TestsFailed = 0;

// Simple registry for tests
std::vector<std::function<void()>> g_TestRunners;

#define TEST_CASE(name) \
    void name(); \
    struct name##_Register { \
        name##_Register() { \
            g_TestRunners.push_back([](){ \
                try { \
                    name(); \
                    std::cout << "[PASS] " << #name << std::endl; \
                    g_TestsPassed++; \
                } catch (const std::exception& e) { \
                    std::cout << "[FAIL] " << #name << ": " << e.what() << std::endl; \
                    g_TestsFailed++; \
                } catch (...) { \
                    std::cout << "[FAIL] " << #name << ": Unknown exception" << std::endl; \
                    g_TestsFailed++; \
                } \
            }); \
        } \
    } name##_Instance; \
    void name()

#define CHECK(condition) \
    if (!(condition)) { \
        throw std::runtime_error("CHECK failed: " #condition); \
    }

TEST_CASE(TestQueryPerformanceCounterImpl)
{
    LARGE_INTEGER li1 = {0};
    LARGE_INTEGER li2 = {0};
    uint32_t ret;

    ret = QueryPerformanceCounterImpl(&li1);
    CHECK(ret == TRUE);

    // Sleep a bit to ensure time passes
    std::this_thread::sleep_for(std::chrono::milliseconds(1));

    ret = QueryPerformanceCounterImpl(&li2);
    CHECK(ret == TRUE);

    // misc_funcs.cpp stores the value using ByteSwap (converting host time to guest BE time).
    // To compare, we must convert back to host endianness or compare both as BE if consistent.
    // Since ByteSwap simply swaps, calling it again restores original value (if symmetrical).
    // Let's assume we want to check monotonicity of the underlying time.

    uint64_t val1 = ByteSwap((uint64_t)li1.QuadPart);
    uint64_t val2 = ByteSwap((uint64_t)li2.QuadPart);

    CHECK(val2 > val1);
}

TEST_CASE(TestQueryPerformanceFrequencyImpl)
{
    LARGE_INTEGER li;
    uint32_t ret = QueryPerformanceFrequencyImpl(&li);
    CHECK(ret == TRUE);

    uint64_t freq = ByteSwap((uint64_t)li.QuadPart);
    CHECK(freq > 0);
}

int main()
{
    std::cout << "Running tests..." << std::endl;
    for (auto& runner : g_TestRunners) {
        runner();
    }

    std::cout << "Tests passed: " << g_TestsPassed << std::endl;
    std::cout << "Tests failed: " << g_TestsFailed << std::endl;

    return g_TestsFailed > 0 ? 1 : 0;
}
