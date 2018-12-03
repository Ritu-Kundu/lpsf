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

/** Module defining the class that implements the main algorithm
 */
#ifndef LSPF_HPP
#define LSPF_HPP

#include "globalDefs.hpp"

//#define DETAILS

// NOTE: SDSL SA Construction doesn't work with 0 as a character
//#include <sdsl/lcp.hpp>
//#include <sdsl/suffix_arrays.hpp>
#include <sdsl/suffix_trees.hpp>

#include <cstddef>
#include <fstream>
#include <iterator>
//#include <queue>
//#include <sdsl/rmq_support.hpp>
//#include <stack>
#include <utility>

namespace lspf
{
/** Class LSPF
 * This class provides for the following:
 *  - Returns the occurences of pattern(degenerate) in input-sequence.
 * It prepares for constant-time LCA/LCP queries while being constructed.
 */

class LSPF
{
public:
  LSPF(const std::string &t);

  /** Destructor
   */
  ~LSPF();

  /** Returns the indices of occurences in the given list of the pth pattern */
  ReturnStatus getBeforeAfter(ComputeMode mode);
  ReturnStatus getLSPF(ComputeMode mode, std::vector<INT> &len_array, std::vector<INT> &ref_array);
  ReturnStatus getNaiveLSPF(ComputeMode mode, std::vector<INT> &len_array, std::vector<INT> &ref_array);

  //////////////////////////////////////////////////////////
  // PRIVATE
  //////////////////////////////////////////////////////////
private:
  const std::string &_cText; //< reference to the text (input sequence)

  const UINT _cTLen; //< Length of the input sequence

  //////////////////////////////////////////////////////////
  // Data-structures to help pre-processing & naive
  //////////////////////////////////////////////////////////
  //sdsl::csa_bitcompressed<> _csa; //< Suffix Array and Rank array

  //sdsl::lcp_bitcompressed<> _lcp; //< LCP array
  sdsl::cst_sct3<> _cst; // Compressed suffix tree (it consists if csa, lcp)
  /** Array contaning minimum leaf(suffix) in the subtree rooted at the each node of the cst.
   * _min_leaf [i] = j => j is the minimum suffix in the subtree of node with id=i  
   * _min_leaf [i] = sa[i] if i is the id is a leaf
   * */
  std::vector<INT> _min_leaf;
  /** Array contaning maximum leaf(suffix) in the subtree rooted at the each node of the cst.
   * _min_leaf [i] = j => j is the maximum suffix in the subtree of node with id=i  
   * _min_leaf [i] = sa[i] if i is the id is a leaf
   * */
  std::vector<INT> _max_leaf;

  //////////////////////////////////////////////////////////
  // Data-structures filled by pre-processing stage
  //////////////////////////////////////////////////////////
  /** Array contaning nearest smaller/larger position in SA before i
   * _before [i] = j => j is the nearest position in SA[0..i-1] such that j < (or >) i  
   * _before [i] = -1 if such j does not exist
   * */
  std::vector<INT> _before;

  /** Array contaning nearest smaller/larger position in SA after i
   * _after [i] = j => j is the nearest position in SA[i+1..n-1] such that j < (or >) i  
   * _after [i] = -1 if such j does not exist 
   * */
  std::vector<INT> _after;
};

} // namespace lspf

#endif
