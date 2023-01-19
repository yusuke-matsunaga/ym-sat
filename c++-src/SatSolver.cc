
/// @file SatSolver.cc
/// @brief SatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolver.h"

#include "ymsat/YmSatMS2.h"
#include "ymsat/YmSat1.h"
#include "ymsat1/YmSat.h"
#include "ymsat_old/YmSatMS2.h"
#include "MiniSat/SatSolverMiniSat.h"
#include "MiniSat2/SatSolverMiniSat2.h"
#include "glueminisat-2.2.8/SatSolverGlueMiniSat2.h"
#include "lingeling/SatSolverLingeling.h"

#include "SatLogger.h"

#include <sys/time.h>
#include <signal.h>


BEGIN_NAMESPACE_YM_SAT

const SatLiteral SatLiteral::X;

//////////////////////////////////////////////////////////////////////
// SatSolverImpl
//////////////////////////////////////////////////////////////////////

BEGIN_NONAMESPACE

// SatSolverType のデフォルト値を設定する．
SatSolverType
fallback_type(
  const SatSolverType& src_type
)
{
  string type = src_type.type();
  if ( type == "minisat" ) {
    // minisat-1.4
    ;
  }
  else if ( type == "minisat2" ) {
    // minisat-2.2
    ;
  }
  else if ( type == "glueminisat2" ) {
    // glueminisat-2.2.8
    ;
  }
  else if ( type == "lingeling" ) {
    ;
  }
  else if ( type == "ymsat1" ) {
    ;
  }
  else if ( type == "ymsat2" ) {
    ;
  }
  else if ( type == "ymsat2old" ) {
    ;
  }
  else if ( type == "ymsat1_old" ) {
    ;
  }
  else if ( type == "" ) {
    // lingeling 今はデフォルトにしている．
    type = "lingeling";
  }
  else {
    cerr << "SatSolver: unknown type '" << type << "', ymsat2 is used, instead." << endl;
    type = "lingeling";
  }
  return SatSolverType(type, src_type.option(), src_type.log_out());
}

END_NONAMESPACE

// @brief 継承クラスを作るクラスメソッド
// @param[in] solver_type SATソルバのタイプ
unique_ptr<SatSolverImpl>
SatSolverImpl::new_impl(
  const SatSolverType& solver_type
)
{
  const string& type = solver_type.type();
  const string& option = solver_type.option();
  if ( type == "minisat" ) {
    // minisat-1.4
    return unique_ptr<SatSolverImpl>(new SatSolverMiniSat(option));
  }
  else if ( type == "minisat2" ) {
    // minisat-2.2
    return unique_ptr<SatSolverImpl>(new SatSolverMiniSat2(option));
  }
  else if ( type == "glueminisat2" ) {
    // glueminisat-2.2.8
    return unique_ptr<SatSolverImpl>(new SatSolverGlueMiniSat2(option));
  }
  else if ( type == "lingeling" ) {
    // lingeling
    return unique_ptr<SatSolverImpl>(new SatSolverLingeling(option));
  }
  else if ( type == "ymsat1" ) {
    return unique_ptr<SatSolverImpl>(new YmSat1(option));
  }
  else if ( type == "ymsat2" ) {
    return unique_ptr<SatSolverImpl>(new YmSatMS2(option));
  }
  else if ( type == "ymsat2old" ) {
    return unique_ptr<SatSolverImpl>(new nsSatold::YmSatMS2(option));
  }
  else if ( type == "ymsat1_old" ) {
    return unique_ptr<SatSolverImpl>(new nsSat1::YmSat(option));
  }
  ASSERT_NOT_REACHED;
  return unique_ptr<SatSolverImpl>{nullptr};
}


//////////////////////////////////////////////////////////////////////
// SatSolver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatSolver::SatSolver(
  const SatSolverType& solver_type
) : mType{fallback_type(solver_type)},
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
  int id = mImpl->new_variable(decision);
  auto lit = SatLiteral::conv_from_varid(id, false);
  if ( decision ) {
    mImpl->freeze_literal(lit);
  }

  mLogger->new_variable(lit);

  ++ mVariableNum;

  return lit;
}

// @brief リテラルを 'フリーズ' する．
void
SatSolver::freeze_literal(
  SatLiteral lit
)
{
  mImpl->freeze_literal(lit);
}

// @brief リテラルを 'フリーズ' する．
void
SatSolver::freeze_literal(
  const vector<SatLiteral>& lits
)
{
  for ( auto lit: lits ) {
    mImpl->freeze_literal(lit);
  }
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

// @brief conflict_limit の最大値
SizeType
SatSolver::set_max_conflict(
  SizeType val
)
{
  return mImpl->set_max_conflict(val);
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
