#include <doctest/doctest.h>

#include "../../src/common/utils.hpp"

namespace jianhan::v0::tests {

TEST_SUITE("Test Util") {

static constexpr std::string_view SUB_PATH{"sth/dummy.anytype"};

TEST_CASE("test Util::buildAbsPath(string)") {
    REQUIRE_NOTHROW(Util::mkAbsPath(SUB_PATH));
}

TEST_CASE("show Util::buildAbsPath(string)") {
    const std::string abs_path = Util::mkAbsPath(SUB_PATH);
    fmt::println(stderr, "\nRelative Path: {}", SUB_PATH);
    fmt::println(stderr, "Absolute Path: {}\n", abs_path);
}

}

}
