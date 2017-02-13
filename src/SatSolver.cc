﻿
/// @file SatSolver.cc
/// @brief SatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatSolver.h"

#include "ymsat/YmSatMS2.h"
#include "ymsat/YmSat1.h"
#include "ymsat1/YmSat.h"
#include "ymsat_old/YmSatMS2.h"
#include "MiniSat/SatSolverMiniSat.h"
#include "MiniSat2/SatSolverMiniSat2.h"
#include "glueminisat-2.2.8/SatSolverGlueMiniSat2.h"

#include "SatLogger.h"
#include "SatLoggerS.h"

#include "ym/CombiGen.h"


BEGIN_NAMESPACE_YM

const SatVarId kSatVarIdIllegal;
const SatLiteral kSatLiteralX;

END_NAMESPACE_YM

BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// SatSolver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] type 実装タイプを表す文字列
// @param[in] option オプション文字列
// @param[in] rec_out ログを記録するストリームへのポインタ
SatSolver::SatSolver(const string& type,
		     const string& option,
		     ostream* rec_out)
{
  if ( type == "minisat" ) {
    // minisat-1.4
    mImpl = new SatSolverMiniSat(option);
  }
  else if ( type == "minisat2" ) {
    // minisat-2.2
    mImpl = new SatSolverMiniSat2(option);
  }
  else if ( type == "glueminisat2" ) {
    // glueminisat-2.2.8
    mImpl = new SatSolverGlueMiniSat2(option);
  }
  else if ( type == "ymsat1" ) {
    mImpl = new YmSat1(option);
  }
  else if ( type == "ymsat2" ) {
    mImpl = new YmSatMS2(option);
  }
  else if ( type == "ymsat2old" ) {
    mImpl = new nsSatold::YmSatMS2(option);
  }
  else if ( type == "ymsat1_old" ) {
    mImpl = new nsSat1::YmSat(option);
  }
  else {
    mImpl = new YmSatMS2(option);
  }

  if ( rec_out ) {
    mLogger = new SatLoggerS(rec_out);
  }
  else {
    mLogger = new SatLogger();
  }
}

// @brief デストラクタ
SatSolver::~SatSolver()
{
  delete mImpl;
  delete mLogger;
}

// @brief 変数を追加する．
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
SatVarId
SatSolver::new_var(bool decision)
{
  SatVarId id = mImpl->new_var(decision);

  mLogger->new_var(id);

  return id;
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatSolver::add_clause(const vector<SatLiteral>& lits)
{
  mImpl->add_clause(lits);

  mLogger->add_clause(lits);
}

// @brief 1項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1)
{
  mImpl->add_clause(1, &lit1);

  mLogger->add_clause(lit1);
}

// @brief 2項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2)
{
  mImpl->add_clause(lit1, lit2);

  mLogger->add_clause(lit1, lit2);
}

// @brief 3項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3)
{
  mImpl->add_clause(lit1, lit2, lit3);

  mLogger->add_clause(lit1, lit2, lit3);
}

// @brief 4項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4)
{
  mImpl->add_clause(lit1, lit2, lit3, lit4);

  mLogger->add_clause(lit1, lit2, lit3, lit4);
}

// @brief 5項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4,
		      SatLiteral lit5)
{
  mImpl->add_clause(lit1, lit2, lit3, lit4, lit5);

  mLogger->add_clause(lit1, lit2, lit3, lit4, lit5);
}

// @brief n入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_andgate_rel(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  ymuint n = lit_list.size();
  vector<SatLiteral> tmp_lits(n + 1);
  for (ymuint i = 0; i < n; ++ i) {
    SatLiteral ilit = lit_list[i];
    tmp_lits[i] = ~ilit;
    add_clause(ilit, ~olit);
  }
  tmp_lits[n] = olit;
  add_clause(tmp_lits);
}

// @brief n入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_orgate_rel(SatLiteral olit,
			  const vector<SatLiteral> lit_list)
{
  ymuint n = lit_list.size();
  vector<SatLiteral> tmp_lits(n + 1);
  for (ymuint i = 0; i < n; ++ i) {
    SatLiteral ilit = lit_list[i];
    add_clause(~ilit, olit);
    tmp_lits[i] = ilit;
  }
  tmp_lits[n] = ~olit;
  add_clause(tmp_lits);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_at_most_one(const vector<SatLiteral>& lit_list)
{
  ymuint n = lit_list.size();
  for (ymuint i1 = 0; i1 < n - 1; ++ i1) {
    SatLiteral lit1 = lit_list[i1];
    for (ymuint i2 = i1 + 1; i2 < n; ++ i2) {
      SatLiteral lit2 = lit_list[i2];
      add_clause(~lit1, ~lit2);
    }
  }
}

// @brief 与えられたリテラルのうちk個しか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatSolver::add_at_most_k(const vector<SatLiteral>& lit_list,
			 ymuint k)
{
  ymuint n = lit_list.size();
  if ( k >= n ) {
    // はじめから条件は満たされている．
    return;
  }

  vector<SatLiteral> tmp_lits(k + 1);
  for (CombiGen cg(n, k + 1) ; !cg.is_end(); ++ cg) {
    for (ymuint i = 0; i <= k; ++ i) {
      tmp_lits[i] = ~lit_list[cg(i)];
    }
    add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうちk個以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatSolver::add_at_least_k(const vector<SatLiteral>& lit_list,
			  ymuint k)
{
  ymuint n = lit_list.size();
  if ( k == 0 ) {
    // はじめから条件は満たされている．
    return;
  }

  ymuint nk = n - k;
  vector<SatLiteral> tmp_lits(nk + 1);
  for (CombiGen cg(n, nk + 1); !cg.is_end(); ++ cg) {
    for (ymuint i = 0; i <= nk; ++ i) {
      tmp_lits[i] = lit_list[cg(i)];
    }
    add_clause(tmp_lits);
  }
}

// @brief assumption 付きの SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
// @retval kSat 充足した．
// @retval kUnsat 充足不能が判明した．
// @retval kUndet わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
SatSolver::solve(const vector<SatLiteral>& assumptions,
		 vector<SatBool3>& model,
		 vector<SatLiteral>& conflicts)
{
  mLogger->solve(assumptions);

  return mImpl->solve(assumptions, model, conflicts);
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
SatSolver::stop()
{
  mImpl->stop();
}

// @brief 正しい状態のときに true を返す．
bool
SatSolver::sane() const
{
  return mImpl->sane();
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
SatSolver::get_stats(SatStats& stats) const
{
  mImpl->get_stats(stats);
}

// @brief 変数の数を得る．
ymuint
SatSolver::variable_num() const
{
  return mImpl->variable_num();
}

// @brief 制約節の数を得る．
ymuint
SatSolver::clause_num() const
{
  return mImpl->clause_num();
}

// @brief 制約節のリテラルの総数を得る．
ymuint
SatSolver::literal_num() const
{
  return mImpl->literal_num();
}

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
SatSolver::write_DIMACS(ostream& s) const
{
  mImpl->write_DIMACS(s);
}

// @brief conflict_limit の最大値
// @param[in] val 設定する値
// @return 以前の設定値を返す．
ymuint64
SatSolver::set_max_conflict(ymuint64 val)
{
  return mImpl->set_max_conflict(val);
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
SatSolver::reg_msg_handler(SatMsgHandler* msg_handler)
{
  mImpl->reg_msg_handler(msg_handler);
}

// @brief 時間計測機能を制御する
void
SatSolver::timer_on(bool enable)
{
  mImpl->timer_on(enable);
}


//////////////////////////////////////////////////////////////////////
// クラス SatSolverImpl
//
// ここでは add_clause() のバリエーションのデフォルト実装を
// 提供している．
//////////////////////////////////////////////////////////////////////

// @brief 1項の節を追加する．
void
SatSolverImpl::add_clause(SatLiteral lit1)
{
  add_clause(1, &lit1);
}

// @brief 2項の節を追加する．
void
SatSolverImpl::add_clause(SatLiteral lit1,
			  SatLiteral lit2)
{
  SatLiteral tmp_lits[2] = {lit1, lit2};
  add_clause(2, tmp_lits);
}

// @brief 3項の節を追加する．
void
SatSolverImpl::add_clause(SatLiteral lit1,
			  SatLiteral lit2,
			  SatLiteral lit3)
{
  SatLiteral tmp_lits[3] = {lit1, lit2, lit3};
  add_clause(3, tmp_lits);
}

// @brief 4項の節を追加する．
void
SatSolverImpl::add_clause(SatLiteral lit1,
			  SatLiteral lit2,
			  SatLiteral lit3,
			  SatLiteral lit4)
{
  SatLiteral tmp_lits[4] = {lit1, lit2, lit3, lit4};
  add_clause(4, tmp_lits);
}

// @brief 5項の節を追加する．
void
SatSolverImpl::add_clause(SatLiteral lit1,
			  SatLiteral lit2,
			  SatLiteral lit3,
			  SatLiteral lit4,
			  SatLiteral lit5)
{
  SatLiteral tmp_lits[5] = {lit1, lit2, lit3, lit4, lit5};
  add_clause(5, tmp_lits);
}

END_NAMESPACE_YM_SAT
