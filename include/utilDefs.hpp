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

/** @file utilDefs.hpp
 * @brief Defines various functions required to parse the input command.
 */

#ifndef UTIL_DEFS
#define UTIL_DEFS

#include <cctype>
#include <getopt.h>

#include "globalDefs.hpp"

namespace lspf {
/** Structure defining various input flags.
 * */
struct InputFlags {
  bool isLSF; // Should lsf array be calculated?
  bool isLPF; // Should lpf array be calculated?
  std::string input_filename; // Input file name
  std::string lsf_filename; // Output file name for writing lsf arrays
  std::string lpf_filename; // Output file name for writing lpf arrays
  bool isVerify; // Should result be verified by comparing against brute-force method
};

/** @brief Prints the usage instructions of the tool.
 * */
void usage(void);

/** @brief Decodes the input parameters to populate the corresponding flags.
 * */
ReturnStatus decodeFlags(int argc, char *argv[], struct InputFlags &flags);

} // end namespace

#endif
