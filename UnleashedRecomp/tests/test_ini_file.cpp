#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <xxhash.h>
#include <ankerl/unordered_dense.h>
#include <filesystem>
#include <fstream>
#include <memory>
#include <charconv>

#include "mod/ini_file.h"

class TempIniFile {
    std::filesystem::path m_path;
public:
    TempIniFile(std::string_view content) {
        auto tmpDir = std::filesystem::temp_directory_path();
        size_t hash = std::hash<std::string_view>{}(content);
        // Use a unique name to avoid conflicts
        m_path = tmpDir / ("test_ini_" + std::to_string(hash) + "_" + std::to_string(std::rand()) + ".ini");

        std::ofstream file(m_path);
        file << content;
        file.close();
    }

    ~TempIniFile() {
        if (std::filesystem::exists(m_path)) {
            std::filesystem::remove(m_path);
        }
    }

    const std::filesystem::path& path() const { return m_path; }
};

TEST_CASE("IniFile Parsing") {
    IniFile ini;

    SUBCASE("Basic Section and Properties") {
        TempIniFile file(R"(
[Section1]
Key1=Value1
Key2=Value2

[Section2]
KeyA=ValueA
)");
        CHECK(ini.read(file.path()));

        CHECK(ini.contains("Section1"));
        CHECK(ini.contains("Section2"));
        CHECK_FALSE(ini.contains("Section3"));

        CHECK(ini.getString("Section1", "Key1", "") == "Value1");
        CHECK(ini.getString("Section1", "Key2", "") == "Value2");
        CHECK(ini.getString("Section2", "KeyA", "") == "ValueA");
        CHECK(ini.getString("Section1", "NonExistent", "Default") == "Default");
    }

    SUBCASE("Whitespace Handling") {
        TempIniFile file(R"(
[  Spaced Section  ]
  Key  =  Value
)");
        CHECK(ini.read(file.path()));

        CHECK(ini.contains("  Spaced Section  "));
        CHECK(ini.getString("  Spaced Section  ", "Key", "") == "Value");
    }

    SUBCASE("Comments") {
        TempIniFile file(R"(
; This is a comment
[Section]
Key=Value
; Another comment
Key2=Value2
)");
        CHECK(ini.read(file.path()));
        CHECK(ini.contains("Section"));
        CHECK(ini.getString("Section", "Key", "") == "Value");
        CHECK(ini.getString("Section", "Key2", "") == "Value2");
    }

    SUBCASE("Quoted Values") {
        TempIniFile file(R"(
[Section]
Key="Quoted Value"
KeyWithSpaces="  Spaces  "
)");
        CHECK(ini.read(file.path()));
        CHECK(ini.getString("Section", "Key", "") == "Quoted Value");
        CHECK(ini.getString("Section", "KeyWithSpaces", "") == "  Spaces  ");
    }

    SUBCASE("Types") {
        TempIniFile file(R"(
[Types]
Int=123
BoolTrue=true
BoolFalse=false
BoolYes=yes
BoolNo=no
Bool1=1
Bool0=0
Float=12.34
)");
        CHECK(ini.read(file.path()));

        CHECK(ini.get<int>("Types", "Int", 0) == 123);
        CHECK(ini.getBool("Types", "BoolTrue", false) == true);
        CHECK(ini.getBool("Types", "BoolFalse", true) == false);
        CHECK(ini.getBool("Types", "BoolYes", false) == true);
        CHECK(ini.getBool("Types", "BoolNo", true) == false);
        CHECK(ini.getBool("Types", "Bool1", false) == true);
        CHECK(ini.getBool("Types", "Bool0", true) == false);
    }

    SUBCASE("Error Handling") {
        CHECK_FALSE(ini.read("non_existent_file.ini"));

        TempIniFile emptyFile("");
        CHECK(ini.read(emptyFile.path()));
        CHECK_FALSE(ini.contains("Section"));
    }
}
