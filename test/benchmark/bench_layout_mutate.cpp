#include <omp.h>
#include <thread>

#include <nanobench.h>
#include <doctest/doctest.h>

#include "../../src/layout/layout_manager.hpp"

static constexpr size_t NUM_LAYOUTS = 1000;
static constexpr size_t MAX_THREADS = 4;

namespace jianhan::v0::layout::bench::mutate {

TEST_SUITE("Bench layout::Manager::mutate()") {

using Layouts = std::vector<std::unique_ptr<Layout>>;

auto checkRandomness(Layouts &l) -> void {
    std::ranges::sort(l);
    uz num_duplicates = 0;
    for (uz i = 0; i < l.size() - 1; ++i) {
        if (l[i] == l[i + 1]) {
            ++num_duplicates;
        }
    }
    CHECK_LT(num_duplicates, 5);
}

auto createLayouts(Manager &manager) -> Layouts {
    Layouts layouts;
    for (uz i = 0; i < NUM_LAYOUTS * 2; ++i) {
        const Layout lyt(manager.create());
        layouts.emplace_back(std::make_unique<Layout>(lyt));
    }
    return layouts;
}

auto baseline(ankerl::nanobench::Bench *const bench) -> void {
    Manager manager;
    Layouts layouts = createLayouts(manager);

    bench->run(
        "baseline (1)",
        [&]() -> void {
            for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                manager.mutate(*layouts[i], *layouts[i + NUM_LAYOUTS]);
            }
        }
    );

    checkRandomness(layouts);
}

auto benchOmpStatic(ankerl::nanobench::Bench *const bench, const uz num_threads) -> void {
    Manager manager;
    Layouts layouts = createLayouts(manager);

    bench->run(
        fmt::format("omp: static ({:d})", num_threads).c_str(),
        [&]() -> void {
#pragma omp parallel for schedule(static) shared(layouts) private(manager) default (none)
            for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                manager.mutate(*layouts[i], *layouts[i + NUM_LAYOUTS]);
            }
        }
    );

    checkRandomness(layouts);
}

auto benchOmpGuided(ankerl::nanobench::Bench *const bench, const uz num_threads) -> void {
    Manager manager;
    Layouts layouts = createLayouts(manager);

    bench->run(
        fmt::format("omp: guided ({:d})", num_threads).c_str(),
        [&]() -> void {
#pragma omp parallel for schedule(guided) shared(layouts) private(manager) default (none)
            for (uz i = 0; i < NUM_LAYOUTS; ++i) {
                manager.mutate(*layouts[i], *layouts[i + NUM_LAYOUTS]);
            }
        }
    );

    checkRandomness(layouts);
}

auto processRange(Manager &manager, const Layouts &layouts, const uz beg, const uz end) -> void {
    for (uz i = beg; i < end; ++i) {
        manager.mutate(*layouts[i], *layouts[i + NUM_LAYOUTS]);
    }
}

auto benchStdThread(ankerl::nanobench::Bench *const bench, const uz num_threads) -> void {
    std::vector managers(num_threads, Manager());
    Layouts layouts = createLayouts(managers[0]);

    const uz range = NUM_LAYOUTS / num_threads;
    bench->run(
        fmt::format("std::thread ({:d})", num_threads).c_str(),
        [&]() -> void {
            std::vector<std::thread> threads{};
            for (uz thread_id = 0; thread_id < num_threads; ++thread_id) {
                const uz beg = thread_id * range;
                const uz end = (thread_id == num_threads - 1) ? NUM_LAYOUTS : beg + range;
                threads.emplace_back(processRange, std::ref(managers[thread_id]), std::ref(layouts), beg, end);
            }
            for (auto &t : threads) {
                t.join();
            }
            threads.clear();
        }
    );

    checkRandomness(layouts);
}

TEST_CASE("bench layout::Manager::mutate()") {
    REQUIRE_LE(MAX_THREADS, std::thread::hardware_concurrency());
    REQUIRE_GE(MAX_THREADS, 1);

    ankerl::nanobench::Bench bench;
    bench.title("mutate()")
         .unit("epoch")
         .warmup(10)
         .relative(true)
         .minEpochIterations(600);
    bench.performanceCounters(true);

    baseline(&bench);
    for (uz i = 2; i <= MAX_THREADS; ++i) {
        omp_set_num_threads(static_cast<int>(i));
        benchOmpStatic(&bench, i);
        benchOmpGuided(&bench, i);
        //BenchStdThread(&bench, i);
    }
}

}

}
