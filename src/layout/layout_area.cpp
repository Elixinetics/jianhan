#include "layout_area.hpp"

namespace jianhan::v0::layout {

Area::Area(const toml_t &config)
    : Area(config.at("val_set").size()) {
    for (const toml_t &val : config.at("val_set").as_array()) {
        key_codes_.emplace_back(val.as_string().str[0]);
    }
    for (const toml_t &pos : config.at("pos_set").as_array()) {
        positions_.emplace_back(pos.as_integer());
    }
    // for comparisons
    std::ranges::sort(key_codes_);
}

Area::Area(const uz size)
    : size_(size), lim_(size - size % 2), idx_(lim_ + 1) {
    assert(size <= KEY_COUNT);
    key_codes_.reserve(size);
    positions_.reserve(size);
}

/**
 * @brief Randomly assign all the keys in the area.
 * @param layout: target layout.
 * @param prng: random number generator.
 * @note This operation can break the object.
 **/
auto Area::assign(Layout &layout, Prng &prng) noexcept -> void {
    std::ranges::shuffle(positions_, prng);
    for (const auto [val, pos] : std::views
         ::zip(key_codes_, positions_)) {
        layout.setKey(val, pos);
    }
}

/**
 * @brief Randomly swap two keys in the area.
 * @param layout: target layout.
 * @param prng: random number generator.
 * @note This operation can break the object.
 **/
auto Area::mutate(Layout &layout, Prng &prng) noexcept -> void {
    // When all the positions have been visited,
    // reshuffle pos_set_ and reset idx_ to 0.
    if (idx_ >= lim_) { reset(prng); }

    // Select two positions in the area, then
    // exchange the corresponding key values.
    const Position pos1 = positions_[idx_++];
    const Position pos2 = positions_[idx_++];
    layout.swapKeyValues(pos1, pos2);
}

auto Area::reset(Prng &prng) noexcept -> void {
    std::ranges::shuffle(positions_, prng);
    idx_ = 0;
}

auto Area::isCompatible(const Layout &layout) const noexcept -> bool {
    // If the key values in a layout (at the same positions) match
    // the key values of the current area (regardless of order),
    // then this area is compatible with the layout.
    std::vector<KeyValue> observed_key_values;
    for (const Position pos : positions_) {
        observed_key_values.emplace_back(
            layout.getVal(pos)
        );
    }
    // Sort to wipe out the effects of element order.
    std::ranges::stable_sort(observed_key_values);
    return observed_key_values == key_codes_;
}

}
