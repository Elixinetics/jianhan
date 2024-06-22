#ifndef JIANHAN_UTILS_HPP
#define JIANHAN_UTILS_HPP

#include <array>
#include <vector>
#include <bitset>
#include <string>
#include <ranges>
#include <memory>
#include <filesystem>

#include "types.hpp"

namespace jianhan::v0 {

// 本算法仅考察 ANSI/IOS 键盘中心的一个 3 * 10 区域
// 即 QWERTY 键盘上的以下 30 个按键:
// Q W E R T Y U I O P
//  A S D F G H J K L ;
//   Z X C V B N M , . /

static constexpr uz KEY_COUNT = 30;
static constexpr uz COL_COUNT = 10;
static constexpr uz ROW_COUNT = 3;

// @formatter:off
static constexpr std::array<KeyValue, KEY_COUNT> KEY_CODES {
    ',', '.', '/', ';', 'A', 'B', 'C', 'D', 'E', 'F',
    'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
};

static constexpr std::array<Position, KEY_COUNT> POSITIONS {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
   10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
   20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
};
// @formatter:on

static constexpr uz KEY_CNT_POW2 = 32; // 30 -> 32
static constexpr uz MAX_KEY_CODE = 92; // 90 -> 92

class Util final {
public:
    static auto buildAbsPath(std::string_view sub_path) -> std::string;

    static auto isKeyValueLegal(KeyValue val) noexcept -> bool;
    static auto isPositionLegal(Position pos) noexcept -> bool;
    static auto isColLegal(Col col) noexcept -> bool;
    static auto isRowLegal(Row row) noexcept -> bool;

    static auto coord2pos(Row row, Col col) noexcept -> Position;
    static auto pos2col(Position pos) noexcept -> Col;
    static auto pos2row(Position pos) noexcept -> Row;

private:
    static constexpr std::array<KeyValue, 4> SIGNITURES{',', '.', ';', '/'};

    using NecessaryFiles = std::pair<std::string, std::vector<std::string>>;
    inline static const std::vector<NecessaryFiles> NECESSITIES{
        {"conf", {"layouts.toml"}},
    };

    static auto findProjectDirectory() -> std::string;
    static auto haveAllNecessaryFiles(const std::filesystem::path &path) -> bool;
};

}

#endif // JIANHAN_UTILS_HPP
