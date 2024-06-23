#ifndef JIANHAN_LAYOUT_HPP
#define JIANHAN_LAYOUT_HPP

#include "../common/utils.hpp"

namespace jianhan::v0 {

namespace layout {
class Manager;
class Area;
}

struct Key final {
    KeyValue val;
    Position pos;

    Key() = delete;
    Key(const KeyValue val, const Position pos): val(val), pos(pos) {}
};

class Layout {
public:
    explicit Layout(std::string_view str);
    virtual ~Layout() = default;

    [[nodiscard]] auto getVal(Position pos) const noexcept -> KeyValue;
    [[nodiscard]] auto getPos(KeyValue val) const noexcept -> Position;

    [[nodiscard]] auto toStr() const noexcept -> std::string;
    [[nodiscard]] auto valid() const noexcept -> bool;

    auto operator<=>(const Layout &other) const noexcept -> std::weak_ordering;
    auto operator==(const Layout &other) const noexcept -> bool;

protected:
    // Key values and positions are stored in a single array
    // since they do not conflict while being indexed:
    // Position 0 <= 29; KeyValue 44 <= 90.
    std::array<u8, MAX_KEY_CODE> items_{};

    Layout();

    auto loadFromString(std::string_view str) -> void;

    auto setKey(KeyValue val, Position pos) noexcept -> void;
    auto swapAPairOfKeys(Position pos1, Position pos2) noexcept -> void;

private:
    std::span<u8> keys_{items_.begin(), items_.begin() + KEY_COUNT};

    static auto varifyLayoutString(std::string_view str) -> void;
    static auto checkStringLength(std::string_view str) -> void;
    static auto checkCharLegalty(std::string_view str) -> void;
    static auto checkUniqueness(std::string_view str) -> void;

    [[nodiscard]] auto arekeysLegal() const noexcept -> bool;
    [[nodiscard]] auto arekeysUnique() const noexcept -> bool;

    class IllegalLytStr final : public FatalError {
    public:
        IllegalLytStr(
            const std::string_view seq,
            const std::string_view msg) noexcept
            : FatalError(fmt::format(WHAT, seq, msg)) {}

    private:
        static constexpr auto WHAT = "illegal layout \"{:s}\": {:s}";
    };

    friend class layout::Manager;
    friend class layout::Area;
};

} // namespace jianhan::v0

#endif // JIANHAN_LAYOUT_HPP
