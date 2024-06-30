#include <doctest/doctest.h>
#include <nanobench.h>

#include "../../src/common/romu_trio.hpp"

namespace jianhan::v0::prng::tests {

TEST_SUITE("Bench RomuTrio64") {

    template<typename Prng> auto bench(
        ankerl::nanobench::Bench *const bench,
        const std::string_view name
    ) -> void {
        std::random_device rd;
        Prng prng(rd());
        bench->run(
            name.data(), [&]() -> void {
                uint64_t r = std::uniform_int_distribution<uint64_t>{}(prng);
                ankerl::nanobench::doNotOptimizeAway(r);
            }
        );
    }

    TEST_CASE("bench RomuTrio64") {
        ankerl::nanobench::Bench bs;
        bs.title("PRNGs (64bit)")
          .unit("random u64")
          .relative(true);
        bs.performanceCounters(true);

        bench<std::mt19937_64>(&bs, "std::mt19937"); // baseline
        bench<RomuTrio64>(&bs, "RomuTrio");
    }

}

}
