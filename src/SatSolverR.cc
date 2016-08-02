
/// @file SatSolverR.cc
/// @brief SatSolverR の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatSolverR.h"

#include "ymsat/YmSatMS2.h"
#include "MiniSat/SatSolverMiniSat.h"
#include "MiniSat2/SatSolverMiniSat2.h"
#include "glueminisat-2.2.8/SatSolverGlueMiniSat2.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// SatSolver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] type 実装タイプを表す文字列
// @param[in] option オプション文字列
// @param[in] rec_out ログを記録するストリームへのポインタ
SatSolverR::SatSolverR(const string& type,
		       const string& option,
		       ostream* rec_out) :
  SatSolver(type, option),
  mRecOut(rec_out)
{
}

// @brief デストラクタ
SatSolverR::~SatSolverR()
{
}

// @brief 変数を追加する．
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
SatVarId
SatSolverR::new_var(bool decision)
{
  SatVarId id = SatSolver::new_var(decision);

  if ( mRecOut ) {
    *mRecOut << "N" << endl
	     << "# varid = " << id << endl;
  }

  return id;
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatSolverR::add_clause(const vector<SatLiteral>& lits)
{
  if ( mRecOut ) {
    *mRecOut << "A";
    for (vector<SatLiteral>::const_iterator p = lits.begin();
	 p != lits.end(); ++ p) {
      SatLiteral l = *p;
      put_lit(l);
    }
    *mRecOut << endl;
  }

  SatSolver::add_clause(lits);
}

// @brief 1項の節を追加する．
void
SatSolverR::add_clause(SatLiteral lit1)
{
  if ( mRecOut ) {
    *mRecOut << "A";
    put_lit(lit1);
    *mRecOut << endl;
  }

  SatSolver::add_clause(lit1);
}

// @brief 2項の節を追加する．
void
SatSolverR::add_clause(SatLiteral lit1,
		       SatLiteral lit2)
{
  if ( mRecOut ) {
    *mRecOut << "A";
    put_lit(lit1);
    put_lit(lit2);
    *mRecOut << endl;
  }

  SatSolver::add_clause(lit1, lit2);
}

// @brief 3項の節を追加する．
void
SatSolverR::add_clause(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3)
{
  if ( mRecOut ) {
    *mRecOut << "A";
    put_lit(lit1);
    put_lit(lit2);
    put_lit(lit3);
    *mRecOut << endl;
  }

  SatSolver::add_clause(lit1, lit2, lit3);
}

// @brief 4項の節を追加する．
void
SatSolverR::add_clause(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3,
		       SatLiteral lit4)
{
  if ( mRecOut ) {
    *mRecOut << "A";
    put_lit(lit1);
    put_lit(lit2);
    put_lit(lit3);
    put_lit(lit4);
    *mRecOut << endl;
  }

  SatSolver::add_clause(lit1, lit2, lit3, lit4);
}

// @brief 5項の節を追加する．
void
SatSolverR::add_clause(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3,
		       SatLiteral lit4,
		       SatLiteral lit5)
{
  if ( mRecOut ) {
    *mRecOut << "A";
    put_lit(lit1);
    put_lit(lit2);
    put_lit(lit3);
    put_lit(lit4);
    put_lit(lit5);
    *mRecOut << endl;
  }

  SatSolver::add_clause(lit1, lit2, lit3, lit4, lit5);
}

// @brief assumption 付きの SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @retval kSat 充足した．
// @retval kUnsat 充足不能が判明した．
// @retval kUndet わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
SatSolverR::solve(const vector<SatLiteral>& assumptions,
		  vector<SatBool3>& model)
{
  if ( mRecOut ) {
    *mRecOut << "S";
    for (vector<SatLiteral>::const_iterator p = assumptions.begin();
	 p != assumptions.end(); ++ p) {
      SatLiteral l = *p;
      put_lit(l);
    }
    *mRecOut << endl;
  }

  SatBool3 stat = SatSolver::solve(assumptions, model);

  return stat;
}

// @brief 学習節をすべて削除する．
void
SatSolverR::forget_learnt_clause()
{
  if ( mRecOut ) {
    *mRecOut << "F" << endl;
  }

  SatSolver::forget_learnt_clause();
}

// @brief リテラルを出力する．
void
SatSolverR::put_lit(SatLiteral lit) const
{
  ASSERT_COND( mRecOut != nullptr );

  *mRecOut << " " << lit.varid();
  if ( lit.is_positive() ) {
    *mRecOut << "P";
  }
  else {
    *mRecOut << "N";
  }
}

END_NAMESPACE_YM_SAT
