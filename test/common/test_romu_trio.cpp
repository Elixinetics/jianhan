#include <doctest/doctest.h>
#include <fmt/core.h>

#include "../../src/common/romu_trio.hpp"

namespace jianhan::v0 {

TEST_SUITE("Test RomuTrio") {

template<typename Prng> auto testPrng() -> bool {
    static constexpr int N_SAMPLES = 1'000;
    static constexpr int N_BINS = 10;

    Prng prng{std::random_device()()};
    std::uniform_real_distribution distribution(0.0, 1.0);

    static constexpr int expected_freq = N_SAMPLES / N_BINS;
    std::vector<int> observed_freq(N_BINS, 0);

    // Generate random numbers and count the
    // frequency of samples in each interval
    for (size_t i = 0; i < N_SAMPLES; ++i) {
        const double rand = distribution(prng);
        auto bin = static_cast<int>(rand * N_BINS);
        bin = bin < N_BINS ? bin : N_BINS - 1;
        ++observed_freq[bin];
    }

    // Calculate chi-square
    double chi_square = 0.0;
    for (const int freq : observed_freq) {
        const int diff = freq - expected_freq;
        chi_square += static_cast<double>(diff * diff) / expected_freq;
    }

    // when df = 9, alpha = 0.05, critical value = 16.919
    static constexpr double critical_value = 16.919;
    return chi_square < critical_value;
}


TEST_CASE("test the quality of random numbers") {
    static constexpr size_t TEST_ROUNDS = 50;
    static constexpr size_t MAX_FAILURES = 5;

    size_t num_failures = 0;
    for (size_t i = 0; i < TEST_ROUNDS; ++i) {
        if (not testPrng<RomuTrio64>()) {
            ++num_failures;
        }
    }

    CHECK_LT(num_failures, MAX_FAILURES);
}

}

}
