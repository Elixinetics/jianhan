#include "layout_config.hpp"

namespace jianhan::v0::layout {

Config::Config(const toml_t &config) {
    validateConfig(config);
    loadFixedKeys(config);
    loadAreas(config);
    makeDefatulArea();
}

auto Config::validateConfig(const toml_t &config) -> void {
    static constexpr uz MIN_MUTABLE_KEYS = 2;

    where_.fill(nullptr);

    // Check fixed keys
    if (config.contains("fixed_key")) {
        const toml_t &key_settings = config.at("fixed_key");
        for (const toml_t &key : key_settings.as_array()) {
            checkIfAllRequiredFieldsExist(key);
            checkKeyValue(key.at("val"));
            checkPosition(key.at("pos"));
        }
        num_fixed_keys_ = key_settings.size();
    } else {
        num_fixed_keys_ = 0;
    }

    // Check number of mutable keys
    num_mutable_keys_ = KEY_COUNT - num_fixed_keys_;
    if (num_mutable_keys_ < MIN_MUTABLE_KEYS) {
        throw IllegalSetting(fmt::format(
            "unable to gengrate adequate samples:\n"
            "required at least 2 mutable keys, got {:d}",
            num_mutable_keys_
        ));
    }

    // Check mutable areas and owned keys
    if (config.contains("mutable_area")) {
        const toml_t &area_configs = config.at("mutable_area");
        for (const toml_t &area : area_configs.as_array()) {
            checkIfAllRequiredFieldsExist(area);
            checkFieldSizes(area);
            checkKeyValues(area);
            checkPositions(area);
        }
        num_areas_ = area_configs.size() + 1;
    } else {
        num_areas_ = 1;
    }
}

auto Config::checkIfAllRequiredFieldsExist(const toml_t &config) -> void {
    if (not config.contains("val")) {
        throw IllegalSetting(format_error(
            "missing key `val` in table:",
            config, "here"
        ));
    }
    if (not config.contains("pos")) {
        throw IllegalSetting(format_error(
            "missing key `pos` in table:",
            config, "here"
        ));
    }
}

auto Config::checkFieldSizes(const toml_t &area_config) -> void {
    const toml_t &val_settings = area_config.at("val");
    const toml_t &pos_settings = area_config.at("pos");

    if (not val_settings.is_array()) {
        throw IllegalSetting(format_error(
            "illegal type for field `val`:",
            val_settings, "should be an array"
        ));
    }

    if (not pos_settings.is_array()) {
        throw IllegalSetting(format_error(
            "illegal type for field `pos`:",
            pos_settings, "should be an array"
        ));
    }

    const uz num_val = val_settings.size();
    const uz num_pos = pos_settings.size();

    auto make_size_comment = [](const uz n) -> std::string {
        return fmt::format("size = {:d}", n);
    };

    if (num_val != num_pos) {
        throw IllegalSetting(format_error(
            "mismatched field size in area:",
            val_settings, make_size_comment(num_val),
            pos_settings, make_size_comment(num_pos),
            {"size of `val` and `pos` must be equal"}
        ));
    }

    if (const uz size = num_val;
        size < 2 or size > KEY_COUNT) {
        throw IllegalSetting(format_error(
            "illegal area size: out of range:",
            val_settings, make_size_comment(num_val),
            pos_settings, make_size_comment(num_pos),
            {"size of area should be in range [2, 30]"}
        ));
    }
}

auto Config::checkKeyValues(const toml_t &area_config) -> void {
    const toml_t &val_settings = area_config.at("val");
    for (const toml_t &val : val_settings.as_array()) {
        checkKeyValue(val);
    }
}

auto Config::checkPositions(const toml_t &area_config) -> void {
    const toml_t &pos_settings = area_config.at("pos");
    for (const toml_t &pos : pos_settings.as_array()) {
        checkPosition(pos);
    }
}

auto Config::checkKeyValue(const toml_t &val) -> void {
    if (not val.is_string()) {
        throw IllegalSetting(format_error(
            "illegal type for key value:",
            val, "should be a toml string"
        ));
    }

    const std::string &str = val.as_string().str;

    // key value should be a single letter
    if (str.length() != 1) {
        throw IllegalSetting(format_error(
            "illegal size for key value:",
            val, "should be a single char"
        ));
    }

    const auto v = static_cast<KeyValue>(str[0]);

    // key value should be a capital letter or
    // one of the 4 symbols: ',', '.', ';', '/'
    if (not Util::isKeyValueLegal(v)) {
        throw IllegalSetting(format_error(
            "illegal key value:",
            val, "should be a capital letter "
            "or one of the 4 symbols: ',', '.', ';' and '/'"
        ));
    }

    // key value should not duplicate
    if (isValProcessed(v)) {
        throw IllegalSetting(format_error(
            "duplicate key values:",
            *where_[v], "here",
            val, "and here"
        ));
    }

    where_[v] = &val;
}

auto Config::isValProcessed(const KeyValue val) const -> bool {
    return where_[val] != nullptr;
}

auto Config::checkPosition(const toml_t &pos) -> void {
    if (not pos.is_integer()) {
        throw IllegalSetting(format_error(
            "illegal type for position:",
            pos, "should be an integer"
        ));
    }

    const auto p = static_cast<Position>(pos.as_integer());

    // position should be in range [0, 29]
    if (not Util::isPositionLegal(p)) {
        throw IllegalSetting(format_error(
            "position out of range:",
            pos, "should be in range [0, 29]"
        ));
    }

    // position should not duplicate
    if (isPosProcessed(p)) {
        throw IllegalSetting(format_error(
            "duplicate positions:",
            *where_[p], "here",
            pos, "and here"
        ));
    }

    where_[p] = &pos;
}

auto Config::isPosProcessed(const Position pos) const -> bool {
    return where_[pos] != nullptr;
}

auto Config::loadFixedKeys(const toml_t &config) -> void {
    if (not config.contains("fixed_key")) {
        return;
    }
    fixed_keys_.reserve(num_fixed_keys_);
    for (const toml_t &key_setting : config.at("fixed_key").as_array()) {
        const KeyValue val = key_setting.at("val").as_string().str[0];
        const Position pos = key_setting.at("pos").as_integer();
        fixed_keys_.emplace_back(val, pos);
    }
}

auto Config::loadAreas(const toml_t &config) -> void {
    if (not config.contains("mutable_area")) {
        return;
    }
    mutable_areas_.reserve(num_areas_);
    for (const auto [area_id, area_config] // area_id is also the index of the area
         : config.at("mutable_area").as_array() | std::views::enumerate) {
        mutable_areas_.emplace_back(area_config);
        const uz area_size = mutable_areas_.back().size_;
        area_ids_.insert(area_ids_.end(), area_size, area_id);
    }
}

auto Config::makeDefatulArea() -> void {
    // Find if there's any unprocessed keys left
    num_unprocessed_keys_ = num_mutable_keys_;
    for (const Area &area : mutable_areas_) {
        num_unprocessed_keys_ -= area.size_;
    }

    // Create a default area with all unprocessed keys,
    // if there's any unprocessed keys left
    if (num_unprocessed_keys_ == 0) {
        return;
    }
    Area area(num_unprocessed_keys_);

    // Fill with the remaininig key values and positions
    for (const KeyValue val : KEY_CODES) {
        if (not isValProcessed(val)) {
            area.addKeyValue(val);
        }
    }
    for (const Position pos : POSITIONS) {
        if (not isPosProcessed(pos)) {
            area.addPosition(pos);
        }
    }
    std::ranges::sort(area.key_codes_);
    mutable_areas_.emplace_back(area);

    // Update area_ids_
    const uz area_id = num_areas_ - 1;
    area_ids_.insert(area_ids_.end(), area.size_, area_id);
}

}
