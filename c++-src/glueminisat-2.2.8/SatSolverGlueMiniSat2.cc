﻿
/// @file SatSolverGlueMiniSat2.cc
/// @brief SatSolverGlueMiniSat2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatSolverGlueMiniSat2.h"
#include "ym/SatStats.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_YM_SAT

using namespace Glueminisat;

BEGIN_NONAMESPACE

inline
Lit
literal2lit(
  SatLiteral l
)
{
  return mkLit(static_cast<Var>(l.varid()), l.is_negative());
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SatSolverGlueMiniSat2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatSolverGlueMiniSat2::SatSolverGlueMiniSat2(
  const Json::Value& js_obj
)
{
  if ( js_obj.isMember("verbose") ) {
    mSolver.verbosity = js_obj["verbose"].asBool();
  }
}

// @brief デストラクタ
SatSolverGlueMiniSat2::~SatSolverGlueMiniSat2()
{
}

// @brief 正しい状態のときに true を返す．
bool
SatSolverGlueMiniSat2::sane() const
{
  return mSolver.okay();
}

// @brief 変数を追加する．
SatVarId
SatSolverGlueMiniSat2::new_variable(
  bool decision
)
{
  return mSolver.newVar(true, decision);
}

// @brief 節を追加する．
void
SatSolverGlueMiniSat2::add_clause(
  const vector<SatLiteral>& lits
)
{
  vec<Lit> tmp;
  for ( auto l: lits ) {
    auto lit = literal2lit(l);
    tmp.push(lit);
  }
  mSolver.addClause_(tmp);
}

// @brief SAT 問題を解く．
SatBool3
SatSolverGlueMiniSat2::solve(
  const vector<SatLiteral>& assumptions,
  SatModel& model,
  vector<SatLiteral>& conflicts
)
{
  vec<Lit> tmp;
  for ( auto l: assumptions ) {
    auto lit = literal2lit(l);
    tmp.push(lit);
  }

  mSolver.conflicts = 0;
  mSolver.decisions = 0;
  mSolver.propagations = 0;

  bool ans = mSolver.solve(tmp);
  if ( ans ) {
    SizeType n = mSolver.model.size();
    model.resize(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      lbool lb = mSolver.model[i];
      SatBool3 val;
      if ( lb == l_True ) {
	val = SatBool3::True;
      }
      else if ( lb == l_False ) {
	val = SatBool3::False;
      }
      else {
	val = SatBool3::X;
      }
      model.set(i, val);
    }
    return SatBool3::True;
  }
  else {
    SizeType n = mSolver.conflict.size();
    conflicts.resize(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      auto lit = mSolver.conflict[i];
      SizeType vid = var(lit);
      bool inv = sign(lit);
      conflicts[i] = get_lit(vid, inv);
    }
    return SatBool3::False;
  }
}

// @brief 探索を中止する．
void
SatSolverGlueMiniSat2::stop()
{
  mSolver.interrupt();
}

// @brief トータルの矛盾回数の制限を設定する．
SizeType
SatSolverGlueMiniSat2::set_conflict_budget(
  SizeType val
)
{
  mSolver.setConfBudget(val);
  return 0;
}

// @brief トータルの implication 回数の制限を設定する．
SizeType
SatSolverGlueMiniSat2::set_propagation_budget(
  SizeType val
)
{
  mSolver.setPropBudget(val);
  return 0;
}

// @brief 現在の内部状態を得る．
SatStats
SatSolverGlueMiniSat2::get_stats() const
{
  SatStats stats;
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
  return stats;
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
void
SatSolverGlueMiniSat2::reg_msg_handler(
  SatMsgHandler* msg_handler
)
{
}

// @brief 時間計測機能を制御する
void
SatSolverGlueMiniSat2::timer_on(
  bool enable
)
{
}

END_NAMESPACE_YM_SAT
