#ifndef JIANHAN_LAYOUT_CONFIG_HPP
#define JIANHAN_LAYOUT_CONFIG_HPP

#include "layout_area.hpp"

namespace jianhan::v0::layout {

class Manager;

class Config final {
public:
    explicit Config(const toml_t &config);

    Config() = delete;

protected:
    std::vector<Area> mutable_areas_{};
    std::vector<Key> fixed_keys_{};
    std::vector<uz> area_ids_{};

    uz num_mutable_keys_{};
    uz num_fixed_keys_{};
    uz num_areas_{};

    auto validateConfig(const toml_t &config) -> void;
    auto loadFixedKeys(const toml_t &config) -> void;
    auto loadAreas(const toml_t &config) -> void;
    auto makeDefatulArea() -> void;

private:
    std::array<const toml_t *, MAX_KEY_CODE> where_{nullptr};
    uz num_unprocessed_keys_{};

    static auto checkIfAllRequiredFieldsExist(const toml_t &config) -> void;
    static auto checkFieldSizes(const toml_t &area_config) -> void;

    auto checkKeyValues(const toml_t &area_config) -> void;
    auto checkPositions(const toml_t &area_config) -> void;

    auto checkKeyValue(const toml_t &val) -> void;
    auto checkPosition(const toml_t &pos) -> void;

    [[nodiscard]] auto isValProcessed(KeyValue val) const -> bool;
    [[nodiscard]] auto isPosProcessed(Position pos) const -> bool;

    class IllegalSetting final : public std::invalid_argument {
    public:
        IllegalSetting() = delete;
        explicit IllegalSetting(const std::string_view msg) noexcept
            : invalid_argument(format(msg)) {}

    private:
        static constexpr auto WHAT{"invalid layout configuration: {:s}"};

        static auto format(const std::string_view msg) -> std::string {
            if (msg.starts_with("[error] ")) { // toml11 prefix
                return fmt::format(WHAT, msg.substr(8));
            }
            return fmt::format(WHAT, msg);
        }
    };

    friend class Manager;
};

}

#endif // JIANHAN_LAYOUT_CONFIG_HPP
