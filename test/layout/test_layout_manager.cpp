#include <doctest/doctest.h>

#include "../../src/layout/layout_manager.hpp"

namespace jianhan::v0::layout::tests {

TEST_SUITE("Test layout::Manager") {

using namespace toml::literals::toml_literals;

TEST_CASE("test layout::Manager() construction") {

    SUBCASE("TOML literial") {
        REQUIRE_NOTHROW(Manager());
    }

    SUBCASE("TOML file") {
        const std::string CFG_PATH = Util::mkAbsPath("conf/layouts.toml");
        Manager::loadConfig(toml::parse(CFG_PATH));
        REQUIRE_NOTHROW((Manager()));
    }

}

static Manager manager;

TEST_CASE("test layout::Manager::createLayout()") {

    // May fail due to too few variable keys

    static constexpr uz SAMPLES = 1'000;
    static constexpr uz MAX_DUP = 10;

    std::vector<std::string> layouts;
    for (uz i = 0; i < SAMPLES; i++) {
        const Layout l = manager.create();
        layouts.emplace_back(l.toStr());
    }

    std::ranges::sort(layouts);
    uz num_duplicate_items = 0;
    for (uz i = 0; i < layouts.size() - 1; ++i) {
        if (layouts[i] == layouts[i + 1]) {
            ++num_duplicate_items;
        }
    }

    CHECK_LT(num_duplicate_items, MAX_DUP);
}

TEST_CASE("show layout::Manager::createLayout()") {
    fmt::println("\nCreate 5 different layouts:");
    for (uz i = 1; i <= 5; i++) {
        Layout layout = manager.create();
        fmt::print("{:d}. {:s}\n", i, layout.toStr());
    }
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

const Layout L = manager.create();

TEST_CASE("test layout::Manager::reinit()") {
    SUBCASE("compare with previous") {
        Layout layout = L;
        REQUIRE(manager.canManage(L));

        manager.reinit(layout);
        REQUIRE_GT(munOfDiffKeys(L, layout), 0);
    }

    SUBCASE("verify randomness") {

        // May fail due to too few variable keys

        static constexpr uz SAMPLES = 1'000;
        static constexpr uz MAX_DUP = 10;

        std::vector<std::string> layouts;
        for (uz i = 0; i < SAMPLES; i++) {
            Layout l = L;
            manager.reinit(l);
            layouts.emplace_back(l.toStr());
        }

        std::ranges::sort(layouts);
        uz num_duplicate_items = 0;
        for (uz i = 0; i < layouts.size() - 1; ++i) {
            if (layouts[i] == layouts[i + 1]) {
                ++num_duplicate_items;
            }
        }

        CHECK_LT(num_duplicate_items, MAX_DUP);
    }
}

TEST_CASE("test layout::Manager::mutate()") {

    SUBCASE("compare with parent") {
        Layout layout = manager.create();

        REQUIRE(manager.canManage(L));
        manager.mutate(layout, L);
        REQUIRE_EQ(munOfDiffKeys(layout, L), 2);
    }

    SUBCASE("verify randomness") {

        // May fail due to too few variable areas

        static constexpr uz SAMPLES = 100;
        static constexpr uz MAX_DUP = 30;

        std::vector<std::string> layouts;
        for (uz i = 0; i < SAMPLES; i++) {
            Layout layout = manager.create();
            manager.mutate(layout, L);
            layouts.emplace_back(layout.toStr());
        }

        std::ranges::sort(layouts);
        uz num_duplicates = 0;
        for (uz i = 0; i < layouts.size() - 1; ++i) {
            if (layouts[i] == layouts[i + 1]) {
                ++num_duplicates;
            }
        }

        CHECK_LT(num_duplicates, MAX_DUP);
    }

}

static auto idxOfDiffKeys(const Layout &lyt_1, const Layout &lyt_2) -> std::vector<uz> {
    std::vector<uz> indexes{};
    for (uz i = 0; i < KEY_COUNT; ++i) {
        if (lyt_1.toStr()[i] != lyt_2.toStr()[i]) {
            indexes.emplace_back(i);
        }
    }
    return indexes;
}

TEST_CASE("show layout::Manager::mutate()") {
    fmt::println("\nMutate 5 different layouts:\n");
    Layout parent = manager.create();
    Layout layout = manager.create();
    for (uz i = 0; i < 5; i++) {
        manager.reinit(parent);
        manager.mutate(layout, parent);

        fmt::println(
            "{:d}.\nbefore: {:s}\nafter:  {:s}",
            i + 1, parent.toStr(), layout.toStr()
        );

        const auto indexes = idxOfDiffKeys(layout, parent);
        CHECK_EQ(indexes.size(), 2);

        fmt::println(
            "diff:   {:s}^{:s}^\n",
            std::string(indexes[0], ' '),
            std::string(indexes[1] - indexes[0] - 1, ' ')
        );
    }
}

TEST_CASE("test reinit() and mutate() errors") {

    // Generate a layout that conflicts with the manager.
    // May fail if the layout has only one mutable area or
    // such layout happens to satisfy the all requirements.

    auto key_vals{KEY_CODES};
    std::ranges::shuffle(key_vals, Prng(2024));
    const std::string str{key_vals.begin(), key_vals.end()};

    Layout unmanageable(str);
    REQUIRE(unmanageable.valid());
    REQUIRE_FALSE(manager.canManage(unmanageable));

    // SUBCASE("reinitFast()") {
    //     // In debug mode, an assertion will be thrown.
    //     // In release mode, the program will not be interrupted,
    //     // but there is high probability that the result not being legal .
    //     manager.reinit(unmanageable);
    // }

    // SUBCASE("mutateFast()") {
    //     // In debug mode, an assertion will be thrown.
    //     // In release mode, the program will not be interrupted,
    //     // but there is high probability that the result not being legal .
    //     Layout l = manager.create();
    //     manager.mutate(l, unmanageable);
    // }

}

}

}
