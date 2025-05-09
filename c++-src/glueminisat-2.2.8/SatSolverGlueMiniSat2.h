﻿#ifndef SATSOLVERGLUEMINISAT2_H
#define SATSOLVERGLUEMINISAT2_H

/// @file SatSolverGlueMiniSat2.h
/// @brief SatSolverGlueMiniSat2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatSolverImpl.h"
#include "Solver.h"
#include "ym/json.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatSolverGlueMiniSat2 SatSolverGlueMiniSat2.h "SatSolverGlueMiniSat2.h"
/// @brief GlueMiniSat を SatSolverImpl のインターフェイスに合わせるためのラッパ
//////////////////////////////////////////////////////////////////////
class SatSolverGlueMiniSat2 :
  public SatSolverImpl
{
public:

  /// @brief コンストラクタ
  SatSolverGlueMiniSat2(
    const JsonValue& js_obj ///< [in] 初期化パラメータ
  );

  /// @brief デストラクタ
  ~SatSolverGlueMiniSat2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatSolver で定義されている仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const override;

  /// @brief 変数を追加する．
  /// @return 新しい変数番号を返す．
  ///
  /// 変数番号は 0 から始まる．
  SatLiteral
  new_variable(
    bool decision ///< [in] 決定変数の時に true とする．
  ) override;

  /// @brief 節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] リテラルのベクタ
  ) override;

  /// @brief SAT 問題を解く．
  /// @retval SatBool3::True 充足した．
  /// @retval SatBool3::False 充足不能が判明した．
  /// @retval SatBool3::X わからなかった．
  /// @note i 番めの変数の割り当て結果は model[i] に入る．
  SatBool3
  solve(
    const vector<SatLiteral>& assumptions, ///< [in] あらかじめ仮定する変数の値割り当てリスト
    SatModel& model,                       ///< [in] 充足するときの値の割り当てを格納する配列．
    vector<SatLiteral>& conflicts          ///< [in] 充足不能の場合に原因となっている仮定を入れる配列．
  ) override;

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  void
  stop() override;

  /// @brief 現在の内部状態を得る．
  SatStats
  get_stats() const override;

  /// @brief トータルの矛盾回数の制限を設定する．
  /// @return 以前の設定値を返す．
  SizeType
  set_conflict_budget(
    SizeType val ///< [in] 設定する値
  ) override;

  /// @brief トータルの implication 回数の制限を設定する．
  /// @return 以前の設定値を返す．
  SizeType
  set_propagation_budget(
    SizeType val ///< [in] 設定する値
  ) override;

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  void
  reg_msg_handler(
    SatMsgHandler* msg_handler ///< [in] 登録するメッセージハンドラ
  ) override;

  /// @brief 時間計測機能を制御する
  void
  timer_on(
    bool enable
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ソルバの本体
  Glueminisat::Solver mSolver;

};

END_NAMESPACE_YM_SAT

#endif // SATSOLVERGLUEMINISAT_H
