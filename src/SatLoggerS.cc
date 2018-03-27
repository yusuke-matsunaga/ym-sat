
/// @file SatLoggerS.cc
/// @brief SatLoggerS の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SatLoggerS.h"
#include "ym/SatVarId.h"
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
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
void
SatLoggerS::new_variable(SatVarId id)
{
  *mS << "N" << endl
      << "# varid = " << id << endl;
}

// @brief 条件リテラルを設定する．
// @param[in] lits リテラルのベクタ
void
SatLoggerS::set_conditional_literals(const vector<SatLiteral>& lits)
{
  mCondLits.clear();
  int n = lits.size();
  mCondLits.resize(n);
  for ( int i = 0; i < n; ++ i ) {
    mCondLits[i] = lits[i];
  }
}

// @brief 条件リテラルを設定する．
// @param[in] n_lits リテラル数
// @param[in] lits リテラルの配列
void
SatLoggerS::set_conditional_literals(int n_lits,
				    const SatLiteral* lits)
{
  mCondLits.clear();
  mCondLits.resize(n_lits);
  for ( int i = 0; i < n_lits; ++ i ) {
    mCondLits[i] = lits[i];
  }
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
  for ( auto l: mCondLits ) {
    put_lit(~l);
  }
  *mS << endl;
}

// @brief 節を追加する．
// @param[in] n_lits リテラル数
// @param[in] lits リテラルの配列
void
SatLoggerS::add_clause(int n_lits,
		       const SatLiteral* lits)
{
  *mS << "A";
  for ( int i = 0; i < n_lits; ++ i ) {
    SatLiteral l = lits[i];
    put_lit(l);
  }
  for ( auto l: mCondLits ) {
    put_lit(~l);
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
