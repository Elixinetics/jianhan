#include <doctest/doctest.h>

#include "../../src/layout/layout_config.hpp"

namespace jianhan::v0::layout::tests {

// #define SHOW_ERR_MSG

TEST_SUITE("Test layout::Config") {

using namespace toml::literals::toml_literals;

TEST_CASE("test layout::Config(toml) construction") {

    SUBCASE("example") {
        const auto config = u8R"(
            [[mutable_area]]
            val = ["A", "E", "I", "O", "U", "N", "H", "T"]
            pos = [10, 11, 12, 13, 16, 17, 18, 19]

            [[mutable_area]]
            val = ["Z", "X", "C", "V"]
            pos = [20, 21, 22, 23]

            [[fixed_key]]
            val = ";"
            pos = 9

            [[fixed_key]]
            val = ","
            pos = 27

            [[fixed_key]]
            val = "."
            pos = 28

            [[fixed_key]]
            val = "/"
            pos = 29
        )"_toml;
        REQUIRE_NOTHROW((Config(config)));
    }

    SUBCASE("empty config") {
        // A toml file without any valid top-level keys is considered as valid.
        // In this case, all the keys will be automatically assigned
        // to a default mutable area.
        const auto config = u8R"(
            [useless]
            nothing = "here"
        )"_toml;
        REQUIRE_NOTHROW((Config(config)));
    }

    SUBCASE("full config") {
        // There is no need to explicitly specify all the keys in the config,
        // as missing keys will be assigned to a default mutable area.
        const auto config = u8R"(
            [[mutable_area]]
            val = ["B", "D", "F", "G", "J", "K", "L", "M", "P", "Q", "R", "S", "W", "Y"]
            pos = [0, 1, 2, 3, 4, 5, 6, 7, 8, 14, 15, 24, 25, 26]

            [[mutable_area]]
            val = ["A", "E", "I", "O", "U", "N", "H", "T"]
            pos = [10, 11, 12, 13, 16, 17, 18, 19]

            [[mutable_area]]
            val = ["Z", "X", "C", "V"]
            pos = [20, 21, 22, 23]

            [[fixed_key]]
            val = ";"
            pos = 9

            [[fixed_key]]
            val = ","
            pos = 27

            [[fixed_key]]
            val = "."
            pos = 28

            [[fixed_key]]
            val = "/"
            pos = 29
        )"_toml;
        REQUIRE_NOTHROW((Config(config)));
    }

}

auto check(const toml_t &cfg) -> void {
#ifdef SHOW_ERR_MSG
    try {
        Config c(cfg);
        REQUIRE(false); // should not reach here
    } catch (const std::exception &e) {
        fmt::println(stderr, "\n{}", e.what());
    } catch (...) {
        fmt::println(stderr, "\nunknown exception");
        REQUIRE(false); // should not reach here
    }
#else
    REQUIRE_THROWS_AS((Config(cfg)), std::exception);
#endif
}

TEST_CASE("test layout::Config(toml) error cases") {

    // Define the SHOW_ERR_MSG macroat the beginning of this file and recompile,
    // if you would like to see the error messages of the following test cases.

    SUBCASE("missing keys in [[mutable_area]]") {
        check({
            u8R"(
                [[mutable_area]]
                something_else = []
            )"_toml
        });
    }

    SUBCASE("missing keys in [[fixed_key]]") {
        check({
            u8R"(
                [[fixed_key]]
                val = "A"
                another_key = "useless"
            )"_toml
        });
    }

    SUBCASE("illegal type - arrays") {
        check({
            u8R"(
                [[mutable_area]]
                val = "A"
                pos = 0
            )"_toml
        });
    }

    SUBCASE("illegal type - key value") {
        check({
            u8R"(
                [[fixed_key]]
                val = 0
                pos = 0
        )"_toml
        });
    }

    SUBCASE("illegal type - position") {
        check({
            u8R"(
                [[fixed_key]]
                val = "Q"
                pos = [0]
            )"_toml
        });
    }

    SUBCASE("illegal area size - unequal array") {
        check({
            u8R"(
                [[mutable_area]]
                val = ["X", "Y", "Z"]
                pos = [0]
            )"_toml
        });
    }

    SUBCASE("illegal area size - out of range") {
        check({
            u8R"(
                [[mutable_area]]
                val = ["X"]
                pos = [0]
            )"_toml
        });
    }

    SUBCASE("illegal toml node - illegal string length") {
        check({
            u8R"(
                [[fixed_key]]
                val = "PYTHON"
                pos = 0
            )"_toml
        });
    }

    SUBCASE("illegal toml node - illegal character") {
        check({
            u8R"(
                [[fixed_key]]
                val = "?"
                pos = 6
            )"_toml
        });
    }

    SUBCASE("illegal toml node - pos out of range") {
        check({
            u8R"(
                [[fixed_key]]
                val = "A"
                pos = 42
            )"_toml
        });
    }

    SUBCASE("duplicate key values - within one area") {
        check({
            u8R"(
                [[mutable_area]]
                val = ["C", "P", "P"]
                pos = [0, 1, 2]
            )"_toml
        });
    }

    SUBCASE("duplicate key values - between areas") {
        check({
            u8R"(
                [[mutable_area]]
                val = ["A", "B", "C"]
                pos = [0, 1, 2]

                [[mutable_area]]
                val = ["C", "D", "E"]
                pos = [3, 4, 5]
            )"_toml
        });
    }

    SUBCASE("duplicate key values - between keys") {
        check({
            u8R"(
                [[fixed_key]]
                val = "I"
                pos = 0

                [[fixed_key]]
                val = "I"
                pos = 1
            )"_toml
        });
    }

    SUBCASE("duplicate key values - both") {
        check({
            u8R"(
                [[mutable_area]]
                val = ["X", "Y", "Z"]
                pos = [0, 1, 2]

                [[fixed_key]]
                val = "Z"
                pos = 3
            )"_toml
        });
    }

    SUBCASE("duplicate positions - within one area") {
        check({
            u8R"(
                [[mutable_area]]
                val = ["Q", "W", "E"]
                pos = [1, 1, 4]
            )"_toml
        });
    }

    SUBCASE("duplicate positions - between areas") {
        check({
            u8R"(
                [[mutable_area]]
                val = ["A", "B", "C"]
                pos = [3, 4, 5]

                [[mutable_area]]
                val = ["D", "E", "F"]
                pos = [1, 2, 3]
            )"_toml
        });
    }

    SUBCASE("duplicate positions - between keys") {
        check({
            u8R"(
                [[fixed_key]]
                val = "X"
                pos = 0

                [[fixed_key]]
                val = "Y"
                pos = 0
            )"_toml
        });
    }

    SUBCASE("duplicate positions - both") {
        check({
            u8R"(
                [[mutable_area]]
                val = ["Q", "W", "E"]
                pos = [0, 1, 2]

                [[fixed_key]]
                val = "R"
                pos = 0
            )"_toml
        });
    }

}

}

}
