
/// @file SatSolverLingeling.cc
/// @brief SatSolverLingeling の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatSolverLingeling.h"
#include "ym/SatStats.h"
#include "ym/SatModel.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

BEGIN_NONAMESPACE

inline
int
translate(
  SatLiteral l
)
{
  int v = l.varid() + 1;
  return l.is_negative() ? -v : v;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SatSolverLingeling
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatSolverLingeling::SatSolverLingeling(
  const Json::Value& js_obj
) : mSolver{lglinit()},
    mNumVars{0}
{
}

// @brief デストラクタ
SatSolverLingeling::~SatSolverLingeling()
{
  lglrelease(mSolver);
}

// @brief 正しい状態のときに true を返す．
bool
SatSolverLingeling::sane() const
{
  // not implemented
  return true;
}

// @brief 変数を追加する．
SatVarId
SatSolverLingeling::new_variable(
  bool decision
)
{
  SatVarId var = mNumVars;
  ++ mNumVars;
  if ( decision ) {
    int lindex = static_cast<int>(var + 1);
    lglfreeze(mSolver, lindex);
  }
  return var;
}

// @brief 節を追加する．
void
SatSolverLingeling::add_clause(
  const vector<SatLiteral>& lits
)
{
  for ( auto l: lits ) {
    int x = translate(l);
    lgladd(mSolver, x);
  }
  lgladd(mSolver, 0);
}

// @brief SAT 問題を解く．
SatBool3
SatSolverLingeling::solve(
  const vector<SatLiteral>& assumptions,
  SatModel& model,
  vector<SatLiteral>& conflicts
)
{
  for ( auto l: assumptions ) {
    int x = translate(l);
    lglassume(mSolver, x);
  }

  lglsetopt(mSolver, "dlim", -1); // 何やってるか不明
  int result = lglsat(mSolver);
  if ( result == LGL_SATISFIABLE ) {
    model.resize(mNumVars);
    for ( int i: Range(mNumVars) ) {
      int v = lglderef(mSolver, i + 1);
      SatBool3 val;
      if ( v == 1) {
	val = SatBool3::True;
      }
      else if ( v == -1 ) {
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
    return SatBool3::False;
  }
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
SatSolverLingeling::stop()
{
  // not implemented
}

// @brief トータルの矛盾回数の制限を設定する．
SizeType
SatSolverLingeling::set_conflict_budget(
  SizeType val
)
{
  // 無効
  return 0;
}

// @brief トータルの implication 回数の制限を設定する．
SizeType
SatSolverLingeling::set_propagation_budget(
  SizeType val
)
{
  // 無効
  return 0;
}

// @brief 現在の内部状態を得る．
SatStats
SatSolverLingeling::get_stats() const
{
  SatStats stats;
#warning "TODO: 未実装"
#if 0
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
#endif
  return stats;
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
void
SatSolverLingeling::reg_msg_handler(
  SatMsgHandler* msg_handler
)
{
}

// @brief 時間計測機能を制御する
void
SatSolverLingeling::timer_on(
  bool enable
)
{
}

END_NAMESPACE_YM_SAT
