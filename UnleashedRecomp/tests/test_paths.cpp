#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <filesystem>
#include <user/paths.h>
#include <fstream>
#include <cstdlib>

// Helper to create empty files
void create_file(const std::filesystem::path& p) {
    std::ofstream f(p);
}

TEST_CASE("Paths Test") {
    // Save original g_executableRoot
    auto originalRoot = g_executableRoot;

    // Create a temp directory for our test sandbox
    auto tempDir = std::filesystem::temp_directory_path() / "test_paths_sandbox";
    std::filesystem::create_directories(tempDir);

    // Redirect g_executableRoot to our sandbox
    g_executableRoot = tempDir;

    SUBCASE("CheckPortable returns true when portable.txt exists") {
        create_file(tempDir / "portable.txt");
        CHECK(CheckPortable() == true);

        std::filesystem::path path = BuildUserPath();
        CHECK(path == tempDir);

        std::filesystem::remove(tempDir / "portable.txt");
    }

    SUBCASE("CheckPortable returns false when portable.txt is missing") {
        if(std::filesystem::exists(tempDir / "portable.txt"))
            std::filesystem::remove(tempDir / "portable.txt");

        CHECK(CheckPortable() == false);
    }

#if defined(__linux__) || defined(__APPLE__)
    SUBCASE("BuildUserPath uses HOME env var on Linux/macOS") {
        // Ensure not portable
        if(std::filesystem::exists(tempDir / "portable.txt"))
            std::filesystem::remove(tempDir / "portable.txt");

        // Mock HOME directory
        auto mockHome = tempDir / "home";
        std::filesystem::create_directories(mockHome);

        // Save old HOME
        const char* oldHome = getenv("HOME");
        // Update HOME env var to point to our mock home
        setenv("HOME", mockHome.c_str(), 1);

        // Determine config path based on platform
#if defined(__linux__)
        std::filesystem::path configDir = mockHome / ".config";
#elif defined(__APPLE__)
        std::filesystem::path configDir = mockHome / "Library" / "Application Support";
#endif

        // Case 1: Config directory exists
        std::filesystem::create_directories(configDir);

        std::filesystem::path path1 = BuildUserPath();
        // Expected: $HOME/.config/UnleashedRecomp
        CHECK(path1 == configDir / USER_DIRECTORY);

        // Case 2: Config directory does not exist
        std::filesystem::remove_all(configDir);

        std::filesystem::path path2 = BuildUserPath();
        // Fallback behavior: ~/.UnleashedRecomp
        CHECK(path2 == mockHome / ("." USER_DIRECTORY));

        // Restore HOME
        if (oldHome) setenv("HOME", oldHome, 1);
        else unsetenv("HOME");
    }
#endif

    // Cleanup
    std::filesystem::remove_all(tempDir);
    g_executableRoot = originalRoot;
}
