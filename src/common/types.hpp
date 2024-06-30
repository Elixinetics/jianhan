/*
           _   _                     _   _
          | | (_)   __ _   _ __     | | | |   __ _   _ __
       _  | | | |  / _` | | '_ \    | |_| |  / _` | | '_ \
      | |_| | | | | (_| | | | | |   |  _  | | (_| | | | | |
       \___/  |_|  \__,_\ |_| |_|   |_| |_|  \__,_\ |_| |_|

This project only considers the center region of the ANSI keyboard.
For a standard QWERTY keyboard, it is the 3 * 10 grid as follows:

                      Q W E R T Y U I O P
                      A S D F G H J K L ;
                      Z X C V B N M , . /
*/

#ifndef JIANHAN_TYPES_HPP
#define JIANHAN_TYPES_HPP

#include <cstdint>
#include <stdfloat>
#include <exception>

#include <fmt/core.h>
#include <toml.hpp>

#include "romu_trio.hpp"

namespace jianhan::v0 {

using f16 = std::bfloat16_t;
using fz = std::float32_t;
using u8 = uint_fast8_t;
using uz = size_t;

using KeyValue = uint8_t; // Key Code (ASCII), ∈ [65, 90] ∪ {44, 46, 47, 59}
using Position = u8; // Position number, ∈ [0, 29]
using Col = u8; // Column number, ∈ [0, 9]
using Row = u8; // Row number, ∈ [0, 2]

using Prng = RomuTrio64;

}

#endif // JIANHAN_TYPES_HPP
