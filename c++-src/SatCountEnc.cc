
/// @file SatCountEnc.cc
/// @brief SatCountEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatCountEnc.h"
#include "ym/CombiGen.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatCountEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] solver SATソルバ
SatCountEnc::SatCountEnc(SatSolver& solver) :
  mSolver{solver}
{
}

// @brief デストラクタ
SatCountEnc::~SatCountEnc()
{
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::add_at_most_one(const vector<SatLiteral>& lit_list)
{
  int n = lit_list.size();
  if ( n <= 1 ) {
    // はじめから条件は満たされている．
    return;
  }
  for ( int i1: Range(n - 1) ) {
    SatLiteral lit1{lit_list[i1]};
    for ( int i2: Range(i1 + 1, n) ) {
      SatLiteral lit2{lit_list[i2]};
      mSolver.add_clause(~lit1, ~lit2);
    }
  }
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::add_at_most_two(const vector<SatLiteral>& lit_list)
{
  int n = lit_list.size();
  if ( n <= 2 ) {
    // はじめから条件は満たされている．
    return;
  }
  for ( int i1: Range(n - 1) ) {
    SatLiteral lit1{lit_list[i1]};
    for ( int i2: Range(i1 + 1, n) ) {
      SatLiteral lit2{lit_list[i2]};
      for ( int i3: Range(i2 + 1, n) ) {
	SatLiteral lit3{lit_list[i3]};
	mSolver.add_clause(~lit1, ~lit2, ~lit3);
      }
    }
  }
}

// @brief 与えられたリテラルのうちk個しか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatCountEnc::add_at_most_k(const vector<SatLiteral>& lit_list,
			 int k)
{
  int n = lit_list.size();
  if ( n <= k) {
    // はじめから条件は満たされている．
    return;
  }

  vector<SatLiteral> tmp_lits(k + 1);
  for ( CombiGen cg(n, k + 1) ; !cg.is_end(); ++ cg ) {
    for ( int i: Range(k + 1) ) {
      tmp_lits[i] = ~lit_list[cg(i)];
    }
    mSolver.add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::add_at_least_two(const vector<SatLiteral>& lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n - 1);
  for ( int i: Range(n) ) {
    tmp_lits.clear();
    for ( int j: Range(n) ) {
      SatLiteral lit{lit_list[j]};
      if ( i != j ) {
	tmp_lits.push_back(lit);
      }
    }
    mSolver.add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうちk個以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatCountEnc::add_at_least_k(const vector<SatLiteral>& lit_list,
			  int k)
{
  int n = lit_list.size();
  if ( k == 0 ) {
    // はじめから条件は満たされている．
    return;
  }

  int nk = n - k;
  vector<SatLiteral> tmp_lits(nk + 1);
  for ( CombiGen cg(n, nk + 1); !cg.is_end(); ++ cg ) {
    for ( int i: Range(nk + 1) ) {
      tmp_lits[i] = lit_list[cg(i)];
    }
    mSolver.add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit_lit 入力のリテラルのリスト
void
SatCountEnc::add_not_one(const vector<SatLiteral>& lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits(n);
  for ( int i: Range(n) ) {
    for ( int j: Range(n) ) {
      SatLiteral lit{lit_list[j]};
      if ( j == i ) {
	tmp_lits[j] = ~lit;
      }
      else {
	tmp_lits[j] =  lit;
      }
    }
    mSolver.add_clause(tmp_lits);
  }
}

END_NAMESPACE_YM_SAT
