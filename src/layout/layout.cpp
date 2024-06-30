#include "layout.hpp"

#include <ranges>

namespace jianhan::v0 {

/**
 * @brief Should not be used to create anything other than a temporary object.
 * @note Empty layout should not be used as a parameter for any function.
 **/
Layout::Layout() = default;

/**
 * @brief Construct a layout from a string that gives the order of the keys.
 * @param str Layout string, e.g."QWERTYUIOPASDFGHJKL;ZXCVBNM,./".
 */
Layout::Layout(const std::string_view str) {
    varifyLayoutString(str);
    loadFromString(str);
}

auto Layout::varifyLayoutString(const std::string_view str) -> void {
    // Check string length
    if (const uz len = str.length(); len != KEY_COUNT) {
        constexpr auto what{"incorrect length: expected {:d}, got {:d}"};
        throw IllegalString(str, fmt::format(what, KEY_COUNT, len));
    }

    // Check characters
    for (const char ch : str) {
        if (const auto v = static_cast<KeyValue>(ch); not Util::isKeyValueLegal(v)) {
            constexpr std::string_view what = "invalid key value: '{:c}'";
            throw IllegalString(str, fmt::format(what, ch));
        }
    }

    // Check for duplicates
    std::bitset<MAX_KEY_CODE> existing_vals;
    for (const char ch : str) {
        if (const auto val = static_cast<KeyValue>(ch); existing_vals[val]) {
            constexpr std::string_view what = "duplicate key values: '{:c}'";
            throw IllegalString(str, fmt::format(what, ch));
        } else [[likely]] {
            existing_vals.set(val);
        }
    }
}

void Layout::loadFromString(const std::string_view str) {
    for (const auto [i, ch] : str | std::views::enumerate) {
        const auto val = static_cast<KeyValue>(ch);
        const auto pos = static_cast<Position>(i);
        setPosValPair(val, pos);
    }
}

auto Layout::getVal(const Position pos) const noexcept -> KeyValue {
    assert(Util::isPositionLegal(pos));
    return key_mappings_[pos];
}

auto Layout::getPos(const KeyValue val) const noexcept -> Position {
    assert(Util::isKeyValueLegal(val));
    return key_mappings_[val];
}

auto Layout::toStr() const noexcept -> std::string {
    auto key_vals = key_mappings_ | std::views::take(KEY_COUNT);
    return {key_vals.begin(), key_vals.end()};
}

auto Layout::setPosValPair(const KeyValue val, const Position pos) noexcept -> void {
    assert(Util::isKeyValueLegal(val));
    assert(Util::isPositionLegal(pos));
    key_mappings_[val] = pos;
    key_mappings_[pos] = val;
}

auto Layout::swapKeyValues(const Position pos1, const Position pos2) noexcept -> void {
    assert(Util::isPositionLegal(pos1));
    assert(Util::isPositionLegal(pos2));
    std::swap(key_mappings_[pos1], key_mappings_[pos2]);
    std::swap(key_mappings_[getVal(pos1)], key_mappings_[getVal(pos2)]);
}

auto Layout::operator<=>(const Layout &other) const noexcept -> std::weak_ordering {
    for (const auto [this_val, other_val] : std::views::zip(
             this->key_mappings_ | std::views::take(KEY_COUNT),
             other.key_mappings_ | std::views::take(KEY_COUNT))) {
        if (this_val < other_val) { return std::weak_ordering::less; }
        if (this_val > other_val) { return std::weak_ordering::greater; }
    }
    return std::weak_ordering::equivalent;
}

auto Layout::operator==(const Layout &other) const noexcept -> bool {
    return this->key_mappings_ == other.key_mappings_;
}

auto Layout::valid() const noexcept -> bool {
    return arekeysLegal() and arekeysUnique();
}

auto Layout::arekeysLegal() const noexcept -> bool {
    return std::ranges::all_of( // Check key codes
               POSITIONS, [this](const auto pos) -> bool {
                   const KeyValue val = this->getVal(pos);
                   return Util::isKeyValueLegal(val);
               }
           )
           and
           std::ranges::all_of( // Check positions
               KEY_CODES, [this](const auto val) -> bool {
                   const Position pos = this->getPos(val);
                   return Util::isPositionLegal(pos);
               }
           );
}

auto Layout::arekeysUnique() const noexcept -> bool {
    std::bitset<MAX_KEY_CODE> already_observed(0);

    // Check key code uniqueness
    for (const Position pos : POSITIONS) {
        const KeyValue val = getVal(pos);
        if (already_observed[val]) {
            return false;
        }
        // mark as observed
        already_observed.set(val);
    }

    // Check position uniqueness
    for (const KeyValue val : KEY_CODES) {
        const Position pos = getPos(val);
        if (already_observed[pos]) {
            return false;
        }
        // mark as observed
        already_observed.set(pos);
    }

    return true;
}

} // namespace jianhan::v0
