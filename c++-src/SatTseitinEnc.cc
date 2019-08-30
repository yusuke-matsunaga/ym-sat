
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

END_NAMESPACE_YM_SAT
