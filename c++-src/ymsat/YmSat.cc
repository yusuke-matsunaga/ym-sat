
/// @file YmSat.cc
/// @brief YmSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "YmSat.h"
#include "Clause.h"
#include "Controller.h"
#include "Analyzer.h"
#include "Selecter.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

const Literal Literal::X;

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
int
YmSat::new_variable(
  bool decision
)
{
  return mMgr.new_variable(decision);
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
YmSat::add_clause(
  const vector<SatLiteral>& lits
)
{
  mMgr.add_clause(lits);
}

// @brief SAT 問題を解く．
SatBool3
YmSat::solve(
  const vector<SatLiteral>& assumptions,
  SatModel& model,
  vector<SatLiteral>& conflicts
)
{
  // 実は conflicts に値を設定していない．
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
YmSat::timer_on(
  bool enable
)
{
  mMgr.timer_on(enable);
}

// @brief conflict_limit の最大値
int
YmSat::set_max_conflict(
  int val
)
{
  return mMgr.set_max_conflict(val);
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
void
YmSat::reg_msg_handler(
  SatMsgHandler* msg_handler
)
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
SatStats
YmSat::get_stats() const
{
  return mMgr.get_stats();
}

END_NAMESPACE_YM_SAT
