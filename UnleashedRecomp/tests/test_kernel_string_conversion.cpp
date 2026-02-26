#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "kernel/string_conversion.h"
#include <vector>
#include <string>

// Helper to create be<uint16_t> string
std::vector<be<uint16_t>> ToBeUnicode(const std::wstring& str) {
    std::vector<be<uint16_t>> result;
    result.reserve(str.length());
    for (wchar_t c : str) {
        result.push_back(static_cast<uint16_t>(c));
    }
    return result;
}

TEST_CASE("RtlUnicodeToMultiByteN") {
    be<uint32_t> bytesWritten = 0;

    SUBCASE("Basic Conversion") {
        std::wstring input = L"Hello";
        auto unicode = ToBeUnicode(input);
        char output[10] = {};

        uint32_t status = RtlUnicodeToMultiByteN(output, sizeof(output), &bytesWritten, unicode.data(), unicode.size() * sizeof(uint16_t));

        CHECK(status == 0x00000000); // STATUS_SUCCESS
        CHECK(bytesWritten == 5);
        CHECK(std::string(output, 5) == "Hello");
    }

    SUBCASE("Truncation") {
        std::wstring input = L"Hello World";
        auto unicode = ToBeUnicode(input);
        char output[5] = {};

        uint32_t status = RtlUnicodeToMultiByteN(output, sizeof(output), &bytesWritten, unicode.data(), unicode.size() * sizeof(uint16_t));

        CHECK(status == 0xC0000229); // STATUS_FAIL_CHECK
        CHECK(bytesWritten == 11); // Required size
    }

    SUBCASE("Null BytesWritten") {
        std::wstring input = L"Test";
        auto unicode = ToBeUnicode(input);
        char output[10] = {};

        uint32_t status = RtlUnicodeToMultiByteN(output, sizeof(output), nullptr, unicode.data(), unicode.size() * sizeof(uint16_t));

        CHECK(status == 0x00000000);
        CHECK(std::string(output, 4) == "Test");
    }

    SUBCASE("Non-ASCII Replacement") {
        // Unicode character > 255. e.g. Euro sign € (U+20AC) or simply 0x0100
        std::vector<be<uint16_t>> unicode;
        unicode.push_back(0x41); // 'A'
        unicode.push_back(0x0100); // > 255
        unicode.push_back(0x42); // 'B'

        char output[10] = {};

        uint32_t status = RtlUnicodeToMultiByteN(output, sizeof(output), &bytesWritten, unicode.data(), unicode.size() * sizeof(uint16_t));

        CHECK(status == 0x00000000);
        CHECK(bytesWritten == 3);
        CHECK(output[0] == 'A');
        CHECK(output[1] == '?');
        CHECK(output[2] == 'B');
    }
}

TEST_CASE("RtlMultiByteToUnicodeN") {
    be<uint32_t> bytesWritten = 0;

    SUBCASE("Basic Conversion") {
        std::string input = "Hello";
        be<uint16_t> output[10];

        uint32_t status = RtlMultiByteToUnicodeN(output, sizeof(output) * 2, &bytesWritten, input.data(), input.length());

        CHECK(status == 0x00000000);
        CHECK(bytesWritten == 10);

        for (size_t i = 0; i < 5; ++i) {
            CHECK(output[i] == (uint16_t)input[i]);
        }
    }

    SUBCASE("Truncation") {
        std::string input = "Hello World";
        be<uint16_t> output[5];

        // MaxBytesInUnicodeString is in bytes. 5 * 2 = 10 bytes.
        // Input length 11.
        // It should convert min(5, 11) = 5 chars.

        uint32_t status = RtlMultiByteToUnicodeN(output, 10, &bytesWritten, input.data(), input.length());

        CHECK(status == 0x00000000);
        CHECK(bytesWritten == 10);

        for (size_t i = 0; i < 5; ++i) {
            CHECK(output[i] == (uint16_t)input[i]);
        }
    }
}
