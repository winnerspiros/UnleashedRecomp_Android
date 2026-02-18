#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <filesystem>

// ISOFileSystem depends on MemoryMappedFile which is in XenonUtils
// We assume include paths are set up correctly in CMakeLists.txt

#include "install/iso_file_system.h"

// Helper to create a minimal valid ISO file
// Structure based on ISOFileSystem implementation:
// - Magic "MICROSOFT*XBOX*MEDIA" at offset 32 * 2048 (Sector 32)
// - Root Info at Sector 32 offset 20 (Root Sector, Root Size)
// - Directory entries at specified sectors

struct DirectoryEntry {
    uint16_t nodeL = 0;
    uint16_t nodeR = 0;
    uint32_t sector = 0;
    uint32_t length = 0;
    uint8_t attributes = 0;
    std::string name;
};

void writeEntry(std::ofstream& file, const DirectoryEntry& entry, size_t offset) {
    file.seekp(offset);
    uint8_t nameLen = (uint8_t)entry.name.length();

    file.write((char*)&entry.nodeL, 2);
    file.write((char*)&entry.nodeR, 2);
    file.write((char*)&entry.sector, 4);
    file.write((char*)&entry.length, 4);
    file.write((char*)&entry.attributes, 1);
    file.write((char*)&nameLen, 1);
    file.write(entry.name.c_str(), nameLen);
}

void createTestISO(const std::filesystem::path& path, bool validMagic = true, uint32_t rootSize = 2048) {
    std::ofstream file(path, std::ios::binary);

    // Create a file large enough to hold our structures
    // Sector size = 2048
    // We use up to Sector 40 for safety
    std::vector<char> buffer(40 * 2048, 0);
    file.write(buffer.data(), buffer.size());

    // Magic at Sector 32 (Offset 32 * 2048)
    size_t magicOffset = 32 * 2048;
    const char* magic = validMagic ? "MICROSOFT*XBOX*MEDIA" : "INVALID*MAGIC*STRING";
    file.seekp(magicOffset);
    file.write(magic, strlen(magic));

    // Root Info at Sector 32 Offset 20
    // Root Sector = 33
    uint32_t rootSector = 33;
    file.seekp(magicOffset + 20);
    file.write((char*)&rootSector, 4);
    file.write((char*)&rootSize, 4);

    // Root Directory Table at Sector 33
    // Entry 0: "subdir" (Directory)
    // - nodeL: 5 (offset to next entry / 4)
    // - sector: 34
    // - length: 2048
    // - attributes: 0x10
    DirectoryEntry rootDirEntry;
    rootDirEntry.nodeL = 5; // Offset 20 / 4
    rootDirEntry.sector = 34;
    rootDirEntry.length = 2048;
    rootDirEntry.attributes = 0x10;
    rootDirEntry.name = "subdir";
    writeEntry(file, rootDirEntry, 33 * 2048);

    // Entry 1: "test.txt" (File) at offset 20 (14 + 6 bytes)
    DirectoryEntry fileEntry;
    fileEntry.sector = 35;
    fileEntry.length = 12;
    fileEntry.attributes = 0;
    fileEntry.name = "test.txt";
    writeEntry(file, fileEntry, 33 * 2048 + 20);

    // Subdirectory Table at Sector 34
    // Entry 0: "sub.txt" (File)
    DirectoryEntry subFileEntry;
    subFileEntry.sector = 36;
    subFileEntry.length = 7;
    subFileEntry.attributes = 0;
    subFileEntry.name = "sub.txt";
    writeEntry(file, subFileEntry, 34 * 2048);

    // File Content "test.txt" at Sector 35
    file.seekp(35 * 2048);
    file.write("Hello World!", 12);

    // File Content "sub.txt" at Sector 36
    file.seekp(36 * 2048);
    file.write("Content", 7);
}

TEST_CASE("ISOFileSystem Tests") {
    std::filesystem::path isoPath = "test_iso_fs.iso";

    SUBCASE("Valid ISO Parsing") {
        createTestISO(isoPath);

        ISOFileSystem isoFs(isoPath);
        CHECK(!isoFs.empty());
        CHECK(isoFs.getName() == "test_iso_fs.iso");

        // Check root file
        CHECK(isoFs.exists("test.txt"));
        CHECK(isoFs.getSize("test.txt") == 12);

        std::vector<uint8_t> buffer(12);
        CHECK(isoFs.load("test.txt", buffer.data(), buffer.size()));
        CHECK(buffer.size() == 12);
        std::string content(buffer.begin(), buffer.end());
        CHECK(content == "Hello World!");

        // Check subdirectory file
        // Note: Directory separator is appended in ISOFileSystem code: fileNameUTF8 + "/"
        CHECK(isoFs.exists("subdir/sub.txt"));
        CHECK(isoFs.getSize("subdir/sub.txt") == 7);

        buffer.resize(7);
        CHECK(isoFs.load("subdir/sub.txt", buffer.data(), buffer.size()));
        CHECK(buffer.size() == 7);
        std::string subContent(buffer.begin(), buffer.end());
        CHECK(subContent == "Content");

        // Check non-existent file
        CHECK(!isoFs.exists("nonexistent.txt"));
        CHECK(!isoFs.exists("subdir/missing.txt"));
    }

    SUBCASE("Invalid Magic String") {
        createTestISO(isoPath, false); // Invalid magic

        ISOFileSystem isoFs(isoPath);
        // Should fail to initialize fully -> empty file map or possibly empty() returns true if mappedFile closed?
        // In constructor: if (!magicFound) mappedFile.close(); return;
        // So mappedFile.isOpen() should be false.
        CHECK(isoFs.empty());
        CHECK(!isoFs.exists("test.txt"));
    }

    SUBCASE("Invalid Root Size") {
        createTestISO(isoPath, true, 10); // Size < 13 (MinRootSize)

        ISOFileSystem isoFs(isoPath);
        // Constructor checks rootSize < 13 -> mappedFile.close(); return;
        CHECK(isoFs.empty());
    }

    SUBCASE("Create Factory Method") {
        createTestISO(isoPath);
        auto isoFs = ISOFileSystem::create(isoPath);
        CHECK(isoFs != nullptr);
        CHECK(!isoFs->empty());

        createTestISO(isoPath, false); // Invalid
        auto invalidFs = ISOFileSystem::create(isoPath);
        CHECK(invalidFs == nullptr);
    }

    // Clean up
    if (std::filesystem::exists(isoPath)) {
        std::filesystem::remove(isoPath);
    }
}
