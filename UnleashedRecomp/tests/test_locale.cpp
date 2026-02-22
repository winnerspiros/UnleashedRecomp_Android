#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>
#include <functional>
#include <tuple>
#include <set>
#include <map>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdint>

#include <fmt/core.h>
#include <toml++/toml.hpp>
#include <SDL.h>

#include <locale/locale.h>
#include <user/config.h>

// Global definitions required by config.h / ConfigDef linking
std::vector<IConfigDef*> g_configDefinitions;

// Define Config::Language. We use the simple constructor which doesn't require
// locale maps or enum templates, as Localise only uses the value.
ConfigDef<ELanguage> Config::Language("System", "Language", ELanguage::English);

// Include the source file directly to provide missing headers context
#include "../locale/locale.cpp"

TEST_CASE("Localise Functionality") {
    // Save original state
    ELanguage originalLanguage = Config::Language;

    SUBCASE("Standard Retrieval - English") {
        Config::Language = ELanguage::English;
        CHECK(Localise("Common_Yes") == "Yes");
        CHECK(Localise("Common_No") == "No");
    }

    SUBCASE("Standard Retrieval - Japanese") {
        Config::Language = ELanguage::Japanese;
        CHECK(Localise("Common_Yes") == "はい");
        CHECK(Localise("Common_No") == "いいえ");
    }

    SUBCASE("Fallback to English") {
        std::string key = "Test_OnlyEnglish";
        g_locale[key] = { { ELanguage::English, "FallbackValue" } };

        Config::Language = ELanguage::German;
        CHECK(Localise(key) == "FallbackValue");

        g_locale.erase(key);
    }

    SUBCASE("Missing Key") {
        CHECK(Localise("NonExistentKey_12345") == g_localeMissing);
    }

    SUBCASE("Missing Language and Missing English") {
        std::string key = "Test_OnlyFrench";
        g_locale[key] = { { ELanguage::French, "Oui" } };

        Config::Language = ELanguage::German;
        CHECK(Localise(key) == g_localeMissing);

        g_locale.erase(key);
    }

    SUBCASE("Correct Language when English exists") {
        std::string key = "Test_Both";
        g_locale[key] = {
            { ELanguage::English, "EnglishVal" },
            { ELanguage::German, "GermanVal" }
        };

        Config::Language = ELanguage::German;
        CHECK(Localise(key) == "GermanVal");

        Config::Language = ELanguage::English;
        CHECK(Localise(key) == "EnglishVal");

        g_locale.erase(key);
    }

    Config::Language = originalLanguage;
}
