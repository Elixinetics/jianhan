#ifndef JIANHAN_LAYOUT_MANAGER_HPP
#define JIANHAN_LAYOUT_MANAGER_HPP

#include "layout_config.hpp"

namespace jianhan::v0::layout {

namespace default_config {

using namespace toml::literals::toml_literals;

static const auto toml = u8R"(
        [[mutable_area]]
        val = ["Z", "X", "C", "V"]
        pos = [20, 21, 22, 23]

        [[fixed_key]]
        val = ";"
        pos = 9

        [[fixed_key]]
        val = ","
        pos = 27

        [[fixed_key]]
        val = "."
        pos = 28

        [[fixed_key]]
        val = "/"
        pos = 29
    )"_toml;
}

class Manager final {
public:
    Manager();

    static auto loadConfig(const toml_t &config) -> void;

    auto create() noexcept -> Layout;
    auto reinit(Layout &layout) noexcept -> void;
    auto mutate(Layout &target, const Layout &parent) noexcept -> void;

    [[nodiscard]] auto canManage(const Layout &layout) const noexcept -> bool;

protected:
    std::vector<Area> mutable_areas_;
    std::vector<Key> fixed_keys_;
    std::vector<uz> area_ids_;

    Prng prng_{};

    const bool need_to_select_area_;
    const bool have_fixed_key_;

    const uz lim_;
    uz idx_;

    auto reset() noexcept -> void;
    auto randomlySelectAnArea() noexcept -> Area &;

private:
    inline static Config config_{default_config::toml};

    auto assignFixedKeys(Layout &layout) noexcept -> void;
    auto assignMutableKeys(Layout &layout) noexcept -> void;
};

}

#endif // JIANHAN_LAYOUT_MANAGER_HPP
