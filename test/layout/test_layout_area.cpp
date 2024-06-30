#include <doctest/doctest.h>

#include "../../src/layout/layout_area.hpp"

namespace jianhan::v0::layout::tests {

TEST_SUITE("Test layout::Area") {

using namespace toml::literals::toml_literals;

TEST_CASE("test layout::Area(toml) construction") {
    const auto config = u8R"(
        val = ["A", "B", "C", "D"]
        pos = [0, 1, 2, 3]
    )"_toml;
    REQUIRE_NOTHROW((Area(config)));
}

static const auto QWERTY = Layout(""
    "QWERTYUIOP"
    "ASDFGHJKL;"
    "ZXCVBNM,./"
);
static const auto CONFIG = u8R"(
    val = ["Q", "W", "E"]
    pos = [0, 1, 2]
)"_toml;
static auto area = Area(CONFIG);
static auto prng = Prng();

TEST_CASE("test layout::Area::assign()") {

    // For an area with 3 keys Q, W and E, there are 6 possible outputs of
    // area.assign(), and only one of them is exactly the same as QWERTY.
    // Since the probability of the occurring of each layout is equal,
    // Pr(area.assign() == QWERTY) should be 1/6.

    static constexpr uz EXPECTION = 100;
    static constexpr uz TOLERANCE = 20;

    Layout layout(QWERTY);
    uz counter = 0;

    for (uz i = 0; i < EXPECTION * 6; ++i) {
        area.assign(layout, prng);
        if (layout == QWERTY) {
            ++counter;
        }
    }

    CHECK_GT(counter, EXPECTION - TOLERANCE);
    CHECK_LT(counter, EXPECTION + TOLERANCE);
}

static auto munOfDiffKeys(const Layout &lyt_1, const Layout &lyt_2) -> uz {
    uz counter = 0;
    for (auto [ch1, ch2]
         : std::views::zip(lyt_1.toStr(), lyt_2.toStr())) {
        if (ch1 != ch2) {
            ++counter;
        }
    }
    return counter;
}

TEST_CASE("test layout::Area::mutate()") {
    Layout layout(QWERTY);
    area.mutate(layout, prng);
    CHECK_EQ(munOfDiffKeys(layout, QWERTY), 2);
}

}

}
