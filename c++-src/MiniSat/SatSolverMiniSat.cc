
/// @file SatSolverMiniSat.cc
/// @brief SatSolverMiniSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatSolverMiniSat.h"
#include "ym/SatStats.h"


BEGIN_NAMESPACE_YM_SAT


//////////////////////////////////////////////////////////////////////
// SatSolverMiniSat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] option オプション文字列
SatSolverMiniSat::SatSolverMiniSat(const string& option)
{
  if ( option == "verbose" ) {
    mSolver.verbosity = 1;
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
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
SatVarId
SatSolverMiniSat::new_variable(bool decision)
{
  return SatVarId(mSolver.newVar());
}

// @brief 条件リテラルを設定する．
// @param[in] lit_list 条件リテラルのリスト
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
void
SatSolverMiniSat::set_conditional_literals(const vector<SatLiteral>& lit_list)
{
  mCondLits.clear();
  int lit_num = lit_list.size();
  mCondLits.resize(lit_num);
  for ( int i = 0; i < lit_num; ++ i ) {
    mCondLits[i] = lit_list[i];
  }
}

// @brief 条件リテラルを設定する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
void
SatSolverMiniSat::set_conditional_literals(int lit_num,
					   const SatLiteral* lits)
{
  mCondLits.clear();
  mCondLits.resize(lit_num);
  for ( int i = 0; i < lit_num; ++ i ) {
    mCondLits[i] = lits[i];
  }
}

// @brief リテラルを 'フリーズ' する．
//
// lingeling 以外は無効
void
SatSolverMiniSat::freeze_literal(SatLiteral lit)
{
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatSolverMiniSat::add_clause(const vector<SatLiteral>& lits)
{
  vec<Lit> tmp;
  for ( auto l: lits ) {
    Lit lit(l.varid().val(), l.is_negative());
    tmp.push(lit);
  }
  for ( auto l: mCondLits ) {
    // 極性が反転することに注意
    Lit lit(l.varid().val(), l.is_positive());
    tmp.push(lit);
  }
  mSolver.addClause(tmp);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
SatSolverMiniSat::add_clause(int lit_num,
			     const SatLiteral* lits)
{
  vec<Lit> tmp;
  for ( int i = 0; i < lit_num; ++ i ) {
    SatLiteral l = lits[i];
    Lit lit(l.varid().val(), l.is_negative());
    tmp.push(lit);
  }
  for ( auto l: mCondLits ) {
    // 極性が反転することに注意
    Lit lit(l.varid().val(), l.is_positive());
    tmp.push(lit);
  }
  mSolver.addClause(tmp);
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
SatSolverMiniSat::solve(const vector<SatLiteral>& assumptions,
			vector<SatBool3>& model,
			vector<SatLiteral>& conflicts)
{
  vec<Lit> tmp;
  for ( auto l: assumptions ) {
    Lit lit(l.varid().val(), l.is_negative());
    tmp.push(lit);
  }
  bool ans = mSolver.solve(tmp);
  if ( ans ) {
    int n = mSolver.model.size();
    model.resize(n);
    for ( int i = 0; i < n; ++ i ) {
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
    // conflicts は未実装
    return SatBool3::False;
  }
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
SatSolverMiniSat::stop()
{
  // 未実装
}

// @brief conflict_limit の最大値
// @param[in] val 設定する値
// @return 以前の設定値を返す．
int
SatSolverMiniSat::set_max_conflict(int val)
{
  // 無効
  return 0;
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
SatSolverMiniSat::get_stats(SatStats& stats) const
{
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
}

// @brief 変数の数を得る．
int
SatSolverMiniSat::variable_num() const
{
  return mSolver.nVars();
}

// @brief 制約節の数を得る．
int
SatSolverMiniSat::clause_num() const
{
  return mSolver.nClauses();
}

// @brief 制約節のリテラルの総数を得る．
int
SatSolverMiniSat::literal_num() const
{
  return mSolver.stats.clauses_literals;
}

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
SatSolverMiniSat::write_DIMACS(ostream& s) const
{
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
SatSolverMiniSat::reg_msg_handler(SatMsgHandler* msg_handler)
{
}

// @brief 時間計測機能を制御する
void
SatSolverMiniSat::timer_on(bool enable)
{
}

END_NAMESPACE_YM_SAT
