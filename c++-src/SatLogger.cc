
/// @file SatLogger.cc
/// @brief SatLogger の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SatLogger.h"
#include "SatLoggerS.h"
#include "ym/SatSolverType.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatLogger
//////////////////////////////////////////////////////////////////////

// @brief 新しいインスタンスを生成するクラスメソッド
// @param[in] solver_type SATソルバのタイプ
unique_ptr<SatLogger>
SatLogger::new_impl(const SatSolverType& solver_type)
{
  ostream* log_out = solver_type.log_out();
  if ( log_out ) {
    return unique_ptr<SatLogger>{new SatLoggerS(log_out)};
  }
  else {
    return unique_ptr<SatLogger>{new SatLogger()};
  }
}

// @brief コンストラクタ
SatLogger::SatLogger()
{
}

// @brief デストラクタ
SatLogger::~SatLogger()
{
}

// @brief 変数を追加する．
// @param[in] lit 変数を表すリテラル
void
SatLogger::new_variable(SatLiteral lit)
{
}

// @brief 条件リテラルを設定する．
// @param[in] lits リテラルのベクタ
void
SatLogger::set_conditional_literals(const vector<SatLiteral>& lits)
{
}

// @brief 条件リテラルを設定する．
// @param[in] n_lits リテラル数
// @param[in] lits リテラルの配列
void
SatLogger::set_conditional_literals(int n_lits,
				    const SatLiteral* lits)
{
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatLogger::add_clause(const vector<SatLiteral>& lits)
{
}

// @brief 節を追加する．
// @param[in] n_lits リテラル数
// @param[in] lits リテラルの配列
void
SatLogger::add_clause(int n_lits,
		      const SatLiteral* lits)
{
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
SatLogger::solve(const vector<SatLiteral>& assumptions)
{
}

END_NAMESPACE_YM_SAT
