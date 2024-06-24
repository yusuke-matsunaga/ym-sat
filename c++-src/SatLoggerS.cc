
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
void
SatLoggerS::solve(const vector<SatLiteral>& assumptions)
{
  *mS << "S";
  for ( auto l: assumptions ) {
    put_lit(l);
  }
  *mS << endl;
}

// @brief solve() の結果を出力する．
void
SatLoggerS::solve_result(
  SatBool3 res
)
{
  switch ( res ) {
  case SatBool3::False: *mS << "# -> UNSAT" << endl; break;
  case SatBool3::True:  *mS << "# -> SAT" << endl; break;
  case SatBool3::X:     *mS << "# -> ABORT" << endl; break;
  }
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
