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

/** Defines some helper functions */

#include "../include/utilDefs.hpp"

namespace lspf
{

static struct option long_options[] = {
    {"mode", required_argument, NULL, 'm'},
    {"input-file", required_argument, NULL, 'i'},
    {"verify", no_argument, NULL, 'v'},
    {"help", no_argument, NULL, 'h'},
    {NULL, 0, NULL, 0}};

/** Decode the input flags
 */
ReturnStatus decodeFlags(int argc, char *argv[], struct InputFlags &flags)
{
  int args = 0;
  int opt;
  std::string md;

  bool ver = false;
  flags.isLPF = true;
  flags.isLSF = true;

  /* initialisation */
  while ((opt = getopt_long(argc, argv, "m:i:v:h", long_options,
                            nullptr)) != -1)
  {
    switch (opt)
    {
    case 'm':
      md = std::string(optarg);
      args++;
      break;

    case 'i':
      flags.input_filename = std::string(optarg);
      args++;
      break;

    case 'v':
      ver = true;
      break;

    case 'h':
      return (ReturnStatus::HELP);
    }
  }
  if (args < 1)
  {
    std::cerr << "Invalid command: Too few arguments: " << std::endl;
    return (ReturnStatus::ERR_ARGS);
  }
  else
  {
    flags.isVerify = ver;
    flags.lsf_filename = flags.input_filename + ".LSF";
    flags.lpf_filename = flags.input_filename + ".LPF";
    if (md == "SUCC")
    { // Compute only LSF
      flags.isLPF = false;
    }
    else if (md == "PREV")
    { // Compute only LPF
      flags.isLSF = false;
    }
    return (ReturnStatus::SUCCESS);
  }
}

/*
 * Usage of the tool
 */
void usage(void)
{
  std::cout << " Usage: lspf <options>\n";
  std::cout << " Standard (Mandatory):\n";
  std::cout
      << "  -i, --input-file \t\t <str> \t \t Input file  name  "
         "\n\t\t\t\t\t\t(uncompressed file when mode is `COM'; compressed file when mode is `DECOM'; "
         "\n\t\t\t\t\t\t a file containing names of the files to compress or decompress in batch mode [one name on each line]).\n\n";

  std::cout << " Additional:\n";
  std::cout << "  -m, --mode \t\t\t<str> \t\t`BOTH' (default) for creating both the arrays"
               "\n\t\t\t\t\t\t or `SUCC' for only lsf array computation"
               "\n\t\t\t\t\t\t or `PREV' for only lpf array computation";
  std::cout
      << "  -v, --verify \t \t \t \t Verify the result by comparing gainast brute force method.  "
         "(Will take O(n^2) time for verification). \n\n";

  std::cout << " Output:\n";
  std::cout << "  <inputfile>.LSF \t\t File containing LSF array (if computed)"
               "\n\t\t\t\t\t\t and <inputfile>.LPF \t\t File containing LPF array (if computed)";
}

} // namespace lspf
