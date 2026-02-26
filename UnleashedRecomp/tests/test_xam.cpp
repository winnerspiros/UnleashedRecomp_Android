#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include <kernel/xam.h>
#include <string>

// Forward declarations if needed, or rely on xam.h

TEST_CASE("Xam Path Management") {
    // Ensure clean state if possible, but gRootMap is global.
    // We can use unique root names for tests.

    SUBCASE("XamRootCreate and XamGetRootPath Basic") {
        std::string rootName = "TestRoot1";
        std::string rootPath = "C:\\Test\\Path";

        XamRootCreate(rootName, rootPath);

        std::string_view retrievedPath = XamGetRootPath(rootName);
        CHECK(retrievedPath == rootPath);
    }

    SUBCASE("XamGetRootPath Non-Existent") {
        std::string rootName = "NonExistentRoot";
        std::string_view retrievedPath = XamGetRootPath(rootName);
        CHECK(retrievedPath == "");
    }

    SUBCASE("XamGetRootPath Empty Root") {
        std::string rootName = "";
        std::string_view retrievedPath = XamGetRootPath(rootName);
        CHECK(retrievedPath == ""); // Unless empty root is explicitly registered, which usually isn't
    }

    // Assuming StringHash might be case-sensitive or not depending on implementation.
    // If it's XXH3_64bits on the string bytes, it IS case sensitive.
    SUBCASE("XamGetRootPath Case Sensitivity") {
        std::string rootName = "CaseSensitiveRoot";
        std::string rootPath = "Path";
        XamRootCreate(rootName, rootPath);

        std::string_view lowerRetrieved = XamGetRootPath("casesensitiveroot");
        // UnleashedRecomp framework.h StringHash uses XXH3_64bits directly on str.data()
        // So it should be case sensitive.
        CHECK(lowerRetrieved == "");
    }

    SUBCASE("Overwriting Root") {
        std::string rootName = "OverwriteRoot";
        std::string path1 = "Path1";
        std::string path2 = "Path2";

        XamRootCreate(rootName, path1);
        CHECK(XamGetRootPath(rootName) == path1);

        // gRootMap is xxHashMap (unordered_dense::map). emplace doesn't replace if key exists.
        // Let's verify behavior. If it uses emplace, it won't update.
        // Code: gRootMap.emplace(StringHash(root), path);
        XamRootCreate(rootName, path2);
        CHECK(XamGetRootPath(rootName) == path1); // Expecting it NOT to update based on code reading
    }
}
