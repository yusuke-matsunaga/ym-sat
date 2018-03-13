﻿
/// @file SatSolverMiniSat2.cc
/// @brief SatSolverMiniSat2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatSolverMiniSat2.h"
#include "ym/SatStats.h"


BEGIN_NAMESPACE_YM_SAT

using namespace Minisat;

BEGIN_NONAMESPACE

inline
Lit
literal2lit(SatLiteral l)
{
  return mkLit(static_cast<Var>(l.varid().val()), l.is_negative());
}

inline
SatLiteral
lit2literal(Lit lit)
{
  int vid = var(lit);
  bool s = sign(lit);
  return SatLiteral(SatVarId(vid), s);
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SatSolverMiniSat2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] option オプション文字列
SatSolverMiniSat2::SatSolverMiniSat2(const string& option)
{
  if ( option == "verbose" ) {
    mSolver.verbosity = 1;
  }
}

// @brief デストラクタ
SatSolverMiniSat2::~SatSolverMiniSat2()
{
}

// @brief 正しい状態のときに true を返す．
bool
SatSolverMiniSat2::sane() const
{
  return mSolver.okay();
}

// @brief 変数を追加する．
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
SatVarId
SatSolverMiniSat2::new_variable(bool decision)
{
  return SatVarId(mSolver.newVar(true, decision));
}

// @brief 条件リテラルを設定する．
// @param[in] lit_list 条件リテラルのリスト
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
void
SatSolverMiniSat2::set_conditional_literals(const vector<SatLiteral>& lit_list)
{
  mCondLits.clear();
  ymuint lit_num = lit_list.size();
  mCondLits.resize(lit_num);
  for (ymuint i = 0; i < lit_num; ++ i) {
    mCondLits[i] = lit_list[i];
  }
}

// @brief 条件リテラルを設定する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
void
SatSolverMiniSat2::set_conditional_literals(ymuint lit_num,
					    const SatLiteral* lits)
{
  mCondLits.clear();
  mCondLits.resize(lit_num);
  for (ymuint i = 0; i < lit_num; ++ i) {
    mCondLits[i] = lits[i];
  }
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatSolverMiniSat2::add_clause(const vector<SatLiteral>& lits)
{
  vec<Lit> tmp;
  for (vector<SatLiteral>::const_iterator p = lits.begin();
       p != lits.end(); ++ p) {
    SatLiteral l = *p;
    Lit lit = literal2lit(l);
    tmp.push(lit);
  }
  for (vector<SatLiteral>::const_iterator p = mCondLits.begin();
       p != mCondLits.end(); ++ p) {
    SatLiteral l = *p;
    // 極性が反転することに注意
    Lit lit = literal2lit(l);
    tmp.push(~lit);
  }
  mSolver.addClause_(tmp);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
SatSolverMiniSat2::add_clause(ymuint lit_num,
			      const SatLiteral* lits)
{
  vec<Lit> tmp;
  for (ymuint i = 0; i < lit_num; ++ i) {
    SatLiteral l = lits[i];
    Lit lit = literal2lit(l);
    tmp.push(lit);
  }
  for (vector<SatLiteral>::const_iterator p = mCondLits.begin();
       p != mCondLits.end(); ++ p) {
    SatLiteral l = *p;
    // 極性が反転することに注意
    Lit lit = literal2lit(l);
    tmp.push(~lit);
  }
  mSolver.addClause_(tmp);
}

// @brief SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
// @retval SatBool3::True 充足した．
// @retval SatBool3::False 充足不能が判明した．
// @retval SatBool3::X わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
SatSolverMiniSat2::solve(const vector<SatLiteral>& assumptions,
			 vector<SatBool3>& model,
			 vector<SatLiteral>& conflicts)
{
  vec<Lit> tmp;
  for (vector<SatLiteral>::const_iterator p = assumptions.begin();
       p != assumptions.end(); ++ p) {
    SatLiteral l = *p;
    Lit lit = literal2lit(l);
    tmp.push(lit);
  }

  mSolver.conflicts = 0;
  mSolver.decisions = 0;
  mSolver.propagations = 0;

  bool ans = mSolver.solve(tmp);
  if ( ans ) {
    ymuint n = mSolver.model.size();
    model.resize(n);
    for (ymuint i = 0; i < n; ++ i) {
      lbool lb = mSolver.model[i];
      if ( lb == l_True ) {
	model[i] = SatBool3::True;
      }
      else if ( lb == l_False ) {
	model[i] = SatBool3::False;
      }
      else {
	model[i] = SatBool3::X;
      }
    }
    return SatBool3::True;
  }
  else {
    ymuint n = mSolver.conflict.size();
    conflicts.resize(n);
    for (ymuint i = 0; i < n; ++ i) {
      Lit lit = mSolver.conflict[i];
      conflicts[i] = lit2literal(lit);
    }
    return SatBool3::False;
  }
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
SatSolverMiniSat2::stop()
{
  mSolver.interrupt();
}

// @brief conflict_limit の最大値
// @param[in] val 設定する値
// @return 以前の設定値を返す．
ymuint64
SatSolverMiniSat2::set_max_conflict(ymuint64 val)
{
  // 無効
  return 0;
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
SatSolverMiniSat2::get_stats(SatStats& stats) const
{
  stats.mRestart = mSolver.starts;
  stats.mVarNum = mSolver.nVars();
  stats.mConstrClauseNum = mSolver.nClauses();
  stats.mConstrLitNum = mSolver.clauses_literals;
  stats.mLearntClauseNum = mSolver.nLearnts();
  stats.mLearntLitNum = mSolver.learnts_literals;
  stats.mConflictNum = mSolver.conflicts;
  stats.mDecisionNum = mSolver.decisions;
  stats.mPropagationNum = mSolver.propagations;
  stats.mConflictLimit = 0;
  stats.mLearntLimit = 0;
}

// @brief 変数の数を得る．
ymuint
SatSolverMiniSat2::variable_num() const
{
  return mSolver.nVars();
}

// @brief 制約節の数を得る．
ymuint
SatSolverMiniSat2::clause_num() const
{
  return mSolver.nClauses();
}

// @brief 制約節のリテラルの総数を得る．
ymuint
SatSolverMiniSat2::literal_num() const
{
  return mSolver.clauses_literals;
}

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
SatSolverMiniSat2::write_DIMACS(ostream& s) const
{
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
SatSolverMiniSat2::reg_msg_handler(SatMsgHandler* msg_handler)
{
}

// @brief 時間計測機能を制御する
void
SatSolverMiniSat2::timer_on(bool enable)
{
}

END_NAMESPACE_YM_SAT
