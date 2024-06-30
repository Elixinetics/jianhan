#include "utils.hpp"

namespace jianhan::v0 {

/*!
 * @brief Make absolute path from sub path.
 * @param sub_path relative path to the project root directory,
 *                 no / at the beginning (e.g. "folder/file.txt").
 * @return the absolute path to sub_path (e.g. "c:/jianhan/folder/file.txt").
 * @note This function does not check the legality of input and output paths.
 */
auto Util::mkAbsPath(std::string_view sub_path) -> std::string {
    static const std::string ROOT = findProjectDirectory();
    return fmt::format("{}/{}", ROOT, sub_path);
}

/*!
 * @brief Find the root directory of project according to necessary files.
 * @return the absolute path to the root directory of project (e.g. "c:/jianhan").
 */
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

    // cannot find the root directory of the project, exit
    throw std::runtime_error("missing necessary files");
}

auto Util::haveAllNecessaryFiles(const std::filesystem::path &path) -> bool {
    for (const auto &[sub_dir_name, file_names] : NECESSARY_FILES) {
        const std::filesystem::path curr_dir = path / sub_dir_name;
        if (not exists(curr_dir) or not is_directory(curr_dir)) {
            return false;
        }
        for (const std::string &file_name : file_names) {
            if (not exists(curr_dir / file_name)) {
                return false;
            }
        }
    }
    return true;
}

auto Util::isKeyValueLegal(const KeyValue val) noexcept -> bool {
    return ('A' <= val and val <= 'Z')
           or val == ',' or val == '.'
           or val == '/' or val == ';';
}

auto Util::isPositionLegal(const Position pos) noexcept -> bool {
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
