#ifndef SATSOLVERIMPL_H
#define SATSOLVERIMPL_H

/// @file SatSolverImpl.h
/// @brief SatSolverImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatSolverImpl SatSolverImpl.h "SatSolverImpl.h"
/// @brief SatSolver の実装クラス
//////////////////////////////////////////////////////////////////////
class SatSolverImpl
{
public:

  /// @brief 継承クラスを作るクラスメソッド
  /// @param[in] solver_type SATソルバのタイプ
  static
  unique_ptr<SatSolverImpl>
  new_impl(const SatSolverType& solver_type);

  /// @brief デストラクタ
  virtual
  ~SatSolverImpl() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  virtual
  bool
  sane() const = 0;

  /// @brief 変数を追加する．
  /// @param[in] decision 決定変数の時に true とする．
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  virtual
  int
  new_variable(bool decition) = 0;

  /// @brief リテラルを 'フリーズ' する．
  ///
  /// lingeling 以外は無効
  virtual
  void
  freeze_literal(SatLiteral lit) = 0;

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  virtual
  void
  add_clause(const vector<SatLiteral>& lits) = 0;

  /// @brief SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
  /// @retval kB3True 充足した．
  /// @retval kB3False 充足不能が判明した．
  /// @retval kB3X わからなかった．
  /// @note i 番めの変数の割り当て結果は model[i] に入る．
  virtual
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	SatModel& model,
	vector<SatLiteral>& conflicts) = 0;

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  virtual
  void
  stop() = 0;

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  virtual
  void
  get_stats(SatStats& stats) const = 0;

  /// @brief conflict_limit の最大値
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  virtual
  int
  set_max_conflict(int val) = 0;

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  /// @param[in] msg_handler 登録するメッセージハンドラ
  virtual
  void
  reg_msg_handler(SatMsgHandler* msg_handler) = 0;

  /// @brief 時間計測機能を制御する
  virtual
  void
  timer_on(bool enable) = 0;

};

END_NAMESPACE_YM_SAT

#endif // LIBYM_YMYMLOGIC_SAT_INCLUDE_SATSOLVERIMPL_H
