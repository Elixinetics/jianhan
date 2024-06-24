#ifndef JIANHAN_LAYOUT_AREA_HPP
#define JIANHAN_LAYOUT_AREA_HPP

#include "layout.hpp"

namespace jianhan::v0::layout {

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

private:
    explicit Area(uz size);

    auto reset(Prng &prng) noexcept -> void;

    friend class Config;
};

}

#endif // JIANHAN_LAYOUT_AREA_HPP
