#ifndef JIANHAN_LAYOUT_AREA_HPP
#define JIANHAN_LAYOUT_AREA_HPP

#include "layout.hpp"

namespace jianhan::v0::layout {

using toml_t = toml::value;

class Config;

class Area final {
public:
    explicit Area(const toml_t &config);

    Area() = delete;

    auto assign(Layout &layout, Prng &prng) noexcept -> void;
    auto mutate(Layout &layout, Prng &prng) noexcept -> void;

    [[nodiscard]] auto isCompatible(const Layout &layout) const noexcept -> bool;

protected:
    std::vector<KeyValue> key_codes_{};
    std::vector<Position> positions_{};

    const uz size_;
    const uz lim_;
    uz idx_;

    auto reset(Prng &prng) noexcept -> void;

    auto addKeyValue(KeyValue val) -> void;
    auto addPosition(Position pos) -> void;

private:
    explicit Area(uz size);

    friend class Config;
};

}

#endif // JIANHAN_LAYOUT_AREA_HPP
