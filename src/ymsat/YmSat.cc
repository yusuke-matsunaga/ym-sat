
/// @file YmSat.cc
/// @brief YmSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "YmSat.h"
#include "Controller.h"
#include "Analyzer.h"
#include "Selecter.h"


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
YmSat::new_var(bool decision)
{
  return mMgr.new_var(decision);
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
YmSat::add_clause(ymuint lit_num,
		  const SatLiteral* lits)
{
  mMgr.add_clause(lit_num, lits);
}

// @brief 1項の節(リテラル)を追加する．
void
YmSat::add_clause(SatLiteral lit1)
{
  mMgr.add_clause(lit1);
}

// @brief 2項の節を追加する．
void
YmSat::add_clause(SatLiteral lit1,
		  SatLiteral lit2)
{
  mMgr.add_clause(lit1, lit2);
}

// @brief 3項の節を追加する．
void
YmSat::add_clause(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3)
{
  mMgr.add_clause(lit1, lit2, lit3);
}

// @brief 4項の節を追加する．
void
YmSat::add_clause(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4)
{
  mMgr.add_clause(lit1, lit2, lit3, lit4);
}

// @brief 5項の節を追加する．
void
YmSat::add_clause(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4,
		  SatLiteral lit5)
{
  mMgr.add_clause(lit1, lit2, lit3, lit4, lit5);
}

// @brief SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @retval kB3True 充足した．
// @retval kB3False 充足不能が判明した．
// @retval kB3X わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
YmSat::solve(const vector<SatLiteral>& assumptions,
	      vector<SatBool3>& model)
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
ymuint64
YmSat::set_max_conflict(ymuint64 val)
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
ymuint
YmSat::variable_num() const
{
  return mMgr.variable_num();
}

// @brief 制約節の数を得る．
ymuint
YmSat::clause_num() const
{
  return mMgr.clause_num();
}

// @brief 制約節のリテラルの総数を得る．
ymuint
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
  ymint idx = var.val() + 1;
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
  for (ymuint i = 0; i < mMgr.bin_clause_num(); ++ i) {
    SatLiteral lit1;
    SatLiteral lit2;
    mMgr.bin_clause(i, lit1, lit2);
    write_lit(s, lit1);
    write_lit(s, lit2);
  }
  for (ymuint i = 0; i < mMgr.clause_num(); ++ i) {
    const SatClause* clause = mMgr.clause(i);
    ymuint nl = clause->lit_num();
    for (ymuint j = 0; j < nl; ++ j) {
      SatLiteral lit = clause->lit(j);
      write_lit(s, lit);
    }
    s << " 0" << endl;
  }
}

END_NAMESPACE_YM_SAT
