
/// @file Expr2Cnf.cc
/// @brief Expr2Cnf の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "Expr2Cnf.h"


BEGIN_NAMESPACE_YM_SAT

vector<SatLiteral>
Expr2Cnf::add_expr(
  const Expr& expr ///< [in] 対象の論理式
)
{
  if ( expr.is_zero() ) {
    // 充足不能
    throw std::invalid_argument{"expr is zero"};
  }
  if ( expr.is_one() ) {
    // 無条件で充足している．
    return {};
  }
  if ( expr.is_literal() ) {
    auto vid = expr.varid();
    if ( mLitMap.count(vid) == 0 ) {
      throw std::logic_error{"varid is not registered"};
    }
    auto lit = mLitMap.at(vid);
    if ( expr.is_nega_literal() ) {
      lit = ~lit;
    }
    return {lit};
  }
  if ( expr.is_and() ) {
    vector<vector<SatLiteral>> lits_list;
    lits_list.reserve(expr.operand_num());
    SizeType n = 0;
    for ( auto& expr1: expr.operand_list() ) {
      auto lits1 = add_expr(expr1);
      lits_list.push_back(lits1);
      n += lits1.size();
    }
    vector<SatLiteral> lits;
    lits.reserve(n);
    for ( auto& lits1: lits_list ) {
      lits.insert(lits.end(), lits1.begin(), lits1.end());
    }
    return lits;
  }
  if ( expr.is_or() ) {
    auto new_lit = mSolver.new_variable(false);
    vector<SatLiteral> lits;
    lits.reserve(expr.operand_num() + 1);
    lits.push_back(~new_lit);
    for ( auto& expr1: expr.operand_list() ) {
      auto lits1 = add_expr(expr1);
      if ( lits1.empty() ) {
	continue;
      }
      if ( lits1.size() == 1 ) {
	auto lit1 = lits1.front();
	lits.push_back(lit1);
      }
      else {
	auto lit1 = mSolver.new_variable(false);
	for ( auto lit: lits1 ) {
	  mSolver.add_clause(~lit1, lit);
	}
	lits.push_back(lit1);
      }
    }
    mSolver.add_clause(lits);
    return {new_lit};
  }
  if ( expr.is_xor() ) {
    // EXOR は両極性を必要とするので非常に効率が悪い．
    auto new_lit = mSolver.new_variable(false);
    vector<SatLiteral> lits;
    lits.reserve(expr.operand_num());
    for ( auto& expr1: expr.operand_list() ) {
      auto lit1 = mSolver.new_variable(false);
      auto lits1 = add_expr(expr1);
      mSolver.add_andgate(lit1, lits1);
      lits.push_back(lit1);
    }
    mSolver.add_xorgate(new_lit, lits);
    return {new_lit};
  }
  throw std::invalid_argument{"unexpected error"};
  return {};
}

END_NAMESPACE_YM_SAT
