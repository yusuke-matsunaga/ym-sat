
/// @file SatSolverMiniSat.cc
/// @brief SatSolverMiniSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatSolverMiniSat.h"
#include "ym/SatStats.h"
#include "ym/SatModel.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_YM_SAT

BEGIN_NONAMESPACE

inline
Lit
literal2lit(
  SatLiteral l
)
{
  return Lit(l.varid(), l.is_negative());
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SatSolverMiniSat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatSolverMiniSat::SatSolverMiniSat(
  const JsonValue& js_obj
)
{
  if ( js_obj.has_key("verbose") ) {
    mSolver.verbosity = js_obj["verbose"].get_bool();
  }
}

// @brief デストラクタ
SatSolverMiniSat::~SatSolverMiniSat()
{
}

// @brief 正しい状態のときに true を返す．
bool
SatSolverMiniSat::sane() const
{
  return mSolver.okay();
}

// @brief 変数を追加する．
SatLiteral
SatSolverMiniSat::new_variable(
  bool decision
)
{
  auto var = mSolver.newVar();
  return get_lit(var, false);
}

// @brief 節を追加する．
void
SatSolverMiniSat::add_clause(
  const vector<SatLiteral>& lits
)
{
  vec<Lit> tmp;
  for ( auto l: lits ) {
    auto lit = literal2lit(l);
    tmp.push(lit);
  }
  mSolver.addClause(tmp);
}

// @brief SAT 問題を解く．
SatBool3
SatSolverMiniSat::solve(
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
    // conflicts は未実装
    return SatBool3::False;
  }
}

// @brief 探索を中止する．
void
SatSolverMiniSat::stop()
{
  // 未実装
}

SizeType
SatSolverMiniSat::set_conflict_budget(
  SizeType val
)
{
  // 無効
  return 0;
}

SizeType
SatSolverMiniSat::set_propagation_budget(
  SizeType val
)
{
  // 無効
  return 0;
}

// @brief 現在の内部状態を得る．
SatStats
SatSolverMiniSat::get_stats() const
{
  SatStats stats;
  stats.mRestart = mSolver.stats.starts;
  stats.mVarNum = mSolver.nVars();
  stats.mConstrClauseNum = mSolver.nClauses();
  stats.mConstrLitNum = mSolver.stats.clauses_literals;
  stats.mLearntClauseNum = mSolver.nLearnts();
  stats.mLearntLitNum = mSolver.stats.learnts_literals;
  stats.mConflictNum = mSolver.stats.conflicts;
  stats.mDecisionNum = mSolver.stats.decisions;
  stats.mPropagationNum = mSolver.stats.propagations;
  stats.mConflictLimit = 0;
  stats.mLearntLimit = 0;
  return stats;
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
void
SatSolverMiniSat::reg_msg_handler(
  SatMsgHandler* msg_handler
)
{
}

// @brief 時間計測機能を制御する
void
SatSolverMiniSat::timer_on(bool enable)
{
}

END_NAMESPACE_YM_SAT
