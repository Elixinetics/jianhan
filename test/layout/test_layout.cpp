#include <doctest/doctest.h>

#include "../../src/layout/layout.hpp"

namespace jianhan::v0::tests {

TEST_SUITE("Test Layout") {

using Err = std::exception;

TEST_CASE("test Layout(string) construction") {

    SUBCASE("success") {
        REQUIRE_NOTHROW(Layout("QWERTYUIOPASDFGHJKL;ZXCVBNM,./")); // QWERTY
        REQUIRE_NOTHROW(Layout("/,.PYFGCRLAOEUIDHTNS;QJKXBMWVZ")); // Devorak
        REQUIRE_NOTHROW(Layout("QWFPGJLUY;ARSTDHNEIOZXCVBKM,./")); // Colemak
    }

    SUBCASE("fail") {
        // illegal string length (31)
        static const std::string s31 = "QWERTYUIOPASDFGHJKL;ZXCVBNM,.//";
        REQUIRE_THROWS_AS((Layout(s31)), Err);
        // illegal key value (qwe)
        static const std::string qwe = "qweRTYUIOPASDFGHJKL;ZXCVBNM,./";
        REQUIRE_THROWS_AS((Layout(qwe)), Err);
        // dupolicated keys (QQQ)
        static const std::string qqq = "QQQRTYUIOPASDFGHJKL;ZXCVBNM,./";
        REQUIRE_THROWS_AS((Layout(qqq)), Err);
    }
}

TEST_CASE("test Layout verification") {

    SUBCASE("valid") {
        REQUIRE(Layout("QWERTYUIOPASDFGHJKL;ZXCVBNM,./").valid());
        REQUIRE(Layout("/,.PYFGCRLAOEUIDHTNS;QJKXBMWVZ").valid());
        REQUIRE(Layout("QWFPGJLUY;ARSTDHNEIOZXCVBKM,./").valid());
    }

    class UnsafeLayout : public Layout {
    public:
        explicit UnsafeLayout(const std::string_view str) {
            loadFromString(str);
        }
    };

    // Note:
    // The following case only addresses the issues that may
    // arise during layout modification. Other cases are
    // covered by the tests before this one.
    SUBCASE("invlid - dupolicate keys") {
        UnsafeLayout l("QQERTYUIOPASDFGHJKL;ZXCVBNM,./");
        // fmt::println(stderr, "{}", l.toStr());
        REQUIRE_FALSE(l.valid());
    }
}

TEST_CASE("test Layout comparison") {

    SUBCASE("operator==") {
        Layout l1("QWERTYUIOPASDFGHJKL;ZXCVBNM,./"); // QW...
        Layout l2("QWERTYUIOPASDFGHJKL;ZXCVBNM,./"); // QW...
        Layout l3("WQERTYUIOPASDFGHJKL;ZXCVBNM,./"); // WQ...
        REQUIRE((l1 == l2 and l1 != l3));
    }

    SUBCASE("sort") {
        Layout l1("ABCDEFGHIJKLMNOPQRSTUVWXYZ,.;/"); // start with A
        Layout l2("BCDEFGHIJKLMNOPQRSTUVWXYZA,.;/"); // start with B
        Layout l3("CDEFGHIJKLMNOPQRSTUVWXYZAB,.;/"); // start with C
        std::vector layouts{l3, l2, l1};
        std::ranges::sort(layouts);
        CHECK_EQ(layouts[0], l1);
        CHECK_EQ(layouts[1], l2);
        CHECK_EQ(layouts[2], l3);
    }
}

}

}
