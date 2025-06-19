
/// @file Aig2Cnf.cc
/// @brief Aig2Cnf の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Aig2Cnf.h"


BEGIN_NAMESPACE_YM_SAT

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
  vector<vector<SatLiteral>> lits_list;
  for ( auto& aig: aig_list ) {
    auto lits = aig2cnf.make_cnf(aig);
    lits_list.push_back(lits);
  }
  return lits_list;
}


//////////////////////////////////////////////////////////////////////
// クラス Aig2Cnf
//////////////////////////////////////////////////////////////////////

// @brief AIG を CNF に変換する．
vector<SatLiteral>
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

  if ( mAigDict.count(aig) > 0 ) {
    // すでに計算済みならその結果を返す．
    auto lits = mAigDict.at(aig);
    return lits;
  }

  // aig.is_and()
  auto fanin_list = aig.ex_fanin_list();

  // 結果のリテラルを格納するリスト
  vector<SatLiteral> lits;
  if ( aig.inv() ) {
    // NAND
    // いずれかのファンインが成り立たなければよい．
    lits.reserve(1);
    auto lit = mSolver.new_variable(true);
    SizeType ni = fanin_list.size();
    vector<SatLiteral> tmp_lits;
    tmp_lits.reserve(ni + 1);
    tmp_lits.push_back(~lit);
    for ( SizeType i = 0; i < ni; ++ i ) {
      auto h = fanin_list[i];
      auto lits1 = make_cnf(~h);
      if ( lits1.empty() ) {
	// たぶんないはず
	continue;
      }
      if ( lits1.size() == 1 ) {
	auto lit1 = lits1.front();
	tmp_lits.push_back(lit1);
      }
      else {
	auto lit1 = mSolver.new_variable(false);
	for ( auto lit2: lits1 ) {
	  mSolver.add_clause(~lit1, lit2);
	}
	tmp_lits.push_back(lit1);
      }
    }
    mSolver.add_clause(tmp_lits);
    lits.push_back(lit);
  }
  else {
    // AND
    // すべてのファンインのリテラルが成り立つ必要がある．
    vector<vector<SatLiteral>> lits_list;
    lits_list.reserve(fanin_list.size());
    for ( auto& h: fanin_list ) {
      auto lits = make_cnf(h);
      lits_list.push_back(lits);
    }
    SizeType n = 0;
    for ( auto& lits: lits_list ) {
      n += lits.size();
    }
    lits.reserve(n);
    for ( auto& lits1: lits_list ) {
      lits.insert(lits.end(), lits1.begin(), lits1.end());
    }
  }
  mAigDict.emplace(aig, lits);
  return lits;
}

END_NAMESPACE_YM_SAT
