#include <iostream>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

// Include dependencies missing from headers due to PCH reliance
#include <xxhash.h>
#include <ankerl/unordered_dense.h>

#include "mod/ini_file.h"

int main() {
    // 1. Setup: Create a temporary ini file
    std::filesystem::path tempIniPath = "benchmark_test.ini";
    {
        std::ofstream iniOut(tempIniPath);
        if (!iniOut) {
            std::cerr << "Failed to create temp ini file." << std::endl;
            return 1;
        }
        iniOut << "[Section1]\n";
        iniOut << "Key1=Value1\n";
        iniOut << "Key2=Value2\n";
        iniOut.close();
    }

    IniFile ini;
    if (!ini.read(tempIniPath)) {
        std::cerr << "Failed to read ini file: " << tempIniPath << std::endl;
        std::filesystem::remove(tempIniPath);
        return 1;
    }

    const int iterations = 1000000;
    // Use a long string to prevent SSO
    std::string defaultVal = "DefaultLongStringValueToPreventSSO_1234567890_1234567890_1234567890";

    // 2. Measure
    auto start = std::chrono::high_resolution_clock::now();

    volatile size_t dummy = 0;
    for (int i = 0; i < iterations; ++i) {
        // Test found key
        std::string val1 = ini.getString("Section1", "Key1", defaultVal);
        // Test missing key (returns defaultVal)
        std::string val2 = ini.getString("Section1", "NonExistentKey", defaultVal);
        dummy += val1.size() + val2.size();
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout << "Time taken for " << iterations << " iterations: " << duration.count() << " ms" << std::endl;

    // Cleanup
    std::filesystem::remove(tempIniPath);

    return 0;
}
