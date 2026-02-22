#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "install/xcontent_file_system.h"
#include <fstream>
#include <vector>
#include <filesystem>
#include <cstring>

namespace fs = std::filesystem;

class XContentTestFixture {
public:
    fs::path tempPath;
    fs::path xcontentPath;

    XContentTestFixture() {
        tempPath = fs::temp_directory_path() / "XContentFileSystemTest";
        if (fs::exists(tempPath)) {
            fs::remove_all(tempPath);
        }
        fs::create_directories(tempPath);
        xcontentPath = tempPath / "test.stfs";
    }

    ~XContentTestFixture() {
        if (fs::exists(tempPath)) {
            fs::remove_all(tempPath);
        }
    }

    void CreateExploitFile(const std::string& filename) {
        std::ofstream ofs(filename, std::ios::binary);

        // Size needs to be enough for header + metadata but small enough to trigger OOB on hash table access
        // We set magic to "CON ".

        std::vector<uint8_t> buffer(0x2000, 0); // 8KB file

        // XContentHeader at 0x0
        // magic "CON " -> 0x434F4E20 (Big Endian)
        buffer[0] = 0x43; buffer[1] = 0x4F; buffer[2] = 0x4E; buffer[3] = 0x20;

        // Header size at 0x340 -> 0x1000 (Big Endian)
        // This sets baseOffset to 0x1000
        buffer[0x340] = 0x00; buffer[0x341] = 0x00; buffer[0x342] = 0x10; buffer[0x343] = 0x00;

        // XContentMetadata at 0x344
        // volumeType at 0x344 + 0x38 -> 0 (STFS) (Big Endian)
        // Offset 0x344 + 0x38 = 0x37C
        buffer[0x37C] = 0x00; buffer[0x37D] = 0x00; buffer[0x37E] = 0x00; buffer[0x37F] = 0x00;

        // StfsVolumeDescriptor starts at 0x344 + 53 = 0x379.
        size_t descriptorOffset = 0x379;

        // descriptorLength (1 byte) -> 0x24
        buffer[descriptorOffset] = 0x24;

        // flags (1 byte) -> readOnlyFormat = 1 (bit 0)
        buffer[descriptorOffset + 2] = 0x01; // flags.bits.readOnlyFormat

        // fileTableBlockCount (2 bytes) -> 1
        // Assuming host endian (LE on x86) for uint16_t if it's not be<uint16_t>
        buffer[descriptorOffset + 3] = 0x01; buffer[descriptorOffset + 4] = 0x00;

        // fileTableBlockNumberRaw (3 bytes)
        // Set block index to 169 (0xA9)
        // parseUint24 reads as LE: bytes[0] | bytes[1]<<8 | bytes[2]<<16
        buffer[descriptorOffset + 5] = 0xA9; // low byte
        buffer[descriptorOffset + 6] = 0x00;
        buffer[descriptorOffset + 7] = 0x00;

        // File size: 0x1500 (5376 bytes)
        // Header requires up to 0x1000 (padding)
        // Reading StfsHashTable at 0x1000.
        // Accessing entry 169 at offset 0x1FD8 relative to 0x0.
        // 0x1000 + 0xFD8 = 0x1FD8 = 8152.
        // File size 0x1500 = 5376.
        // 8152 > 5376. So accessing 0x1FD8 is OOB.

        ofs.write((char*)buffer.data(), 0x1500);
        ofs.close();
    }
};

TEST_CASE("XContentFileSystem Security Tests") {
    XContentTestFixture fixture;

    SUBCASE("Out of Bounds Read in hashEntryFromBlockIndex") {
        fixture.CreateExploitFile(fixture.xcontentPath.string());

        // This should not crash, but gracefully fail (return nullptr or just fail to load)
        auto fs = XContentFileSystem::create(fixture.xcontentPath);
        CHECK(fs == nullptr);
    }
}
