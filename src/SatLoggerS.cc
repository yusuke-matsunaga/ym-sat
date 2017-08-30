
/// @file SatLoggerS.cc
/// @brief SatLoggerS の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
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

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatLoggerS::add_clause(const vector<SatLiteral>& lits)
{
  *mS << "A";
  for (vector<SatLiteral>::const_iterator p = lits.begin();
       p != lits.end(); ++ p) {
    SatLiteral l = *p;
    put_lit(l);
  }
  *mS << endl;
}

// @brief 1項の節(リテラル)を追加する．
void
SatLoggerS::add_clause(SatLiteral lit1)
{
  *mS << "A";
  put_lit(lit1);
  *mS << endl;
}

// @brief 2項の節を追加する．
void
SatLoggerS::add_clause(SatLiteral lit1,
		       SatLiteral lit2)
{
  *mS << "A";
  put_lit(lit1);
  put_lit(lit2);
  *mS << endl;
}

// @brief 3項の節を追加する．
void
SatLoggerS::add_clause(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3)
{
  *mS << "A";
  put_lit(lit1);
  put_lit(lit2);
  put_lit(lit3);
  *mS << endl;
}

// @brief 4項の節を追加する．
void
SatLoggerS::add_clause(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3,
		       SatLiteral lit4)
{
  *mS << "A";
  put_lit(lit1);
  put_lit(lit2);
  put_lit(lit3);
  put_lit(lit4);
  *mS << endl;
}

// @brief 5項の節を追加する．
void
SatLoggerS::add_clause(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3,
		       SatLiteral lit4,
		       SatLiteral lit5)
{
  *mS << "A";
  put_lit(lit1);
  put_lit(lit2);
  put_lit(lit3);
  put_lit(lit4);
  put_lit(lit5);
  *mS << endl;
}

// @brief 6項の節を追加する．
void
SatLoggerS::add_clause(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3,
		       SatLiteral lit4,
		       SatLiteral lit5,
		       SatLiteral lit6)
{
  *mS << "A";
  put_lit(lit1);
  put_lit(lit2);
  put_lit(lit3);
  put_lit(lit4);
  put_lit(lit5);
  put_lit(lit6);
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
  for (vector<SatLiteral>::const_iterator p = assumptions.begin();
       p != assumptions.end(); ++ p) {
    SatLiteral l = *p;
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
