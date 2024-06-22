#ifndef JIANHAN_ROMU_TRIO_HPP
#define JIANHAN_ROMU_TRIO_HPP

#include <random>

namespace jianhan::v0 {

class SplitMix64 {
public:
    SplitMix64() = delete;

    explicit SplitMix64(const uint64_t seed): state_{seed} {}

    auto seed(const uint64_t seed) -> void {
        state_ = seed;
    }

    auto operator()() -> uint64_t {
        state_ += 0x9E3779B97F4A7C15ull;
        uint64_t z = state_; // transform state
        z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
        z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
        return z ^ (z >> 31);
    }

    auto warmup() -> void {
        for (size_t i = 0; i < 5; ++i) {
            operator()();
        }
    }

private:
    uint64_t state_{};
};

class RomuTrio64 {
public:
    using result_type = uint64_t;

    RomuTrio64() : RomuTrio64(42) {}

    explicit RomuTrio64(const uint64_t seed) {
        this->seed(seed), warmup();
    }

    auto seed(const uint64_t seed) -> void {
        // SplitMix64 is used to seed the PRNG, which is recommended
        // by the authors of RomuTrio: https://www.romu-random.org/
        static SplitMix64 initializer_(42);

        initializer_.seed(seed);
        initializer_.warmup();

        x_state_ = initializer_();
        y_state_ = initializer_();
        z_state_ = initializer_();
    }

    auto operator()() -> result_type {
        const uint64_t xp = x_state_, yp = y_state_, zp = z_state_;
        x_state_ = 15241094284759029579ull * zp; // update x, y, z
        y_state_ = yp - xp, y_state_ = std::rotl(y_state_, 12);
        z_state_ = zp - yp, z_state_ = std::rotl(z_state_, 44);
        return xp;
    }

    [[maybe_unused]] static constexpr auto min() -> result_type {
        return std::numeric_limits<result_type>::min();
    }

    [[maybe_unused]] static constexpr auto max() -> result_type {
        return std::numeric_limits<result_type>::max();
    }

private:
    uint64_t x_state_{};
    uint64_t y_state_{};
    uint64_t z_state_{};

    auto warmup() -> void {
        for (size_t i = 0; i < 11; ++i) {
            operator()();
        }
    }
};

}

#endif // JIANHAN_ROMU_TRIO_HPP
