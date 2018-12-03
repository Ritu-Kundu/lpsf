/**
    lspf: Tool to compute Longest Successor and/or Previous Factor Array
    Copyright (C) 2017 Ritu Kundu
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

/** Header file containing the definitions, macros and declarations used by each
 * of the other modules.
 */
#ifndef GLOBAL_DEFS
#define GLOBAL_DEFS

#include <cstdint>
#include <iostream>
#include <list>
#include <vector>

namespace lspf {
using UINT_64 = uint64_t;
using UINT_32 = uint32_t;
using UINT_16 = uint16_t;
using UINT_8 = std::uint8_t;

using UINT = UINT_32;
using INT = int32_t;

/** Enum for various possible states (success or errors) rsturned from a
 * function */
enum class ReturnStatus {
  SUCCESS,
  ERR_ARGS,
  ERR_FILE_OPEN,
  ERR_INVALID_INPUT,
  ERR_INVALID_INDEX,
  ERR_LIMIT_EXCEEDS,
  ERR_EXTERNAL,
  HELP
};

/** Enum for various possible modes (corresponding to computing lsf or lpf) */
enum class ComputeMode {
  SUCC,
  PREV
};

} // end namespace
#endif
