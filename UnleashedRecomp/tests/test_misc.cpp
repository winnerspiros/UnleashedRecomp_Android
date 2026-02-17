#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "misc_impl.h"
#include <thread>
#include <chrono>

TEST_CASE("GetTickCountImpl monotonic check") {
    // Check initial call
    uint32_t t1 = GetTickCountImpl_Internal();

    // Sleep for a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(20));

    // Check subsequent call
    uint32_t t2 = GetTickCountImpl_Internal();

    // Ensure monotonicity (allowing for wrap around theoretically, but practically unlikely in test)
    // However, since we are testing "monotonically increasing milliseconds", strictly speaking it should be t2 >= t1.
    // If it wraps, t2 < t1. But 32-bit ms wraps in ~49 days.
    CHECK(t2 >= t1);

    // Ensure time has advanced at least the sleep duration (minus some small tolerance if needed, but steady_clock should be fine)
    // t2 - t1 should be roughly 20.
    // We check it's at least close.
    // Note: sleep_for guarantees *at least* the duration.
    CHECK(t2 - t1 >= 19);
}
