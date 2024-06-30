#include "layout_manager.hpp"

namespace jianhan::v0::layout {

Manager::Manager()
    : mutable_areas_(config_.mutable_areas_),
      fixed_keys_(config_.fixed_keys_),
      area_ids_(config_.area_ids_),
      need_to_select_area_(config_.num_areas_ > 1),
      have_fixed_key_(config_.num_fixed_keys_ > 0),
      lim_(config_.num_mutable_keys_), idx_(lim_ + 1) {}

auto Manager::loadConfig(const toml_t &config) -> void {
    config_ = Config(config);
}

auto Manager::create() noexcept -> Layout {
    Layout layout;
    assignFixedKeys(layout);
    assignMutableKeys(layout);
    assert(layout.valid());
    return layout;
}

auto Manager::assignFixedKeys(Layout &layout) noexcept -> void {
    if (not have_fixed_key_) { return; }
    for (const auto &[val, pos] : fixed_keys_) {
        layout.setPosValPair(val, pos);
    }
}

auto Manager::assignMutableKeys(Layout &layout) noexcept -> void {
    for (Area &area : mutable_areas_) {
        area.assign(layout, prng_);
    }
}

/**
 * @brief Shuffle all the keys in each mutable area.
 * @param layout: target layout.
 * @note - The target layout should be valid and manageable.
 *         Otherwise, the layout after mutation may not be valid.
 * @note - This function may cause undefined behavior if target layout
 *         is not valid or unmanagable, for the argument is not checked.
 **/
auto Manager::reinit(Layout &layout) noexcept -> void {
    assert(layout.valid() and canManage(layout));
    assignMutableKeys(layout);
    assert(layout.valid());
}

/**
 * @brief Slightly modify the layout based on a parent layout.
 * @param target: target layout.
 * @param parent: parent layout.
 * @note - The target layout can be invalid or unmanagable.
 * @note - The parent layout should be valid and manageable.
 *         Otherwise, the target after mutation may not be valid.
 * @note - This function may cause undefined behavior if parent layout
 *         is not valid or unmanagable, for the arguments are not checked.
 **/
auto Manager::mutate(Layout &target, const Layout &parent) noexcept -> void {
    assert(parent.valid() and canManage(parent));
    target.key_mappings_ = parent.key_mappings_;
    Area &rand_area = randomlySelectAnArea();
    rand_area.mutate(target, prng_);
    assert(target.valid());
}

auto Manager::randomlySelectAnArea() noexcept -> Area & {
    if (not need_to_select_area_) {
        return mutable_areas_[0];
    }

    // When all the area ids have been visited,
    // reshuffle area_ids_ and reset idx_ to 0.
    if (idx_ >= lim_) { reset(); }

    // Randomly select an area based on a random ID.
    // The probability of each area being selected
    // is proportional to its size, which ensures
    // the randomness of the seleciton of keys.
    const uz random_id = area_ids_[idx_++];
    return mutable_areas_[random_id];
}

auto Manager::reset() noexcept -> void {
    std::ranges::shuffle(area_ids_, prng_);
    idx_ = 0;
}

auto Manager::canManage(const Layout &layout) const noexcept -> bool {
    // Check if the layout is compatible with the manager's configuration.
    return std::ranges::all_of(mutable_areas_, [&layout](const Area &area) -> bool {
        return area.isCompatible(layout);
    });
}

}
