
/// @file SatSolver_TseitinEnc.cc
/// @brief SatSolver の実装ファイル(Tseitin encoding関係)
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolver.h"
#include "Expr2Cnf.h"
#include "Aig2Cnf.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatSolver
//////////////////////////////////////////////////////////////////////

// @brief n入力ANDゲートの入出力の関係を表す条件を追加する．
void
SatSolver::add_andgate(
  SatLiteral olit,
  const vector<SatLiteral>& lit_list
)
{
  SizeType n{lit_list.size()};
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n + 1);
  for ( auto ilit: lit_list ) {
    add_clause(ilit, ~olit);
    tmp_lits.push_back(~ilit);
  }
  tmp_lits.push_back(olit);
  add_clause(tmp_lits);
}

// @brief n入力ORゲートの入出力の関係を表す条件を追加する．
void
SatSolver::add_orgate(
  SatLiteral olit,
  const vector<SatLiteral>& lit_list
)
{
  SizeType n{lit_list.size()};
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n + 1);
  for ( auto ilit: lit_list ) {
    add_clause(~ilit, olit);
    tmp_lits.push_back(ilit);
  }
  tmp_lits.push_back(~olit);
  add_clause(tmp_lits);
}

// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
void
SatSolver::_add_xorgate_sub(
  SatLiteral olit,
  const vector<SatLiteral>& lit_list,
  SizeType start,
  SizeType num
)
{
  ASSERT_COND( num >= 2 );

  if ( num == 2 ) {
    auto lit0 = lit_list[start + 0];
    auto lit1 = lit_list[start + 1];
    add_xorgate(olit, lit0, lit1);
  }
  else if ( num == 3 ) {
    auto lit0 = lit_list[start + 0];
    auto lit1 = lit_list[start + 1];
    auto lit2 = lit_list[start + 2];
    add_xorgate(olit, lit0, lit1, lit2);
  }
  else {
    SizeType nl{num / 2};
    SizeType nr{num - nl};
    auto llit = new_variable(false);
    _add_xorgate_sub(llit, lit_list, start, nl);
    auto rlit = new_variable(false);
    _add_xorgate_sub(rlit, lit_list, start + nl, nr);
    add_xorgate(olit, llit, rlit);
  }
}

// @brief 与えられた論理式を充足する条件を追加する．
vector<SatLiteral>
SatSolver::add_expr(
  const Expr& expr,
  const LitMap& lit_map
)
{
  Expr2Cnf expr2cnf(*this, lit_map);
  return expr2cnf.make_cnf(expr);
}

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

// @brief half_adder の入出力の関係を表す条件を追加する．
void
SatSolver::add_half_adder(
  SatLiteral alit,
  SatLiteral blit,
  SatLiteral slit,
  SatLiteral olit
)
{
  add_clause(~slit,  alit,  blit);
  add_clause( slit,  alit, ~blit);
  add_clause( slit, ~alit,  blit);
  add_clause(~slit, ~alit, ~blit);
  add_clause(~olit,  alit       );
  add_clause(~olit,         blit);
  add_clause( olit, ~alit, ~blit);
}

// @brief full_adder の入出力の関係を表す条件を追加する．
void
SatSolver::add_full_adder(
  SatLiteral alit,
  SatLiteral blit,
  SatLiteral ilit,
  SatLiteral slit,
  SatLiteral olit
)
{
  add_clause(~slit,  alit,  blit,  ilit);
  add_clause( slit,  alit,  blit, ~ilit);
  add_clause( slit,  alit, ~blit,  ilit);
  add_clause(~slit,  alit, ~blit, ~ilit);
  add_clause( slit, ~alit,  blit,  ilit);
  add_clause(~slit, ~alit,  blit, ~ilit);
  add_clause(~slit, ~alit, ~blit,  ilit);
  add_clause( slit, ~alit, ~blit, ~ilit);
  add_clause(~olit,  alit,  blit       );
  add_clause(~olit,  alit,         ilit);
  add_clause(~olit,         blit,  ilit);
  add_clause( olit, ~alit, ~blit       );
  add_clause( olit, ~alit,        ~ilit);
  add_clause( olit,        ~blit, ~ilit);
}

// @brief 多ビットadderの入出力の関係を表す条件を追加する．
void
SatSolver::add_adder(
  const vector<SatLiteral>& alits,
  const vector<SatLiteral>& blits,
  SatLiteral ilit,
  const vector<SatLiteral>& slits,
  SatLiteral olit
)
{
  SizeType na{alits.size()};
  SizeType nb{blits.size()};
  SizeType ns{slits.size()};
  ASSERT_COND( na <= ns );
  ASSERT_COND( nb <= ns );

  bool a_zero = false;
  bool b_zero = false;
  bool c_zero = false;
  for ( SizeType i = 0; i < ns; ++ i ) {
    auto slit = slits[i];
    auto alit = SatLiteral::X;
    auto blit = SatLiteral::X;
    if ( i < na ) {
      alit = alits[i];
    }
    else {
      a_zero = true;
    }
    if ( i < nb ) {
      blit = blits[i];
    }
    else {
      b_zero = true;
    }
    if ( a_zero && b_zero ) {
      if ( c_zero ) {
	add_clause(~slit);
      }
      else {
	add_buffgate(ilit, slit);
	c_zero = true;
      }
      if ( i == (ns - 1) ) {
	add_clause(~olit);
      }
    }
    else if ( a_zero ) { // !b_zero
      auto olit1 = i < (ns - 1) ? new_variable(false) : olit;
      add_half_adder(blit, ilit, slit, olit1);
      ilit = olit1;
    }
    else if ( b_zero ) { // !a_zero
      auto olit1 = i < (ns - 1) ? new_variable(false) : olit;
      add_half_adder(alit, ilit, slit, olit1);
      ilit = olit1;
    }
    else { // !a_zero && !b_zero
      auto olit1 = i < (ns - 1) ? new_variable(false) : olit;
      add_full_adder(alit, blit, ilit, slit, olit1);
      ilit = olit1;
    }
  }
}

BEGIN_NONAMESPACE

SizeType
get_ln(
  SizeType n
)
{
  SizeType n_ln = 0;
  while ( (1 << n_ln) <= n ) {
    ++ n_ln;
  }
  return n_ln;
}

END_NONAMESPACE

// @brief 1's counter の入出力の関係を表す条件を追加する．
vector<SatLiteral>
SatSolver::add_counter(
  const vector<SatLiteral>& ilits,
  bool decision
)
{
  SizeType ni{ilits.size()};
  if ( ni == 1 ) {
    auto olit = ilits[0];
    return {olit};
  }
  else if ( ni == 2 ) {
    auto olit0 = new_variable(decision);
    auto olit1 = new_variable(decision);
    add_half_adder(ilits[0], ilits[1], olit0, olit1);
    return {olit0, olit1};
  }
  else if ( ni == 3 ) {
    auto olit0 = new_variable(decision);
    auto olit1 = new_variable(decision);
    add_full_adder(ilits[0], ilits[1], ilits[2], olit0, olit1);
    return {olit0, olit1};
  }
  else if ( ni == 4 ) {
    auto olit0 = new_variable(decision);
    auto olit1 = new_variable(decision);
    auto olit2 = new_variable(decision);
    auto c0 = new_variable(decision);
    auto c1 = new_variable(decision);
    auto d0 = new_variable(decision);
    auto d1 = new_variable(decision);
    auto e1 = new_variable(decision);
    add_half_adder(ilits[0], ilits[1], c0, c1);
    add_half_adder(ilits[2], ilits[3], d0, d1);
    add_half_adder(c0, d0, olit0, e1);
    add_full_adder(c1, d1, e1, olit1, olit2);
    return {olit0, olit1, olit2};
  }
  else if ( ni == 5 ) {
    auto olit0 = new_variable(decision);
    auto olit1 = new_variable(decision);
    auto olit2 = new_variable(decision);
    auto c0 = new_variable(decision);
    auto c1 = new_variable(decision);
    auto d0 = new_variable(decision);
    auto d1 = new_variable(decision);
    auto e1 = new_variable(decision);
    add_half_adder(ilits[0], ilits[1], c0, c1);
    add_half_adder(ilits[2], ilits[3], d0, d1);
    vector<SatLiteral> clits{c0, c1};
    vector<SatLiteral> dlits{d0, d1};
    vector<SatLiteral> tmp_olits{olit0, olit1};
    add_adder(clits, dlits, ilits[4], tmp_olits, olit2);
    return {olit0, olit1, olit2};
  }
  else {
    SizeType ni1{(ni - 1) / 2};
    vector<SatLiteral> tmp_ilits1(ni1);
    for ( SizeType i = 0; i < ni1; ++ i ) {
      tmp_ilits1[i] = ilits[i];
    }
    auto tmp_olits1 = add_counter(tmp_ilits1);

    SizeType ni2{ni - ni1 - 1};
    vector<SatLiteral> tmp_ilits2(ni2);
    for ( SizeType i = 0; i < ni2; ++ i ) {
      tmp_ilits2[i] = ilits[i + ni1];
    }
    auto tmp_olits2 = add_counter(tmp_ilits2);

    SizeType no{get_ln(ni)};
    vector<SatLiteral> olits(no);
    for ( SizeType i = 0; i < no; ++ i ) {
      olits[i] = new_variable(decision);
    }
    vector<SatLiteral> tmp_olits3(no - 1);
    for ( SizeType i = 0; i < (no - 1); ++ i ) {
      tmp_olits3[i] = olits[i];
    }
    add_adder(tmp_olits1, tmp_olits2, ilits[ni - 1], tmp_olits3, olits[no - 1]);
    return olits;
  }
}

END_NAMESPACE_YM_SAT
