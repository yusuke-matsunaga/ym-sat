
/// @file SatSolver.cc
/// @brief SatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolver.h"
#include "SatSolverImpl.h"
#include "SatLogger.h"
#include <sys/time.h>
#include <signal.h>


BEGIN_NAMESPACE_YM_SAT

const SatLiteral SatLiteral::X;

//////////////////////////////////////////////////////////////////////
// SatSolver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatSolver::SatSolver(
  const SatSolverType& solver_type
) : mType{solver_type},
    mImpl{SatSolverImpl::new_impl(mType)},
    mLogger{SatLogger::new_impl(mType)}
{
}

// @brief デストラクタ
SatSolver::~SatSolver()
{
  // SatSolverImpl.h を SatSolver.h にインクルードさせないために
  // デストラクタは default 定義できない．
}

// @brief 変数を追加する．
SatLiteral
SatSolver::new_variable(
  bool decision
)
{
  auto varid = mImpl->new_variable(decision);
  auto lit = SatLiteral::conv_from_varid(varid, false);
  mImpl->reg_lit(varid, lit);

  mLogger->new_variable(lit);

  ++ mVariableNum;

  return lit;
}

BEGIN_NONAMESPACE

SatSolver* the_solver;
bool expired;

void
stop_solver(int)
{
  the_solver->stop();
  expired = true;
}

END_NONAMESPACE

// @brief assumption 付きの SAT 問題を解く．
SatBool3
SatSolver::solve(
  const vector<SatLiteral>& assumptions,
  int time_limit
)
{
  sig_t old_func = nullptr;
  expired = false;

  if ( time_limit > 0 ) {
    // インターバルタイマをセットする．
    the_solver = this;
    old_func = signal(SIGALRM, stop_solver);

    struct itimerval itimer_val;
    itimer_val.it_interval.tv_sec = 0;
    itimer_val.it_interval.tv_usec = 0;
    itimer_val.it_value.tv_sec = time_limit;
    itimer_val.it_value.tv_usec = 0;

    setitimer(ITIMER_REAL, &itimer_val, nullptr);
  }

  mLogger->solve(assumptions);

  SatBool3 stat = mImpl->solve(assumptions, mModel, mConflictLiterals);
  if ( expired ) {
    stat = SatBool3::X;
  }

  if ( time_limit > 0 ) {
    signal(SIGALRM, old_func);
  }

  return stat;
}

// @brief 探索を中止する．
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
SatStats
SatSolver::get_stats() const
{
  return mImpl->get_stats();
}

// @brief DIMACS 形式で制約節を出力する．
void
SatSolver::write_DIMACS(
  ostream& s
) const
{
  s << "p cnf " << variable_num() << " " << clause_num() << endl;
  for ( auto clause: mClauseList ) {
    for ( auto lit: clause ) {
      auto var = lit.varid();
      int idx = var + 1;
      if ( lit.is_negative() ) {
	s << " -";
      }
      else {
	s << " ";
      }
      s << idx;
    }
    s << " 0" << endl;
  }
}

// @brief トータルの矛盾回数の制限を設定する．
SizeType
SatSolver::set_conflict_budget(
  SizeType val
)
{
  return mImpl->set_conflict_budget(val);
}

// @brief トータルの implication 回数の制限を設定する．
SizeType
SatSolver::set_propagation_budget(
  SizeType val
)
{
  return mImpl->set_propagation_budget(val);
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
void
SatSolver::reg_msg_handler(
  SatMsgHandler* msg_handler
)
{
  mImpl->reg_msg_handler(msg_handler);
}

// @brief 時間計測機能を制御する
void
SatSolver::timer_on(
  bool enable
)
{
  mImpl->timer_on(enable);
}

// @brief set_conditional_literal() の下請け関数
void
SatSolver::_set_conditional_literals(
  SizeType n,
  SatLiteral lits[]
)
{
  mConditionalLits.clear();
  mConditionalLits.resize(n);
  for ( int i = 0; i < n; ++ i ) {
    mConditionalLits[i] = lits[i];
  }
}

// @brief add_clause() の下請け関数
void
SatSolver::_add_clause(
  const vector<SatLiteral>& lits
)
{
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(lits.size() + mConditionalLits.size());
  for ( auto l: mConditionalLits ) {
    // 条件リテラルは反転する．
    tmp_lits.push_back(~l);
  }
  for ( auto l: lits ) {
    tmp_lits.push_back(l);
  }

  _add_clause_sub(tmp_lits);
}

// @brief add_clause() の下請け関数
void
SatSolver::_add_clause(
  SizeType n,
  SatLiteral lits[]
)
{
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n + mConditionalLits.size());
  for ( auto l: mConditionalLits ) {
    // 条件リテラルは反転する．
    tmp_lits.push_back(~l);
  }
  for ( int i = 0; i < n; ++ i ) {
    tmp_lits.push_back(lits[i]);
  }

  _add_clause_sub(tmp_lits);
}

// @brief _add_clause() の下請け関数
void
SatSolver::_add_clause_sub(
  const vector<SatLiteral>& lits
)
{
  mClauseList.push_back(lits);

  mLiteralNum += lits.size();

  mImpl->add_clause(lits);

  mLogger->add_clause(lits);
}

END_NAMESPACE_YM_SAT
