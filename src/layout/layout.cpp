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

void Layout::loadFromString(const std::string_view str) {
    for (const auto [i, ch] : str | std::views::enumerate) {
        const auto val = static_cast<KeyValue>(ch);
        const auto pos = static_cast<Position>(i);
        setKey(val, pos);
    }
}

auto Layout::varifyLayoutString(const std::string_view str) -> void {
    checkStringLength(str);
    checkCharLegalty(str);
    checkUniqueness(str);
}

auto Layout::checkStringLength(const std::string_view str) -> void {
    if (str.length() != KEY_COUNT) {
        constexpr std::string_view what = "invalid sequence length: {:d}";
        throw IllegalLytStr(str, fmt::format(what, str.length()));
    }
}

auto Layout::checkCharLegalty(const std::string_view str) -> void {
    for (const char ch : str) {
        if (const auto v = static_cast<KeyValue>(ch); not Util::isKeyValueLegal(v)) {
            constexpr std::string_view what = "invalid key val: '{:c}'";
            throw IllegalLytStr(str, fmt::format(what, ch));
        }
    }
}

auto Layout::checkUniqueness(const std::string_view str) -> void {
    std::bitset<MAX_KEY_CODE> existing_vals;
    for (const char ch : str) {
        if (const auto val = static_cast<KeyValue>(ch); existing_vals[val]) {
            constexpr std::string_view what = "duplicate key val: '{:c}'";
            throw IllegalLytStr(str, fmt::format(what, ch));
        } else [[likely]] {
            existing_vals.set(val);
        }
    }
}

auto Layout::getVal(const Position pos) const noexcept -> KeyValue {
    assert(Util::isPositionLegal(pos));
    return items_[pos];
}

auto Layout::getPos(const KeyValue val) const noexcept -> Position {
    assert(Util::isKeyValueLegal(val));
    return items_[val];
}

auto Layout::toStr() const noexcept -> std::string {
    return {keys_.begin(), keys_.end()};
}

auto Layout::setKey(const KeyValue val, const Position pos) noexcept -> void {
    assert(Util::isKeyValueLegal(val));
    assert(Util::isPositionLegal(pos));
    items_[val] = pos;
    items_[pos] = val;
}

auto Layout::swapKeyValues(const Position pos1, const Position pos2) noexcept -> void {
    assert(Util::isPositionLegal(pos1));
    assert(Util::isPositionLegal(pos2));
    const KeyValue val1 = getVal(pos1);
    const KeyValue val2 = getVal(pos2);
    std::swap(items_[val1], items_[val2]);
    std::swap(items_[pos1], items_[pos2]);
}

auto Layout::operator<=>(const Layout &other) const noexcept -> std::weak_ordering {
    for (const auto [this_val, other_val] : std::views::zip(keys_, other.keys_)) {
        if (this_val > other_val) { return std::weak_ordering::greater; }
        if (this_val < other_val) { return std::weak_ordering::less; }
    }
    return std::weak_ordering::equivalent;
}

auto Layout::operator==(const Layout &other) const noexcept -> bool {
    return this->items_ == other.items_;
}

auto Layout::valid() const noexcept -> bool {
    return arekeysLegal() and arekeysUnique();
}

auto Layout::arekeysLegal() const noexcept -> bool {
    const bool all_key_codes_legal = std::ranges::all_of(
        POSITIONS, [this](const auto pos) -> bool {
            const KeyValue val = this->getVal(pos);
            return Util::isKeyValueLegal(val);
        }
    );
    const bool all_positions_legal = std::ranges::all_of(
        KEY_CODES, [this](const auto val) -> bool {
            const Position pos = this->getPos(val);
            return Util::isPositionLegal(pos);
        }
    );
    return all_key_codes_legal and all_positions_legal;
}

auto Layout::arekeysUnique() const noexcept -> bool {
    std::bitset<MAX_KEY_CODE> already_observed(0);

    // Check for duplicate key codes
    for (const Position pos : POSITIONS) {
        const KeyValue val = getVal(pos);
        if (already_observed[val]) {
            return false;
        }
        // mark as observed
        already_observed.set(val);
    }

    // Check for duplicate positions
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
