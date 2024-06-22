#ifndef JIANHAN_TYPES_HPP
#define JIANHAN_TYPES_HPP

#include <cstdint>
#include <stdfloat>
#include <exception>

#include <fmt/core.h>
#include <toml.hpp>

#include "romu_trio.hpp"

namespace jianhan::v0 {

using bf16 = std::bfloat16_t;
using fpz = std::float32_t;
using u8 = uint_fast8_t;
using uz = size_t;

using KeyValue = uint8_t; // 键值（键码）, ∈ [65, 90] ∪ {44, 46, 47, 59}
using Position = u8; // 键位（编号）, ∈ [0, 29]
using Col = u8; // 列, ∈ [0, 9]
using Row = u8; // 行, ∈ [0, 2]

using Prng = RomuTrio64;

using toml_t = toml::value;

class FatalError : public std::exception {
public:
    explicit FatalError(const std::string_view msg) noexcept: msg_(msg) {}

    [[nodiscard]] const char *what() const noexcept override {
        return msg_.c_str();
    }

private:
    std::string msg_;
};

}

#endif // JIANHAN_TYPES_HPP
