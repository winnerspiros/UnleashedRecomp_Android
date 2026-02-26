#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <filesystem>
#include <string_view>
#include <vector>
#include <cstring>

#include <user/persistent_data.h>

TEST_CASE("PersistentData Verification")
{
    SUBCASE("VerifySignature")
    {
        PersistentData data;

        // Test with correct signature
        const char correctSig[4] = EXT_SIGNATURE;
        std::memcpy(data.Signature, correctSig, sizeof(data.Signature));
        CHECK(data.VerifySignature() == true);

        // Test with incorrect signature
        const char wrongSig[4] = { 'B', 'A', 'D', ' ' };
        std::memcpy(data.Signature, wrongSig, sizeof(data.Signature));
        CHECK(data.VerifySignature() == false);

        // Test partial match
        const char partialSig[4] = { 'E', 'X', 'T', 'X' };
        std::memcpy(data.Signature, partialSig, sizeof(data.Signature));
        CHECK(data.VerifySignature() == false);
    }

    SUBCASE("VerifyVersion")
    {
        PersistentData data;

        // Correct version
        data.Version = EXT_VERSION;
        CHECK(data.VerifyVersion() == true);

        // Older version (should be supported as <= check)
        // If EXT_VERSION > 0, check 0 or EXT_VERSION-1
        if (EXT_VERSION > 0)
        {
            data.Version = EXT_VERSION - 1;
            CHECK(data.VerifyVersion() == true);
        }

        // Newer version (not supported)
        data.Version = EXT_VERSION + 1;
        CHECK(data.VerifyVersion() == false);
    }
}
