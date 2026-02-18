#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <vector>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include <map>
#include <utility>
#include <cassert>
#include <iostream>

// Dependencies for ModLoader
#include <ankerl/unordered_dense.h>
#include <xxhash.h>
#include "../xxHashMap.h"
#include "../mod/mod_loader.h"

// Include implementation to test
#include "../mod/resolve_path.inl"

// Mock Init function to satisfy linker if needed (though since we don't call it, smart linker might strip it, but better safe)
void ModLoader::Init() {}

TEST_CASE("ModLoader::ResolvePath") {
    // Reset global state
    g_mods.clear();
    g_modFileIndex.clear();
    ModLoader::s_saveFilePath.clear();

    // Test Case 1: Empty Mods
    SUBCASE("Empty Mods") {
        CHECK(ModLoader::ResolvePath("test.txt").empty());
    }

    // Test Case 2: Save Path Redirection
    SUBCASE("Save Path Redirection") {
        ModLoader::s_saveFilePath = "C:/SavePath/SYS-DATA";

        // SYS-DATA root
        CHECK(ModLoader::ResolvePath("save:\\SYS-DATA") == "C:/SavePath/SYS-DATA");

        // Relative path under save root
        // Expected: parent_path() / path
        // parent_path of "C:/SavePath/SYS-DATA" is "C:/SavePath"
        // path is "config.bin" (after removing "save:\\")
        // Result: "C:/SavePath/config.bin"
        CHECK(ModLoader::ResolvePath("save:\\config.bin") == "C:/SavePath/config.bin");

        // Empty save path
        ModLoader::s_saveFilePath.clear();
        CHECK(ModLoader::ResolvePath("save:\\SYS-DATA").empty());
    }

    // Test Case 3: Basic File Resolution
    SUBCASE("Basic File Resolution") {
        Mod mod;
        mod.type = ModType::HMM;
        mod.includeDirs.push_back("C:/Mod1");
        g_mods.push_back(mod);

        // Add file to index manually
        // Key must be lowercase relative path using '/'
        std::string key = "data/file.txt";
        // Index entry: {modIndex, fullPath}
        g_modFileIndex[key].emplace_back(0, "C:/Mod1/data/file.txt");

        // Forward slash
        CHECK(ModLoader::ResolvePath("data/file.txt") == "C:/Mod1/data/file.txt");

        // Backslash
        CHECK(ModLoader::ResolvePath("data\\file.txt") == "C:/Mod1/data/file.txt");

        // Case insensitive
        CHECK(ModLoader::ResolvePath("DATA\\FILE.TXT") == "C:/Mod1/data/file.txt");

        // Non-existent file
        CHECK(ModLoader::ResolvePath("unknown.txt").empty());
    }

    // Test Case 4: Merge Logic (UMM vs HMM)
    SUBCASE("Merge Logic") {
        // Mod 0: UMM (Merge=True) - Has file
        Mod mod0;
        mod0.type = ModType::UMM;
        mod0.merge = true;
        mod0.includeDirs.push_back("C:/ModBase");
        g_mods.push_back(mod0); // Index 0

        // Mod 1: HMM - Has file
        Mod mod1;
        mod1.type = ModType::HMM;
        mod1.includeDirs.push_back("C:/ModOverride");
        g_mods.push_back(mod1); // Index 1

        // Scenario A: Mergeable file (.arl)
        // Mod0 has it, Mod1 has it.
        // Index has Mod0 then Mod1.
        std::string key = "sound.arl";
        g_modFileIndex[key].emplace_back(0, "C:/ModBase/sound.arl");
        g_modFileIndex[key].emplace_back(1, "C:/ModOverride/sound.arl");

        // Expectation: Mod0 is skipped because it's UMM & Merge & File is mergeable & Not ReadOnly
        // Result: Mod1 path
        CHECK(ModLoader::ResolvePath("sound.arl") == "C:/ModOverride/sound.arl");

        // Scenario B: Non-mergeable file (.txt)
        key = "info.txt";
        g_modFileIndex[key].emplace_back(0, "C:/ModBase/info.txt");
        g_modFileIndex[key].emplace_back(1, "C:/ModOverride/info.txt");

        // Expectation: Mod0 is returned because .txt is not in canBeMerged list
        CHECK(ModLoader::ResolvePath("info.txt") == "C:/ModBase/info.txt");

        // Scenario C: Mergeable file but Mod0 is ReadOnly for this file
        key = "read_only.arl";
        // Mark as ReadOnly in Mod0
        // Note: fsPath used in check is lowercase? No, fsPath is original path (replaced slashes).
        // ModLoader::ResolvePath converts input path to lowercase for key lookup,
        // but uses original casing (with /) for ReadOnly check.
        // Let's use specific casing to verify.
        std::string pathStr = "read_only.arl"; // lowercase for simplicity
        g_mods[0].readOnly.emplace(pathStr);

        g_modFileIndex[pathStr].emplace_back(0, "C:/ModBase/read_only.arl");
        g_modFileIndex[pathStr].emplace_back(1, "C:/ModOverride/read_only.arl");

        // Expectation: Mod0 is NOT skipped because it is ReadOnly.
        // Result: Mod0 path
        CHECK(ModLoader::ResolvePath("read_only.arl") == "C:/ModBase/read_only.arl");
    }

    // Test Case 5: Caching
    SUBCASE("Caching") {
        Mod mod;
        mod.type = ModType::HMM;
        mod.includeDirs.push_back("C:/Mod1");
        g_mods.push_back(mod);

        std::string key = "cached.txt";
        g_modFileIndex[key].emplace_back(0, "C:/Mod1/cached.txt");

        // First call populates cache
        CHECK(ModLoader::ResolvePath("cached.txt") == "C:/Mod1/cached.txt");

        // Modify index (hack to verify cache is used)
        g_modFileIndex[key].clear();

        // Second call should still return cached path
        CHECK(ModLoader::ResolvePath("cached.txt") == "C:/Mod1/cached.txt");
    }
}
