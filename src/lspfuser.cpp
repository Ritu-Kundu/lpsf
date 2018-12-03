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

/** Module containing main() method which reads and preprocesses the input and
 * pattern files in the form required for identification of the ocuurences.
 */
#include "../include/LSPF.hpp"
#include "../include/globalDefs.hpp"
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <random>

//////////////////////////////////////////////////////////
// CONFIGURATION SETTINGS
//////////////////////////////////////////////////////////
/** Operating modes of the tool.
 * DETAILS: Check one or more strings by adding to the list <text_list>.
 * 
 * RANDOM: Test <cNum_Strings> number of random strings from the alphabet
 * <cAlphabet> of lengths varying up to <cMax_len>.
 * 
 * EXHAUSTIVE: Test all binary strings of length up to <max_len>.
 * 
 * RANDOM and EXHAUSTIVE mode give only the number of errors and errors (if any) (i.e. do not display the
 * results).
 * */
/** Choose the operating mode of the tool by uncommenting/commenting the corresponding macro.
 * Change the corresponding settings (as required).
 * */
#define DETAILS
//#define RANDOM
//#define EXHAUSTIVE

#ifdef DETAILS
std::list<std::string> text_list = {"abbaabbbaaabab", "ananas", "aabbabaabbaababbabab"};
#endif

#ifdef RANDOM
const std::string cAlphabet = "abc";
const int cNum_strings = 10;
const int cMax_len = 20;
#endif

#ifdef EXHAUSTIVE
const int max_len = 5;
#endif
//////////////////////////////////////////////////////////
// END OF CONFIGURATION SETTINGS
//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////

using namespace lspf;

int testLSPF(std::string &text, LSPF &lspf, ComputeMode mode);
int test(std::string &text);
std::ofstream statfile("stats.txt");

int main(int argc, char **argv)
{

  if (!statfile.is_open())
  {
    std::cerr << "Cannot open  stats file \n";
    return 1;
  }
  statfile << "n"
           << "\t"
           << "lpf-time (in microsec)"
           << "\t"
           << "lsf_time (in microsec) \n";

#ifdef DETAILS
  for (auto it = text_list.begin(); it != text_list.end(); ++it)
  {
    auto num_errors = test(*it);
    if (num_errors > 0)
    {
      std::cout << "FOUND ERROR :::::::::::::\n";
      break;
    }
  }

#endif

//########################################################################################################
#ifdef RANDOM

  // Generate Strings
  const std::string cAlphabet = "abc";
  const int cNum_strings = 10;
  const int cMax_len = 20;

  std::random_device rd;
  std::mt19937 gen_len(rd());
  std::uniform_int_distribution<> dis_len(2, cMax_len);
  std::mt19937_64 gen_char{std::random_device()()};
  std::uniform_int_distribution<size_t> dist_char{0, cAlphabet.length() - 1};

  int failed_strings = 0;
  for (int i = 0; i < cNum_strings; ++i)
  {
    // Choose length
    auto len = dis_len(gen_len);
    // Generate string
    std::string text;
    std::generate_n(std::back_inserter(text), len,
                    [&] { return cAlphabet[dist_char(gen_char)]; });
    std::cout << "String # " << i << std::endl;

    // Find LUF
    auto num_errors = test(text);
    if (num_errors > 0)
    {
      std::cout << "FOUND ERROR :::::::::::::\n";
      ++failed_strings;
      break;
    }
  }
  if (failed_strings == 0)
  {
    std::cout << "All successfull" << std::endl;
  }
  else
  {
    std::cout << "num Failed Strings: " << failed_strings << std::endl;
  }

#endif
//########################################################################################################
#ifdef EXHAUSTIVE
  // Generate Strings
  std::cout << "************Starting" << std::endl;
  const int max_len = 5;
  int failed_strings = 0;
  const int num_rows = 1 << (max_len - 1); // 2^(max_len-1)
  const int num_col = max_len;
  std::vector<std::vector<char>> all_strings(num_rows,
                                             std::vector<char>(num_col, 'a'));
  ;
  // Fill the strings array
  int block_size = 1;
  for (auto i = 1; i < num_col; ++i)
  {
    const int start_block_ind = block_size;
    const int next_block_ind = 2 * block_size;
    // std::cout << "col bs sind nind: " << i << " "<<block_size << " " <<
    // start_block_ind << " " <<next_block_ind<<std::endl;
    for (auto j = start_block_ind; j < num_rows; j = j + next_block_ind)
    {
      for (auto k = 0; k < block_size; ++k)
      {
        all_strings[j + k][i] = 'b';
      }
    }
    block_size = 2 * block_size;
  }
  for (auto i = 0; i < num_rows; ++i)
  {
    for (auto j = 0; j < num_col; ++j)
    {
      std::cout << all_strings[i][j] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "Starting building LPF/LSF Arrays.\n";

  for (int str_len = 0; str_len < max_len; ++str_len)
  {
    auto len = str_len;
    auto num_str = 1 << str_len - 1; // 2^(str_len-1)
    for (auto row = 0; row < num_str; ++row)
    {
      // Generate string
      std::string text(all_strings[row].data(), str_len);
      std::cout << "row col " << row << " " << str_len << std::endl;

      // Find LPF/LSF
      auto num_errors = test(text);
      if (num_errors > 0)
      {
        std::cout << "FOUND ERROR :::::::::::::\n";
        ++failed_strings;
        break;
      }
    }
  }
  if (failed_strings == 0)
  {
    std::cout << "All successfull" << std::endl;
  }
  else
  {
    std::cout << "num Failed Strings: " << failed_strings << std::endl;
  }
#endif

  return 0;
}

// Returns number of errors found in LPF as well as LSF
int test(std::string &text)
{
  LSPF lspf(text);
  int num_errors = 0;
  std::cout << "\n\n*****************\n"
            << text << "\n\n*****************\n";

  // std::cout << "================ LPF Testing ================\n";
  std::cout << "::: LPF Arrays ::: \n";

  statfile << text.size() << "\t";
  num_errors = testLSPF(text, lspf, ComputeMode::PREV);
  if (num_errors > 0)
  {
    std::cout << "================ LPF ERROR!!!! ================\n";
    return num_errors;
  }

  // std::cout << "================ LSF Testing ================\n";

  std::cout << "::: LSF Arrays ::: \n";

  statfile << "\t";
  num_errors = testLSPF(text, lspf, ComputeMode::SUCC);
  if (num_errors > 0)
  {
    std::cout << "================ LSF ERROR!!!! ================\n";
  }
  statfile << "\n";
  return num_errors;
}

// Returns number of errors found
int testLSPF(std::string &text, LSPF &lspf, ComputeMode mode)
{
  std::vector<INT> lspf_l(text.size(), 0);
  std::vector<INT> naiveLspf(text.size(), 0);
  std::vector<INT> lspf_r(text.size(), 0);
  std::vector<INT> naiveLspf_r(text.size(), 0);

  // std::cout << "Computing ***********>\n";
  auto t_start = std::chrono::high_resolution_clock::now();
  lspf.getLSPF(mode, lspf_l, lspf_r);
  auto t_end = std::chrono::high_resolution_clock::now();
  double elaspedTimeMs =
      std::chrono::duration<double, std::milli>(t_end - t_start).count();
  statfile << elaspedTimeMs;
  // std::cout << "Computing Finished ***********>\n";

  // Verify
  lspf.getNaiveLSPF(mode, naiveLspf, naiveLspf_r);

  int num_errors = 0;
  int n = text.size();

  // std::cout << "Naive Computing Done. Verifying now. ======= " << std::endl;
  for (auto i = 0; i < text.size(); ++i)
  {
    // std::cout << i << "\t" << lspf_l[i] << "\t" << lspf_r[i]<<"\n";
    if (lspf_l[i] != naiveLspf[i])
    {
      std::cout << "LEN ERROR ::: (i, calc, naive): " 
                << i << " " << lspf_l[i] << " " << naiveLspf[i] << "\n";
      ++num_errors;
    }
    if (lspf_r[i] != naiveLspf_r[i])
    {
      std::cout << "REF ERROR ::: (i, calc, naive): " 
                << i << " " << lspf_r[i] << " " << naiveLspf_r[i] << "\n";
      ++num_errors;
    }
  }
  std::cout << "Number of errors: " << num_errors << "\n"
            << std::endl;
#ifdef DETAILS
  std::cout << "i \t\t lspf_l[i] \t\t lspf_r[i]\n";
  for (auto i = 0; i < n; ++i)
  {
    std::cout << i << " \t\t " << lspf_l[i] << "\t\t" << lspf_r[i] << "\n";
  }
#endif
  return num_errors;
}