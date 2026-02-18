#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <filesystem>
#include <functional>
#include <iostream>
#include <fstream>
#include <span>

// Include headers to get class definitions
#include "install/virtual_file_system.h"
#include "install/iso_file_system.h"
#include "install/xcontent_file_system.h"
#include "install/directory_file_system.h"
#include "install/installer.h"
#include "install/hashes/game.h"
#include "install/hashes/update.h"
#include "install/hashes/spagonia.h"
#include "xex_patcher.h"
#include "memory_mapped_file.h"

#include <cstdint>
#include <utility>
#include <iterator>
#include <cstddef>

#include "install/hashes/apotos_shamar.cpp"
#include "install/hashes/chunnan.cpp"
#include "install/hashes/empire_city_adabat.cpp"
#include "install/hashes/game.cpp"
#include "install/hashes/holoska.cpp"
#include "install/hashes/mazuri.cpp"
#include "install/hashes/spagonia.cpp"
#include "install/hashes/update.cpp"

// --- Stub Implementations ---

// MemoryMappedFile Stubs
MemoryMappedFile::MemoryMappedFile() {}
MemoryMappedFile::MemoryMappedFile(const std::filesystem::path &path) {}
MemoryMappedFile::MemoryMappedFile(MemoryMappedFile &&other) {}
MemoryMappedFile::~MemoryMappedFile() {}
bool MemoryMappedFile::open(const std::filesystem::path &path) { return false; }
void MemoryMappedFile::close() {}
bool MemoryMappedFile::isOpen() const { return false; }
uint8_t *MemoryMappedFile::data() const { return nullptr; }
size_t MemoryMappedFile::size() const { return 0; }

// ISOFileSystem Stubs
ISOFileSystem::ISOFileSystem(const std::filesystem::path &isoPath) : name("ISOFileSystemStub") {}
bool ISOFileSystem::load(const std::string &path, uint8_t *fileData, size_t fileDataMaxByteCount) const { return false; }
size_t ISOFileSystem::getSize(const std::string &path) const { return 0; }
bool ISOFileSystem::exists(const std::string &path) const { return false; }
const std::string &ISOFileSystem::getName() const { return name; }
bool ISOFileSystem::empty() const { return true; }
std::unique_ptr<ISOFileSystem> ISOFileSystem::create(const std::filesystem::path &isoPath) { return nullptr; }

// XContentFileSystem Stubs
XContentFileSystem::XContentFileSystem(const std::filesystem::path &contentPath) : name("XContentFileSystemStub") {}
bool XContentFileSystem::load(const std::string &path, uint8_t *fileData, size_t fileDataMaxByteCount) const { return false; }
size_t XContentFileSystem::getSize(const std::string &path) const { return 0; }
bool XContentFileSystem::exists(const std::string &path) const { return false; }
const std::string &XContentFileSystem::getName() const { return name; }
bool XContentFileSystem::empty() const { return true; }
std::unique_ptr<XContentFileSystem> XContentFileSystem::create(const std::filesystem::path &contentPath) { return nullptr; }
bool XContentFileSystem::check(const std::filesystem::path &contentPath) { return false; }

// XexPatcher Stubs
XexPatcher::Result XexPatcher::apply(const uint8_t* xexBytes, size_t xexBytesSize, const uint8_t* patchBytes, size_t patchBytesSize, std::vector<uint8_t> &outBytes, bool skipData) {
    return XexPatcher::Result::Success;
}

// Global state to track patcher calls
struct PatcherCall {
    std::filesystem::path baseXex;
    std::filesystem::path patchXex;
    std::filesystem::path newXex;
};
std::vector<PatcherCall> g_patcherCalls;
bool g_patcherShouldFail = false;

XexPatcher::Result XexPatcher::apply(const std::filesystem::path &baseXexPath, const std::filesystem::path &patchXexPath, const std::filesystem::path &newXexPath) {
    g_patcherCalls.push_back({baseXexPath, patchXexPath, newXexPath});
    if (g_patcherShouldFail) return XexPatcher::Result::PatchFailed;

    // Simulate creating the output file
    std::ofstream os(newXexPath);
    if (!os) return XexPatcher::Result::FileWriteFailed;
    os << "patched_content";
    return XexPatcher::Result::Success;
}

// Include the implementation to test
#include "install/installer.cpp"

// --- Tests ---

// Mock VFS Implementation
struct MockVFS : public VirtualFileSystem {
    std::map<std::string, std::vector<uint8_t>> files;
    std::string name;

    MockVFS(const std::string& n = "MockVFS") : name(n) {}

    bool load(const std::string &path, uint8_t *fileData, size_t fileDataMaxByteCount) const override {
        auto it = files.find(path);
        if (it == files.end()) return false;
        if (fileDataMaxByteCount < it->second.size()) return false;
        std::copy(it->second.begin(), it->second.end(), fileData);
        return true;
    }

    size_t getSize(const std::string &path) const override {
        auto it = files.find(path);
        if (it == files.end()) return 0;
        return it->second.size();
    }

    bool exists(const std::string &path) const override {
        return files.find(path) != files.end();
    }

    const std::string &getName() const override {
        return name;
    }

    void addFile(const std::string& path, const std::string& content) {
        std::vector<uint8_t> data(content.begin(), content.end());
        files[path] = data;
    }

    void addFile(const std::string& path, size_t size) {
        std::vector<uint8_t> data(size, 0);
        files[path] = data;
    }
};

TEST_CASE("Installer::install - Happy Path") {
    // Reset globals
    g_patcherCalls.clear();
    g_patcherShouldFail = false;

    // Setup Temp Dir
    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "InstallerTest_Happy";
    std::filesystem::remove_all(tempDir);
    std::filesystem::create_directories(tempDir);

    // Setup Sources
    Installer::Sources sources;

    // Game Source
    auto gameVfs = std::make_unique<MockVFS>("GameVFS");
    // Add files required by GameFiles list
    // Note: installer.cpp checks hashes if skipHashChecks is false. We will set skipHashChecks = true.
    for (size_t i = 0; i < GameFilesSize; ++i) {
        gameVfs->addFile(GameFiles[i].first, 128); // dummy size
    }
    sources.game = std::move(gameVfs);

    // Update Source
    auto updateVfs = std::make_unique<MockVFS>("UpdateVFS");
    for (size_t i = 0; i < UpdateFilesSize; ++i) {
        updateVfs->addFile(UpdateFiles[i].first, 64);
    }
    sources.update = std::move(updateVfs);

    // Journal
    Journal journal;
    bool progressCalled = false;
    auto progress = [&]() { progressCalled = true; return true; };

    // Run Install
    bool result = Installer::install(sources, tempDir, true /* skipHashChecks */, journal, std::chrono::seconds(0), progress);

    // Verify
    CHECK(result == true);
    CHECK(journal.lastResult == Journal::Result::Success);
    CHECK(progressCalled);

    // Check files exist
    CHECK(std::filesystem::exists(tempDir / "game" / "default.xex"));
    CHECK(std::filesystem::exists(tempDir / "update" / "default.xexp"));
    CHECK(std::filesystem::exists(tempDir / "patched" / "default.xex"));

    // Check patcher interaction
    REQUIRE(g_patcherCalls.size() == 1);
    CHECK(g_patcherCalls[0].baseXex == tempDir / "game" / "default.xex");
    CHECK(g_patcherCalls[0].patchXex == tempDir / "update" / "default.xexp");
    CHECK(g_patcherCalls[0].newXex == tempDir / "patched" / "default.xex");

    // Cleanup
    std::filesystem::remove_all(tempDir);
}

TEST_CASE("Installer::install - Patcher Failure") {
    g_patcherCalls.clear();
    g_patcherShouldFail = true;

    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "InstallerTest_Fail";
    std::filesystem::remove_all(tempDir);
    std::filesystem::create_directories(tempDir);

    Installer::Sources sources;

    auto gameVfs = std::make_unique<MockVFS>("GameVFS");
    for (size_t i = 0; i < GameFilesSize; ++i) gameVfs->addFile(GameFiles[i].first, 128);
    sources.game = std::move(gameVfs);

    auto updateVfs = std::make_unique<MockVFS>("UpdateVFS");
    for (size_t i = 0; i < UpdateFilesSize; ++i) updateVfs->addFile(UpdateFiles[i].first, 64);
    sources.update = std::move(updateVfs);

    Journal journal;
    bool result = Installer::install(sources, tempDir, true, journal, std::chrono::seconds(0), []{return true;});

    CHECK(result == false);
    CHECK(journal.lastResult == Journal::Result::PatchProcessFailed);
    CHECK(journal.lastPatcherResult == XexPatcher::Result::PatchFailed);

    std::filesystem::remove_all(tempDir);
}

TEST_CASE("Installer::install - Missing Update Files") {
    g_patcherCalls.clear();
    g_patcherShouldFail = false;

    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "InstallerTest_Missing";
    std::filesystem::remove_all(tempDir);
    std::filesystem::create_directories(tempDir);

    Installer::Sources sources;

    // Game is fine
    auto gameVfs = std::make_unique<MockVFS>("GameVFS");
    for (size_t i = 0; i < GameFilesSize; ++i) gameVfs->addFile(GameFiles[i].first, 128);
    sources.game = std::move(gameVfs);

    // Update is missing files
    auto updateVfs = std::make_unique<MockVFS>("UpdateVFS");
    // Empty
    sources.update = std::move(updateVfs);

    Journal journal;
    bool result = Installer::install(sources, tempDir, true, journal, std::chrono::seconds(0), []{return true;});

    CHECK(result == false);
    // It should fail on file copy
    CHECK(journal.lastResult == Journal::Result::FileMissing);

    std::filesystem::remove_all(tempDir);
}

TEST_CASE("Installer::install - DLC Installation") {
    g_patcherCalls.clear();
    g_patcherShouldFail = false;

    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "InstallerTest_DLC";
    std::filesystem::remove_all(tempDir);
    std::filesystem::create_directories(tempDir);

    Installer::Sources sources;

    // Only DLC, no game/update
    sources.game = nullptr;
    sources.update = nullptr;

    // Add one DLC
    Installer::DLCSource dlcSource;
    auto dlcVfs = std::make_unique<MockVFS>("DLCVFS");

    // Spagonia files
    for (size_t i = 0; i < SpagoniaFilesSize; ++i) {
        dlcVfs->addFile(SpagoniaFiles[i].first, 100);
    }
    // Also validation file which is "DLC.xml" usually but logic uses validationFile param passed to copyFiles
    // In Installer::install: copyFiles(..., DLCValidationFile, ...)
    // DLCValidationFile is "DLC.xml".
    // SpagoniaFiles list likely contains it?
    // Let's check SpagoniaFiles? We can just add it to be sure if it's not in the list (but it should be).
    // The copyFiles logic looks for validation file in the file list.
    // So we just need to ensure the VFS has everything in SpagoniaFiles.

    dlcSource.sourceVfs = std::move(dlcVfs);
    dlcSource.filePairs = { SpagoniaFiles, SpagoniaFilesSize };
    dlcSource.fileHashes = nullptr; // Skip hash checks
    dlcSource.targetSubDirectory = "dlc/Spagonia";

    sources.dlc.push_back(std::move(dlcSource));

    Journal journal;
    bool result = Installer::install(sources, tempDir, true, journal, std::chrono::seconds(0), []{return true;});

    CHECK(result == true);
    CHECK(journal.lastResult == Journal::Result::Success);

    CHECK(std::filesystem::exists(tempDir / "dlc" / "Spagonia" / "DLC.xml"));

    std::filesystem::remove_all(tempDir);
}
