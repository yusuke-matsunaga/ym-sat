
/// @file Aig2Cnf.cc
/// @brief Aig2Cnf の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Aig2Cnf.h"


BEGIN_NAMESPACE_YM_SAT

BEGIN_NONAMESPACE
SizeType count = 0;
SizeType c_count = 0;
SizeType l_count = 0;
END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// クラス SatSolver
//////////////////////////////////////////////////////////////////////

// @brief 与えられたAIGを充足する条件を追加する．
vector<vector<SatLiteral>>
SatSolver::add_aig(
  const vector<AigHandle>& aig_list,
  const LitMap& lit_map
)
{
  Aig2Cnf aig2cnf(*this, lit_map);
  count = 0;
  c_count = 0;
  l_count = 0;
  vector<vector<SatLiteral>> lits_list;
  lits_list.reserve(aig_list.size());
  for ( auto& aig: aig_list ) {
    auto lits = aig2cnf.make_cnf(aig);
    lits_list.push_back(lits);
  }
  cout << "count = " << count << endl
       << "clause count = " << c_count << endl
       << "literal count = " << l_count << endl;
  return lits_list;
}


//////////////////////////////////////////////////////////////////////
// クラス Aig2Cnf
//////////////////////////////////////////////////////////////////////

// @brief AIG を CNF に変換する．
std::vector<SatLiteral>
Aig2Cnf::make_cnf(
  const AigHandle& aig
)
{
  // 境界条件
  if ( aig.is_zero() ) {
    // 定数０は充足不可
    throw std::logic_error{"aig is zero"};
  }
  if ( aig.is_one() ) {
    // 定数１は常に充足している．
    return {};
  }
  if ( aig.is_input() ) {
    // 対応するリテラルを返す．
    auto input_id = aig.input_id();
    if ( mLitMap.count(input_id) == 0 ) {
      // 対応するリテラルが登録されていない．
      ostringstream buf;
      buf << "input_id[" << input_id << "] is not registered";
      throw std::logic_error{buf.str()};
    }
    auto lit = mLitMap.at(input_id);
    if ( aig.inv() ) {
      lit = ~lit;
    }
    return {lit};
  }

  // トップレベルが AND 条件の場合は節を作る必要がない．
  if ( !aig.inv() ) {
    // すべてのファンインのリテラルが成り立つ必要がある．
    auto fanin_list = aig.ex_fanin_list();
    std::vector<SatLiteral> lits_list;
    lits_list.reserve(fanin_list.size());
    for ( auto aig1: fanin_list ) {
      auto lit1 = _cnf_sub(aig1);
      lits_list.push_back(lit1);
    }
    return lits_list;
  }

  auto lit = _cnf_sub(aig);
  return {lit};
}

SatLiteral
Aig2Cnf::_cnf_sub(
  const AigHandle& aig
)
{
  if ( aig.is_const() ) {
    throw std::logic_error{"Aig2Cnf::make_cnf(): constant AIG"};
  }

  if ( aig.is_input() ) {
    // 対応するリテラルを返す．
    auto input_id = aig.input_id();
    if ( mLitMap.count(input_id) == 0 ) {
      // 対応するリテラルが登録されていない．
      std::ostringstream buf;
      buf << "input_id[" << input_id << "] is not registered";
      throw std::logic_error{buf.str()};
    }
    auto lit = mLitMap.at(input_id);
    if ( aig.inv() ) {
      lit = ~lit;
    }
    return lit;
  }

  auto posi_aig = aig.positive_handle();
  SatLiteral lit;
  if ( mAigDict.count(posi_aig) > 0 ) {
    // すでに計算済みならその結果を返す．
    lit = mAigDict.at(posi_aig);
  }
  else { // aig.is_and()
    ++ count;
    //auto fanin_list = aig.ex_fanin_list();
    auto fanin_list = std::vector<AigHandle>{aig.fanin0(), aig.fanin1()};
    SizeType ni = fanin_list.size();

    // 結果のリテラルを格納するリスト
    lit = mSolver.new_variable(true);
    {
      // lit が成り立たない条件 -> いずれかのファンインのリテラルが成り立たない．
      vector<SatLiteral> tmp_lits;
      tmp_lits.reserve(ni + 1);
      tmp_lits.push_back(lit);
      for ( auto& h: fanin_list ) {
	auto lit1 = _cnf_sub(h);
	tmp_lits.push_back(~lit1);
      }
      mSolver.add_clause(tmp_lits);
      ++ c_count;
      l_count += ni + 1;
    }
    {
      // lit が成り立つ条件 -> すべてのファンインのリテラルが成り立つ必要がある．
      for ( auto& h: fanin_list ) {
	auto lit1 = _cnf_sub(h);
	mSolver.add_clause(~lit, lit1);
	++ c_count;
	l_count += 2;
      }
    }
  }
  mAigDict.emplace(posi_aig, lit);
  if ( aig.inv() ) {
    lit = ~lit;
  }
  return lit;
}

END_NAMESPACE_YM_SAT
