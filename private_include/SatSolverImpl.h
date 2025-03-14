﻿#ifndef SATSOLVERIMPL_H
#define SATSOLVERIMPL_H

/// @file SatSolverImpl.h
/// @brief SatSolverImpl のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018, 2019, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatStats.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatSolverImpl SatSolverImpl.h "SatSolverImpl.h"
/// @brief SatSolver の実装クラス
//////////////////////////////////////////////////////////////////////
class SatSolverImpl
{
public:

  /// @brief 継承クラスを作るクラスメソッド
  static
  unique_ptr<SatSolverImpl>
  new_impl(
    const SatInitParam& init_param ///< [in] 初期化パラメータ
  );

  /// @brief デストラクタ
  virtual
  ~SatSolverImpl();


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが実装する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  virtual
  bool
  sane() const = 0;

  /// @brief 変数を追加する．
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  virtual
  SatLiteral
  new_variable(
    bool decition ///< [in] 決定変数の時に true とする．
  ) = 0;

  /// @brief 節を追加する．
  virtual
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] リテラルのベクタ
  ) = 0;

  /// @brief SAT 問題を解く．
  /// @retval kB3True 充足した．
  /// @retval kB3False 充足不能が判明した．
  /// @retval kB3X わからなかった．
  /// @note i 番めの変数の割り当て結果は model[i] に入る．
  virtual
  SatBool3
  solve(
    const vector<SatLiteral>& assumptions, ///< [in] あらかじめ仮定する変数の値割り当てリスト
    SatModel& model,                       ///< [out] 充足するときの値の割り当てを格納する配列．
    vector<SatLiteral>& conflicts          ///< [out] 充足不能の場合に原因となっている仮定を入れる配列．
  ) = 0;

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  virtual
  void
  stop() = 0;

  /// @brief 現在の内部状態を得る．
  virtual
  SatStats
  get_stats() const = 0;

  /// @brief トータルの矛盾回数の制限を設定する．
  /// @return 以前の設定値を返す．
  virtual
  SizeType
  set_conflict_budget(
    SizeType val ///< [in] 設定する値
  ) = 0;

  /// @brief トータルの implication 回数の制限を設定する．
  /// @return 以前の設定値を返す．
  virtual
  SizeType
  set_propagation_budget(
    SizeType val ///< [in] 設定する値
  ) = 0;

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  virtual
  void
  reg_msg_handler(
    SatMsgHandler* msg_handler ///< [in] 登録するメッセージハンドラ
  ) = 0;

  /// @brief 時間計測機能を制御する
  virtual
  void
  timer_on(
    bool enable
  ) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  static
  SatLiteral
  get_lit(
    SatVarId varid, ///< [in] 変数番号
    bool inv        ///< [in] 反転フラグ
  )
  {
    return SatLiteral::conv_from_varid(varid, inv);
  }

};

END_NAMESPACE_YM_SAT

#endif // LIBYM_YMYMLOGIC_SAT_INCLUDE_SATSOLVERIMPL_H
