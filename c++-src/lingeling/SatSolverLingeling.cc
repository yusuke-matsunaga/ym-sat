
/// @file SatSolverLingeling.cc
/// @brief SatSolverLingeling の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatSolverLingeling.h"
#include "ym/SatStats.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

BEGIN_NONAMESPACE

inline
int
translate(SatLiteral l)
{
  int v = l.varid().val() + 1;
  return l.is_negative() ? -v : v;
}

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
// SatSolverLingeling
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] option オプション文字列
SatSolverLingeling::SatSolverLingeling(const string& option)
{
  mSolver = lglinit();
  mNumVars = 0;
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
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
SatVarId
SatSolverLingeling::new_variable(bool decision)
{
  SatVarId var(mNumVars);
  ++ mNumVars;
  return var;
}

// @brief 条件リテラルを設定する．
// @param[in] lit_list 条件リテラルのリスト
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
void
SatSolverLingeling::set_conditional_literals(const vector<SatLiteral>& lit_list)
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
SatSolverLingeling::set_conditional_literals(int lit_num,
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
SatSolverLingeling::freeze_literal(SatLiteral lit)
{
  lglfreeze(mSolver, translate(lit));
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatSolverLingeling::add_clause(const vector<SatLiteral>& lits)
{
  for ( auto l: lits ) {
    int x = translate(l);
    lgladd(mSolver, x);
  }
  for ( auto l: mCondLits ) {
    // 極性が反転することに注意
    int x = translate(~l);
    lgladd(mSolver, x);
  }
  lgladd(mSolver, 0);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
SatSolverLingeling::add_clause(int lit_num,
			      const SatLiteral* lits)
{
  for ( int i = 0; i < lit_num; ++ i ) {
    SatLiteral l = lits[i];
    int x = translate(l);
    lgladd(mSolver, x);
  }
  for ( auto l: mCondLits ) {
    // 極性が反転することに注意
    int x = translate(~l);
    lgladd(mSolver, x);
  }
  lgladd(mSolver, 0);
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
SatSolverLingeling::solve(const vector<SatLiteral>& assumptions,
			  vector<SatBool3>& model,
			  vector<SatLiteral>& conflicts)
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
      if ( v == 1) {
	model[i] = SatBool3::True;
      }
      else if ( v == -1 ) {
	model[i] = SatBool3::False;
      }
      else {
	model[i] = SatBool3::X;
      }
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

// @brief conflict_limit の最大値
// @param[in] val 設定する値
// @return 以前の設定値を返す．
int
SatSolverLingeling::set_max_conflict(int val)
{
  // 無効
  return 0;
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
SatSolverLingeling::get_stats(SatStats& stats) const
{
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
}

// @brief 変数の数を得る．
int
SatSolverLingeling::variable_num() const
{
  return mNumVars;
}

// @brief 制約節の数を得る．
int
SatSolverLingeling::clause_num() const
{
  return lglnclauses(mSolver);
}

// @brief 制約節のリテラルの総数を得る．
int
SatSolverLingeling::literal_num() const
{
  return 0;
}

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
SatSolverLingeling::write_DIMACS(ostream& s) const
{
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
SatSolverLingeling::reg_msg_handler(SatMsgHandler* msg_handler)
{
}

// @brief 時間計測機能を制御する
void
SatSolverLingeling::timer_on(bool enable)
{
}

END_NAMESPACE_YM_SAT
