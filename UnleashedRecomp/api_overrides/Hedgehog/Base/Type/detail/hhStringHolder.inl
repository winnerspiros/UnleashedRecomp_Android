#include <cstring>
#include <cstdint>
#include <atomic>

namespace Hedgehog::Base
{
    inline SStringHolder* SStringHolder::GetHolder(const char* in_pStr)
    {
        return (SStringHolder*)((size_t)in_pStr - sizeof(RefCount));
    }

    inline SStringHolder* SStringHolder::Make(const char* in_pStr, size_t length)
    {
        constexpr uint32_t overhead = sizeof(RefCount) + 1;

        if (length > (uint32_t)-1 - overhead)
            return nullptr;

        auto pHolder = (SStringHolder*)__HH_ALLOC(overhead + (uint32_t)length);

        if (pHolder)
        {
            pHolder->RefCount = 1;
            std::memcpy(pHolder->aStr, in_pStr, length);
            pHolder->aStr[length] = '\0';
        }

        return pHolder;
    }

    inline void SStringHolder::AddRef()
    {
        std::atomic_ref refCount(RefCount.value);

        be<uint32_t> original, incremented;
        do
        {
            original.value = refCount.load();
            incremented = original + 1;
        } while (!refCount.compare_exchange_weak(original.value, incremented.value));
    }

    inline void SStringHolder::Release()
    {
        std::atomic_ref refCount(RefCount.value);

        be<uint32_t> original, decremented;
        do
        {
            original.value = refCount.load();
            decremented = original - 1;
        } while (!refCount.compare_exchange_weak(original.value, decremented.value));

        if (decremented == 0)
            __HH_FREE(this);
    }

    inline bool SStringHolder::IsUnique() const
    {
        return RefCount == 1;
    }
}
