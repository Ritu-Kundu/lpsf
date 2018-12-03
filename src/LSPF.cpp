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

/** Module implementing the class LSPF
 */
#ifndef LSPF_CPP
#define LSPF_CPP

#include "LSPF.hpp"

namespace lspf {
LSPF::LSPF(const std::string &t)
    : _cText(t), _cTLen(t.size()), _before(_cTLen, 0), _after(_cTLen, 0) {
  /* Preprocess */
  // Compute Suffix Tree */

  sdsl::construct_im(_cst, _cText, 1); // 1 for alphabet type
  /*
  std::cout << "DFS traversal\n";
  for (auto it = _cst.begin(); it != _cst.end(); ++it)
  {
    if (it.visit() == 1)
    {               // node visited the first time
      auto v = *it; // get the node by dereferencing the iterator
                    // process node, e.g. output it in format d-[lb, rb]
      std::cout << _cst.depth(v) << "-[" << _cst.lb(v) << "," << _cst.rb(v) <<
  "]" << std::endl;
    }
  }
  */
  // std::cout << "Bottom up traversal\n";
  /* Assumption: Id of each leaf in the cst is same as the suffix it represents.
   * Therfore, internal node id will start after number of leaver. In the
   * current implementation of sdsl, this is the case.*/

  _min_leaf.reserve(_cst.nodes());
  _max_leaf.reserve(_cst.nodes());
  for (auto i = 0; i < _cst.csa.size();
       ++i) { // leaves have max/min same as the suffix they represent
    _min_leaf.push_back(_cst.csa[i]);
    _max_leaf.push_back(_cst.csa[i]);
  }
  auto max_suff = _cst.csa.size();
  for (auto i = _cst.csa.size(); i < _cst.nodes();
       ++i) { // internal nodes have max/min initialised to 0
    _min_leaf.push_back(max_suff);
    _max_leaf.push_back(0);
  }
#ifdef DETAILS
  std::cout << "v\t\t id \t\t min_leaf \t\t max_leaf\n";
#endif
  for (auto it = _cst.begin_bottom_up(); it != _cst.end_bottom_up(); ++it) {
    auto v = *it; // get the node by dereferencing the iterator
                  // process node, e.g. output it in format d-[lb, rb]
    auto pid = _cst.id(_cst.parent(v));
    auto vid = _cst.id(v);
    if (_min_leaf[vid] < _min_leaf[pid]) {
      _min_leaf[pid] = _min_leaf[vid];
    }
    if (_max_leaf[vid] > _max_leaf[pid]) {
      _max_leaf[pid] = _max_leaf[vid];
    }
#ifdef DETAILS
    std::cout << _cst.depth(v) << "-[" << _cst.lb(v) << "," << _cst.rb(v) <<std::left << std::setw(20) << "]"
              << "\t\t" << vid << "\t\t" << _min_leaf[vid] << "\t\t"
              << _max_leaf[vid] << std::endl;
#endif
  }
}

LSPF::~LSPF(void) {}

ReturnStatus LSPF::getBeforeAfter(ComputeMode mode) {
  const INT len = _cTLen;
  /* Simulate doubly linked list */
  std::vector<INT> list(len);
  std::vector<INT> next(len); // next pointer (index)
  std::vector<INT> pvs(len);  // pvs pointer (index)
  // Note here that sa[0] is the end of string marker.
  // Therefore, start from sa[1]; and subtract 1 from isa[i];
  for (INT i = 0; i < len; ++i) {
    next[i] = i + 1;
    pvs[i] = i - 1;
    list[i] = _cst.csa[i + 1];
  }

  INT start = 0;
  INT stop = 0;
  if (mode == ComputeMode::PREV) { // smaller index before and after
    start = len - 1;
    stop = -1;
  } else if (mode == ComputeMode::SUCC) { // larger index before and after
    stop = len;
  } else {
    std::cerr << "ERR: Invalid mode.";
    return ReturnStatus::ERR_ARGS;
  }
  INT i = start;
  while (i != stop) {
    INT r = _cst.csa.isa[i] - 1;
    INT n = next[r];
    INT p = pvs[r];
    if (p == -1) {
      _before[i] = -1;
    } else {
      _before[i] = list[p];
    }
    if (n == len) {
      _after[i] = -1;
    } else {
      _after[i] = list[n];
    }

    // delete i (conceptually)
    list[r] = -1;
    if (p >= 0) {
      next[p] = n;
    }
    if (n < len) {
      pvs[n] = p;
    }
    if (mode == ComputeMode::PREV) { // smaller index before and after
      --i;
    } else if (mode == ComputeMode::SUCC) { // larger index before and after
      ++i;
    }
  }
#ifdef DETAILS
  std::cout << "i \t\t sa[i] \t\t before[SA[i]] \t\t after[Sa[i]]\n";
  for (auto i = 0; i < len; ++i) {
    std::cout << i << " \t\t " << _cst.csa[i + 1] << " \t\t "
              << _before[_cst.csa[i + 1]] << " \t\t " << _after[_cst.csa[i + 1]]
              << "\n";
  }
#endif
  return ReturnStatus::SUCCESS;
}

ReturnStatus LSPF::getLSPF(ComputeMode mode, std::vector<INT> &len_array,
                           std::vector<INT> &ref_array) {
  const INT len = _cTLen;
  if (len_array.size() != len || ref_array.size() != len) {
    std::cerr << "ERR: Length and/or Reference array size not sufficient.";
    return ReturnStatus::ERR_LIMIT_EXCEEDS;
  }
  getBeforeAfter(mode);
  // longest common prefix of the suffix i that occurs before it in sa-order at
  // an index (smaller/large) than i.
  std::vector<INT> lcp_before(len, 0);
  // longest common prefix of the suffix i that occurs after it in sa-order at
  // an index (smaller/large) than i.
  std::vector<INT> lcp_after(len, 0);
  // least len of the factor repeated before in sa-order at an  index
  // (smaller/large) than i.
  INT lb = 0;
  // least len of the factor repeated after in sa-order at an  index
  // (smaller/large) than i.
  INT la = 0;

  INT start = 0;
  INT stop = len;
  if (mode == ComputeMode::PREV) {
    start = 1;
    ref_array[0] = -1;
  } else if (mode == ComputeMode::SUCC) {
    stop = len - 1;
    ref_array[len-1] = -1;
  } else {
    std::cerr << "ERR: Invalid mode.";
    return ReturnStatus::ERR_ARGS;
  }
  for (INT i = start; i < stop; ++i) {
    lb = std::max(lcp_before[i - 1] - 1, 0);
    la = std::max(lcp_after[i - 1] - 1, 0);
    /* Find length of the longset previous/successor factor */
    // Compute lcp_before at i
    if (_before[i] == -1) {
      lcp_before[i] = 0;
    } else {
      auto j = _before[i];
      while (_cText[i + lb] == _cText[j + lb]) {
        ++lb;
      }
      lcp_before[i] = lb;
    }
    // Compute lcp_after at i
    if (_after[i] == -1) {
      lcp_after[i] = 0;
    } else {
      auto j = _after[i];
      while (_cText[i + la] == _cText[j + la]) {
        ++la;
      }
      lcp_after[i] = la;
    }
    // Choose max of the two
    len_array[i] = std::max(lcp_before[i], lcp_after[i]);

    /* Find leftmost/rightmost position */
    if (len_array[i] == 0) {
      ref_array[i] = -1;
    } else {
      // See sdsl tutorial
      auto occ = _after[i];
      if (lcp_before[i] > lcp_after[i]) {
        occ = _before[i];
      }
      // Find node corresponding to lca of these vertices (constant time)
      // auto v = _cst.node(left_in_lcp, right_in_lcp); // lca of these suffixes
      auto leaf_i = _cst.select_leaf(_cst.csa.isa[i] + 1);
      auto leaf_occ = _cst.select_leaf(_cst.csa.isa[occ] + 1);
      auto v = _cst.lca(leaf_i, leaf_occ); // lca of these suffixes
      auto vid = _cst.id(v);
      if (mode == ComputeMode::PREV) {
        // auto l = _cst.leftmost_leaf(v);
        // ref_array[i] = _cst.sn(l);
        ref_array[i] = _min_leaf[vid];
      } else if (mode == ComputeMode::SUCC) {
        // auto l = _cst.rightmost_leaf(v);
        // ref_array[i] = _cst.sn(l);
        ref_array[i] = _max_leaf[vid];
      } else {
        std::cerr << "ERR: Invalid mode.";
        return ReturnStatus::ERR_ARGS;
      }
    }
  }
#ifdef DETAILS
  std::cout << "i \t\t sa[i] \t\t lcp_before[SA[i]] \t\t lcp_after[Sa[i]] \t\t lspf_l[Sa[i]] \t\t lspf_r[Sa[i]]\n";
  for (auto i = 0; i < len; ++i) {
    std::cout << i << " \t\t " << _cst.csa[i + 1] << " \t\t "
              << lcp_before[_cst.csa[i + 1]] << "\t\t"
              << lcp_after[_cst.csa[i + 1]] <<" \t\t " << len_array[_cst.csa[i + 1]] << "\t\t" << ref_array[_cst.csa[i + 1]]<<"\n";
  }
#endif
  return ReturnStatus::SUCCESS;
}

ReturnStatus LSPF::getNaiveLSPF(ComputeMode mode, std::vector<INT> &len_array,
                                std::vector<INT> &ref_array) {
  const INT len = _cTLen;
  INT start = 0;
  INT stop = len;
  if (mode == ComputeMode::PREV) { // smaller index before and after
    start = 1;
    ref_array[0] = -1;
  } else if (mode == ComputeMode::SUCC) { // larger index before and after
    stop = len - 1;
    ref_array[len-1] = -1;
  } else {
    std::cerr << "ERR: Invalid mode.";
    return ReturnStatus::ERR_ARGS;
  }
  for (INT i = start; i < stop; ++i) {
    if (mode == ComputeMode::PREV) {
      len_array[i] = 0;
      ref_array[i] = -1;
      for (INT j = 0; j < i; ++j) {
        auto lca = _cst.lca(_cst.select_leaf(_cst.csa.isa[i] + 1), _cst.select_leaf(_cst.csa.isa[j] + 1));
        auto lcp = _cst.depth(lca);
        if (lcp > len_array[i]) {
          len_array[i] = lcp;
          ref_array[i] = j;
        }
      }
    } else // successor factor
    {
      len_array[i] = 0;
      ref_array[i] = -1;
      for (INT j = len - 1; j > i; --j) {
        auto lca = _cst.lca(_cst.select_leaf(_cst.csa.isa[i] + 1), _cst.select_leaf(_cst.csa.isa[j] + 1));
        auto lcp = _cst.depth(lca);
        if (lcp > len_array[i]) {
          len_array[i] = lcp;
          ref_array[i] = j;
        }
      }
    }    
  }
  return ReturnStatus::SUCCESS;
}

} // namespace lspf

#endif
