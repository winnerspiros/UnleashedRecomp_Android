#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"
#include "ui/imgui_math.h"

TEST_CASE("Math Interpolation") {
    SUBCASE("Lerp (float)") {
        CHECK(Lerp(0.0f, 10.0f, 0.0f) == doctest::Approx(0.0f));
        CHECK(Lerp(0.0f, 10.0f, 0.5f) == doctest::Approx(5.0f));
        CHECK(Lerp(0.0f, 10.0f, 1.0f) == doctest::Approx(10.0f));
        CHECK(Lerp(0.0f, 10.0f, -0.5f) == doctest::Approx(-5.0f)); // Extrapolation
        CHECK(Lerp(10.0f, 0.0f, 0.25f) == doctest::Approx(7.5f));
    }

    SUBCASE("Cubic (float)") {
        // t^3 interpolation
        CHECK(Cubic(0.0f, 10.0f, 0.0f) == doctest::Approx(0.0f));
        CHECK(Cubic(0.0f, 10.0f, 0.5f) == doctest::Approx(10.0f * 0.125f)); // 1.25
        CHECK(Cubic(0.0f, 10.0f, 1.0f) == doctest::Approx(10.0f));
    }

    SUBCASE("Hermite (float)") {
        // Smoothstep-like: t * t * (3 - 2 * t)
        CHECK(Hermite(0.0f, 1.0f, 0.0f) == doctest::Approx(0.0f));
        CHECK(Hermite(0.0f, 1.0f, 0.5f) == doctest::Approx(0.5f)); // Symmetric at 0.5
        CHECK(Hermite(0.0f, 1.0f, 1.0f) == doctest::Approx(1.0f));

        // Check easing behavior
        // t=0.1 -> 0.01 * 2.8 = 0.028 (slower start than linear 0.1)
        CHECK(Hermite(0.0f, 1.0f, 0.1f) == doctest::Approx(0.028f));
    }

    SUBCASE("Lerp (ImVec2)") {
        ImVec2 a = {0.0f, 10.0f};
        ImVec2 b = {10.0f, 20.0f};
        ImVec2 res = Lerp(a, b, 0.5f);
        CHECK(res.x == doctest::Approx(5.0f));
        CHECK(res.y == doctest::Approx(15.0f));
    }

    SUBCASE("ColourLerp") {
        // Standard RGBA: 0x00000000 to 0xFFFFFFFF
        // Note: IM_COL32 layout depends on IMGUI_USE_BGRA_PACKED_COLOR,
        // but ColorConvertU32ToFloat4 handles it.

        ImU32 c1 = IM_COL32(0, 0, 0, 0);
        ImU32 c2 = IM_COL32(255, 255, 255, 255);

        ImU32 mid = ColourLerp(c1, c2, 0.5f);

        ImVec4 midVec = ImGui::ColorConvertU32ToFloat4(mid);
        CHECK(midVec.x == doctest::Approx(0.5f).epsilon(0.01f));
        CHECK(midVec.y == doctest::Approx(0.5f).epsilon(0.01f));
        CHECK(midVec.z == doctest::Approx(0.5f).epsilon(0.01f));
        CHECK(midVec.w == doctest::Approx(0.5f).epsilon(0.01f));
    }
}
