
/// @file SatTseitinEnc.cc
/// @brief SatTseitinEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatTseitinEnc.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatTseitinEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] solver SATソルバ
SatTseitinEnc::SatTseitinEnc(SatSolver& solver) :
  mSolver{solver}
{
}

// @brief デストラクタ
SatTseitinEnc::~SatTseitinEnc()
{
}

// @brief n入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatTseitinEnc::add_andgate(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits(n + 1);
  for ( int i = 0; i < n; ++ i ) {
    SatLiteral ilit{lit_list[i]};
    tmp_lits[i] = ~ilit;
    mSolver.add_clause(ilit, ~olit);
  }
  tmp_lits[n] = olit;
  mSolver.add_clause(tmp_lits);
}

// @brief n入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatTseitinEnc::add_orgate(SatLiteral olit,
			  const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits(n + 1);
  for ( int i = 0; i < n; ++ i ) {
    SatLiteral ilit{lit_list[i]};
    mSolver.add_clause(~ilit, olit);
    tmp_lits[i] = ilit;
  }
  tmp_lits[n] = ~olit;
  mSolver.add_clause(tmp_lits);
}

// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatTseitinEnc::add_xorgate(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  int n_exp = 1 << n;
  vector<SatLiteral> tmp_lits(n + 1);
  for ( int p = 0; p < n_exp; ++ p ) {
    int c = 0;
    for ( int i = 0; i < n; ++ i ) {
      SatLiteral ilit{lit_list[i]};
      if ( p & (1 << i) ) {
	tmp_lits[i] = ~ilit;
	++ c;
      }
      else {
	tmp_lits[i] =  ilit;
      }
    }
    if ( (c % 2) == 1 ) {
      tmp_lits[n] =  olit;
    }
    else {
      tmp_lits[n] = ~olit;
    }
    mSolver.add_clause(tmp_lits);
  }
}

// @brief half_adder の入出力の関係を表す条件を追加する．
// @param[in] alit, blit 入力のリテラル
// @param[in] slit 和の出力のリテラル
// @param[in] olit キャリー出力のリテラル
void
SatTseitinEnc::add_half_adder(SatLiteral alit,
			      SatLiteral blit,
			      SatLiteral slit,
			      SatLiteral olit)
{
  mSolver.add_clause(~slit,  alit,  blit);
  mSolver.add_clause( slit,  alit, ~blit);
  mSolver.add_clause( slit, ~alit,  blit);
  mSolver.add_clause(~slit, ~alit, ~blit);
  mSolver.add_clause(~olit,  alit       );
  mSolver.add_clause(~olit,         blit);
  mSolver.add_clause( olit, ~alit, ~blit);
}

// @brief full_adder の入出力の関係を表す条件を追加する．
// @param[in] alit, blit 入力のリテラル
// @param[in] ilit キャリー入力のリテラル
// @param[in] slit 和の出力のリテラル
// @param[in] olit キャリー出力のリテラル
void
SatTseitinEnc::add_full_adder(SatLiteral alit,
			     SatLiteral blit,
			     SatLiteral ilit,
			     SatLiteral slit,
			     SatLiteral olit)
{
  mSolver.add_clause(~slit,  alit,  blit,  ilit);
  mSolver.add_clause( slit,  alit,  blit, ~ilit);
  mSolver.add_clause( slit,  alit, ~blit,  ilit);
  mSolver.add_clause(~slit,  alit, ~blit, ~ilit);
  mSolver.add_clause( slit, ~alit,  blit,  ilit);
  mSolver.add_clause(~slit, ~alit,  blit, ~ilit);
  mSolver.add_clause(~slit, ~alit, ~blit,  ilit);
  mSolver.add_clause( slit, ~alit, ~blit, ~ilit);
  mSolver.add_clause(~olit,  alit,  blit       );
  mSolver.add_clause(~olit,  alit,         ilit);
  mSolver.add_clause(~olit,         blit,  ilit);
  mSolver.add_clause( olit, ~alit, ~blit       );
  mSolver.add_clause( olit, ~alit,        ~ilit);
  mSolver.add_clause( olit,        ~blit, ~ilit);
}

// @brief 多ビットadderの入出力の関係を表す条件を追加する．
// @param[in] alits, blits 入力のリテラル
// @param[in] ilit キャリー入力のリテラル
// @param[in] slits 和の出力のリテラル
// @param[in] olit キャリー出力のリテラル
void
SatTseitinEnc::add_adder(const vector<SatLiteral>& alits,
			 const vector<SatLiteral>& blits,
			 SatLiteral ilit,
			 const vector<SatLiteral>& slits,
			 SatLiteral olit)
{
  int na = alits.size();
  int nb = blits.size();
  int ns = slits.size();
  ASSERT_COND( na <= ns );
  ASSERT_COND( nb <= ns );

  bool a_zero = false;
  bool b_zero = false;
  bool c_zero = false;
  for ( int i = 0; i < ns; ++ i ) {
    auto slit{slits[i]};
    auto alit{kSatLiteralX};
    auto blit{kSatLiteralX};
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
	mSolver.add_clause(~slit);
      }
      else {
	add_buffgate(ilit, slit);
	c_zero = true;
      }
      if ( i == (ns - 1) ) {
	mSolver.add_clause(~olit);
      }
    }
    else if ( a_zero ) { // !b_zero
      auto olit1{i < (ns - 1) ? mSolver.new_variable(false) : olit};
      add_half_adder(blit, ilit, slit, olit1);
      ilit = olit1;
    }
    else if ( b_zero ) { // !a_zero
      auto olit1{i < (ns - 1) ? mSolver.new_variable(false) : olit};
      add_half_adder(alit, ilit, slit, olit1);
      ilit = olit1;
    }
    else { // !a_zero && !b_zero
      auto olit1{i < (ns - 1) ? mSolver.new_variable(false) : olit};
      add_full_adder(alit, blit, ilit, slit, olit1);
      ilit = olit1;
    }
  }
}

END_NAMESPACE_YM_SAT
