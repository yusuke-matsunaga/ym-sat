
/// @file SatLoggerS.cc
/// @brief SatLoggerS の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SatLoggerS.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatLoggerS
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] s 出力先のストリーム
SatLoggerS::SatLoggerS(ostream* s) :
  mS(s)
{
}

// @brief デストラクタ
SatLoggerS::~SatLoggerS()
{
}

// @brief 変数を追加する．
// @param[in] lit 変数を表すリテラル
void
SatLoggerS::new_variable(SatLiteral lit)
{
  *mS << "N" << endl
      << "# var = " << lit << endl;
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatLoggerS::add_clause(const vector<SatLiteral>& lits)
{
  *mS << "A";
  for ( auto l: lits ) {
    put_lit(l);
  }
  *mS << endl;
}

// @brief assumption 付きの SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @retval kSat 充足した．
// @retval kUnsat 充足不能が判明した．
// @retval kUndet わからなかった．
//
// i 番めの変数の割り当て結果は model[i] に入る．
void
SatLoggerS::solve(const vector<SatLiteral>& assumptions)
{
  *mS << "S";
  for ( auto l: assumptions ) {
    put_lit(l);
  }
  *mS << endl;
}

// @brief リテラルを出力する．
void
SatLoggerS::put_lit(SatLiteral lit) const
{
  *mS << " " << lit.varid();
  if ( lit.is_positive() ) {
    *mS << "P";
  }
  else {
    *mS << "N";
  }
}

END_NAMESPACE_YM_SAT
