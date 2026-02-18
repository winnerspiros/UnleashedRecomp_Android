#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#define UNIT_TEST_FILESYSTEM

// Setup PPC environment macros BEFORE including stdafx.h (if it includes ppc headers)
#define PPC_CONFIG_H_INCLUDED
#define PPC_IMAGE_BASE 0x82000000
#define PPC_IMAGE_SIZE 0x01000000
#define PPC_CODE_BASE 0x82000000

#include <stdafx.h>

// Include the real PPCContext definition if not already in stdafx.h
#include <tools/XenonRecomp/XenonUtils/ppc_context.h>

// Redefine PPC_LOOKUP_FUNC for simple mocking
#undef PPC_LOOKUP_FUNC
#define PPC_LOOKUP_FUNC(base, guest) (*(PPCFunc**)((uint8_t*)base + guest))

#include <string>
#include <filesystem>
#include <vector>
#include <iostream>
#include <map>
#include <fstream>
#include <algorithm> // For std::replace
#include <unordered_map>

// Mock ModLoader
#include <mod/mod_loader.h>

// Mock storage for ModLoader
std::map<std::string, std::filesystem::path> g_mockModPaths;

std::filesystem::path ModLoader::ResolvePath(std::string_view path)
{
    std::string pathStr(path);
    if (g_mockModPaths.count(pathStr))
    {
        return g_mockModPaths[pathStr];
    }
    return {};
}

std::vector<std::filesystem::path>* ModLoader::GetIncludeDirectories(size_t modIndex)
{
    return nullptr;
}

void ModLoader::Init() {}

// Mock XamGetRootPath
#include <kernel/xam.h>

std::unordered_map<std::string, std::string> g_mockRoots;

std::string_view XamGetRootPath(const std::string_view& root)
{
    std::string rootStr(root);
    if (g_mockRoots.count(rootStr))
    {
        return g_mockRoots[rootStr];
    }
    return {};
}

// Mock Logger
#include <os/logger.h>
#include <fmt/core.h>

namespace os::logger
{
    void Init() {}
    void Log(const std::string_view str, ELogType type, const char* func)
    {
        // Mock logging
    }
}

// Mock Config definitions
#include <user/config.h>
std::vector<IConfigDef*> g_configDefinitions;

// Mock Memory global (declared extern in memory.h)
#include <kernel/memory.h>
Memory g_memory;
Memory::Memory() {} // Dummy constructor

// Include the file under test
// We need to make sure we don't link against the actual file_system.obj
#include "../kernel/io/file_system.cpp"

// Helpers for test setup
void ClearMocks()
{
    g_mockModPaths.clear();
    g_mockRoots.clear();
    ModLoader::s_isLogTypeConsole = false;
}

TEST_CASE("FileSystem::ResolvePath - Mod Loading")
{
    ClearMocks();
    g_mockModPaths["game:\\test.txt"] = "mods/mod1/test.txt";

    SUBCASE("CheckForMods = true, Mod exists")
    {
        ModLoader::s_isLogTypeConsole = true; // Exercise logging path
        auto result = FileSystem::ResolvePath("game:\\test.txt", true);
        CHECK(result.generic_string() == "mods/mod1/test.txt");
    }

    SUBCASE("CheckForMods = false, Mod exists")
    {
        // Should ignore mod and resolve normally
        // Assuming normal resolution for "game:\" needs XamGetRootPath
        g_mockRoots["game"] = "hdd1/game";
        auto result = FileSystem::ResolvePath("game:\\test.txt", false);
        CHECK(result.generic_string() == "hdd1/game/test.txt");
    }

    SUBCASE("CheckForMods = true, Mod does not exist")
    {
        g_mockRoots["game"] = "hdd1/game";
        auto result = FileSystem::ResolvePath("game:\\other.txt", true);
        CHECK(result.generic_string() == "hdd1/game/other.txt");
    }
}

TEST_CASE("FileSystem::ResolvePath - Root Resolution")
{
    ClearMocks();
    g_mockRoots["game"] = "hdd1/game";
    g_mockRoots["update"] = "hdd1/update";

    SUBCASE("Standard Root")
    {
        auto result = FileSystem::ResolvePath("game:\\file.xex", false);
        CHECK(result.generic_string() == "hdd1/game/file.xex");
    }

    SUBCASE("Unknown Root")
    {
        auto result = FileSystem::ResolvePath("unknown:\\file.txt", false);
        CHECK(result.generic_string() == "file.txt");
    }

    SUBCASE("No Root (Relative Path)")
    {
        auto result = FileSystem::ResolvePath("folder\\file.txt", false);
        CHECK(result.generic_string() == "folder/file.txt");
    }
}

TEST_CASE("FileSystem::ResolvePath - Update Partition Logic")
{
    ClearMocks();

    std::filesystem::path tempDir = std::filesystem::temp_directory_path() / "recomp_test_fs";
    std::filesystem::create_directories(tempDir / "update");
    std::filesystem::create_directories(tempDir / "game");

    std::filesystem::path updateFile = tempDir / "update" / "patch.xex";
    {
        std::ofstream f(updateFile);
        f << "dummy";
    }

    std::string updateRoot = (tempDir / "update").string();
    std::replace(updateRoot.begin(), updateRoot.end(), '\\', '/');

    std::string gameRoot = (tempDir / "game").string();
    std::replace(gameRoot.begin(), gameRoot.end(), '\\', '/');

    g_mockRoots["game"] = gameRoot;
    g_mockRoots["update"] = updateRoot;

    SUBCASE("File exists in update partition")
    {
        auto result = FileSystem::ResolvePath("game:\\patch.xex", false);
        std::string expected = updateRoot + "/patch.xex";
        CHECK(result.generic_string() == std::filesystem::path(expected).generic_string());
    }

    SUBCASE("File does not exist in update partition")
    {
        auto result = FileSystem::ResolvePath("game:\\original.xex", false);
        std::string expected = gameRoot + "/original.xex";
        CHECK(result.generic_string() == std::filesystem::path(expected).generic_string());
    }

    // Cleanup
    std::filesystem::remove_all(tempDir);
}
