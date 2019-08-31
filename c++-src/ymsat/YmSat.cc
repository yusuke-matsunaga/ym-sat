
/// @file YmSat.cc
/// @brief YmSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "YmSat.h"
#include "SatClause.h"
#include "Controller.h"
#include "Analyzer.h"
#include "Selecter.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// YmSat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSat::YmSat()
{
  mController = nullptr;
  mAnalyzer = nullptr;
  mSelecter = nullptr;
}

// @brief デストラクタ
YmSat::~YmSat()
{
  delete mController;
  delete mAnalyzer;
  delete mSelecter;
}

// @brief 変数を追加する．
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
SatVarId
YmSat::new_variable(bool decision)
{
  return mMgr.new_variable(decision);
}

// @brief 条件リテラルを設定する．
// @param[in] lit_list 条件リテラルのリスト
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
void
YmSat::set_conditional_literals(const vector<SatLiteral>& lit_list)
{
  mMgr.set_conditional_literals(lit_list);
}

// @brief 条件リテラルを設定する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
void
YmSat::set_conditional_literals(int lit_num,
				const SatLiteral* lits)
{
  mMgr.set_conditional_literals(lit_num, lits);
}

// @brief リテラルを 'フリーズ' する．
//
// lingeling 以外は無効
void
YmSat::freeze_literal(SatLiteral lit)
{
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
YmSat::add_clause(const vector<SatLiteral>& lits)
{
  mMgr.add_clause(lits);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
YmSat::add_clause(int lit_num,
		  const SatLiteral* lits)
{
  mMgr.add_clause(lit_num, lits);
}

// @brief SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
// @retval kB3True 充足した．
// @retval kB3False 充足不能が判明した．
// @retval kB3X わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
YmSat::solve(const vector<SatLiteral>& assumptions,
	     SatModel& model,
	     vector<SatLiteral>& conflicts)
{
  return mMgr.solve(assumptions, model, *mController, *mAnalyzer, *mSelecter);
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
YmSat::stop()
{
  mMgr.stop();
}

// @brief 時間計測機能を制御する
void
YmSat::timer_on(bool enable)
{
  mMgr.timer_on(enable);
}

// @brief conflict_limit の最大値
// @param[in] val 設定する値
// @return 以前の設定値を返す．
int
YmSat::set_max_conflict(int val)
{
  return mMgr.set_max_conflict(val);
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
YmSat::reg_msg_handler(SatMsgHandler* msg_handler)
{
  mMgr.reg_msg_handler(msg_handler);
}

// @brief 正しい状態のときに true を返す．
bool
YmSat::sane() const
{
  return mMgr.sane();
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
YmSat::get_stats(SatStats& stats) const
{
  mMgr.get_stats(stats);
}

// @brief 変数の数を得る．
int
YmSat::variable_num() const
{
  return mMgr.variable_num();
}

// @brief 制約節の数を得る．
int
YmSat::clause_num() const
{
  return mMgr.clause_num();
}

// @brief 制約節のリテラルの総数を得る．
int
YmSat::literal_num() const
{
  return mMgr.literal_num();
}

BEGIN_NONAMESPACE

// DIMACS 形式でリテラルを出力する．
void
write_lit(ostream& s,
	  SatLiteral lit)
{
  SatVarId var = lit.varid();
  int idx = var.val() + 1;
  if ( lit.is_negative() ) {
    s << " -";
      }
  else {
    s << " ";
  }
  s << idx;
}

END_NONAMESPACE

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
YmSat::write_DIMACS(ostream& s) const
{
  s << "p cnf " << variable_num() << " " << clause_num() << endl;
  for ( int i: Range(mMgr.unit_clause_num()) ) {
    SatLiteral lit1 = mMgr.unit_clause(i);
    write_lit(s, lit1);
    s << " 0" << endl;
  }
  for ( int i: Range(mMgr.bin_clause_num()) ) {
    SatLiteral lit1;
    SatLiteral lit2;
    mMgr.bin_clause(i, lit1, lit2);
    write_lit(s, lit1);
    write_lit(s, lit2);
    s << " 0" << endl;
  }
  for ( int i: Range(mMgr.clause_num()) ) {
    const SatClause* clause = mMgr.clause(i);
    int nl = clause->lit_num();
    for ( int j: Range(nl) ) {
      SatLiteral lit = clause->lit(j);
      write_lit(s, lit);
    }
    s << " 0" << endl;
  }
}

END_NAMESPACE_YM_SAT
