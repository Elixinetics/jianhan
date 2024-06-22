#include "utils.hpp"

namespace jianhan::v0 {

auto Util::buildAbsPath(std::string_view sub_path) -> std::string {
    static const std::string ROOT = findProjectDirectory();
    return fmt::format("{}/{}", ROOT, sub_path);
}

auto Util::findProjectDirectory() -> std::string {
    static constexpr size_t MAX_DEPTH = 8;

    auto path = std::filesystem::current_path();
    for (size_t i = 0; i < MAX_DEPTH; ++i) {
        if (haveAllNecessaryFiles(path)) {
            return path.generic_string();
        }
        path = path.parent_path();

        if (path == path.root_path()) {
            break;
        }
    }

    // cannot find the root directory of project, quit
    throw FatalError("cannot find the root directory of project");
}

auto Util::haveAllNecessaryFiles(const std::filesystem::path &path) -> bool {
    for (const auto &[dir_name, file_names] : NECESSITIES) {
        const std::filesystem::path sub_dir = path / dir_name;
        if (!exists(sub_dir) or !is_directory(sub_dir)) {
            return false;
        }

        for (const std::string &file_name : file_names) {
            if (not exists(sub_dir / file_name)) {
                return false;
            }
        }
    }
    return true;
}

auto Util::isKeyValueLegal(const KeyValue val) noexcept -> bool {
    if ('A' <= val and val <= 'Z') { return true; }
    return std::ranges::contains(SIGNITURES, val);
}

auto Util::isPositionLegal(Position pos) noexcept -> bool {
    return pos < KEY_COUNT;
}

auto Util::isColLegal(const Col col) noexcept -> bool {
    return col < COL_COUNT;
}

auto Util::isRowLegal(const Row row) noexcept -> bool {
    return row < ROW_COUNT;
}

auto Util::coord2pos(const Row row, const Col col) noexcept -> Position {
    assert(isRowLegal(row) and isColLegal(col));
    return row * COL_COUNT + col;
}

auto Util::pos2col(const Position pos) noexcept -> Col {
    assert(isPositionLegal(pos));
    return pos % COL_COUNT;
}

auto Util::pos2row(const Position pos) noexcept -> Row {
    assert(isPositionLegal(pos));
    return pos / COL_COUNT;
}

}
