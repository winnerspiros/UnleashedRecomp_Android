#include <cstdint>
#include <atomic>
#include <cassert>

// Mock xbox.h stuff
#include "../build_overrides/tools/XenonRecomp/XenonUtils/xbox.h"

// Mock GuestToHostFunction
template<typename Ret, typename... Args>
Ret GuestToHostFunction(be<uint32_t> func, Args... args) { return Ret(); }

// Mock MmGetHostAddress since it is extern "C" in xbox.h
extern "C" void* MmGetHostAddress(uint32_t ptr) { return (void*)(uintptr_t)ptr; }

#include "../UnleashedRecomp/api/boost/smart_ptr/shared_ptr.h"

int main() {
    // Ensure we can instantiate the template
    boost::shared_ptr<int> p;
    return 0;
}
