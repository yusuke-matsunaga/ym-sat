#ifndef YMSAT_H
#define YMSAT_H

/// @file YmSat.h
/// @brief YmSat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatSolverImpl.h"
#include "CoreMgr.h"
#include "conf.h"


BEGIN_NAMESPACE_YM_SAT

class Controller;
class Analyzer;
class Selecter;

//////////////////////////////////////////////////////////////////////
/// @class YmSat YmSat.h "YmSat.h"
/// @brief SatSolver の実装クラス
///
/// 実はこのクラスは Controller, Analyzer, Selecter の３つのオブジェクト
/// を結びつけるだけのハブクラス<br>
/// 実際の処理は CoreMgr が行う．<br>
/// ヒューリスティックにより動作が変わる部分を Controller, Analyzer,
/// Selecter で実装する<br>
///
/// コンストラクタで Controller, Analyzer, Selecter を指定するとシンプル
/// になるが，これらのオブジェクトのコンストラクタが CoreMgr& を必要とする
/// 関係で生成が YmSat よりも後になってしまう．<br>
/// そこで継承クラス中でこれらのオブジェクトを生成して set_controller()
/// などで設定するスタイルを取る<br>
/// 継承クラスのみにこれらのアクセスを許すのは中途半端な状態のオブジェクト
/// を作りたくないから
//////////////////////////////////////////////////////////////////////
class YmSat :
  public SatSolverImpl
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  YmSat();

  /// @brief デストラクタ
  ~YmSat();

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 変数と節の生成を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を追加する．
  /// @return 新しい変数番号を返す．
  ///
  /// 変数番号は 0 から始まる．
  SatVarId
  new_variable(
    bool decision ///< [in] 決定変数の時に true とする．
  ) override;

  /// @brief 節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] リテラルのベクタ
  ) override;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 動作制御を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief SAT 問題を解く．
  /// @retval SatBool3::True 充足した．
  /// @retval SatBool3::False 充足不能が判明した．
  /// @retval SatBool3::X わからなかった．
  /// @note i 番めの変数の割り当て結果は model[i] に入る．
  SatBool3
  solve(
    const vector<SatLiteral>& assumptions, ///< [in] あらかじめ仮定する変数の値割り当てリスト
    SatModel& model,                       ///< [out] 充足するときの値の割り当てを格納する配列．
    vector<SatLiteral>& conflicts          ///< [out] 充足不能の場合に原因となっている仮定を入れる配列．
  ) override;

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  void
  stop() override;

  /// @brief 時間計測機能を制御する
  void
  timer_on(
    bool enable
  ) override;

  /// @brief conflict_limit の最大値
  /// @return 以前の設定値を返す．
  SizeType
  set_max_conflict(
    SizeType val ///< [in] 設定する値
  ) override;

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  void
  reg_msg_handler(
    SatMsgHandler* msg_handler ///< [in] 登録するメッセージハンドラ
  ) override;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内部状態の取得を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const override;

  /// @brief 現在の内部状態を得る．
  SatStats
  get_stats() const override;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief コアマネージャを返す．
  CoreMgr&
  mgr()
  {
    return mMgr;
  }

  /// @brief Controler をセットする．
  void
  set_controller(Controller* controller)
  {
    mController = controller;
  }

  /// @brief Analyzer をセットする．
  void
  set_analyzer(Analyzer* analyzer)
  {
    mAnalyzer = analyzer;
  }

  /// @brief Selecter をセットする．
  void
  set_selecter(Selecter* selecter)
  {
    mSelecter = selecter;
  }

  /// @brief 制約節の数を得る．
  SizeType
  _clause_num() const
  {
    return mMgr.clause_num();
  }

  /// @brief 総矛盾数を返す．
  SizeType
  conflict_num() const
  {
    return mMgr.conflict_num();
  }

  /// @brief 総分岐数を返す．
  SizeType
  decision_num() const
  {
    return mMgr.decision_num();
  }

  /// @brief 総インプリケーション数を返す．
  SizeType
  propagation_num() const
  {
    return mMgr.propagation_num();
  }

  /// @brief 矛盾回数の制限値を得る．
  SizeType
  conflict_limit() const
  {
    return mMgr.conflict_limit();
  }

  /// @brief 学習節の制限値を得る．
  SizeType
  learnt_limit() const
  {
    return mMgr.learnt_limit();
  }

  /// @brief conflict_limit の最大値を返す．
  SizeType
  max_conflict() const
  {
    return mMgr.max_conflict();
  }

  /// @brief 矛盾回数の制限値を設定する．
  void
  set_conflict_limit(
    SizeType limit ///< [in] 設定する値
  )
  {
    mMgr.set_conflict_limit(limit);
  }

  /// @brief 学習節の制限値を設定する．
  void
  set_learnt_limit(
    SizeType limit ///< [in] 設定する値
  )
  {
    mMgr.set_learnt_limit(limit);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コアマネージャ
  CoreMgr mMgr;

  // コントローラー
  Controller* mController;

  // 解析器
  Analyzer* mAnalyzer;

  // 変数選択器
  Selecter* mSelecter;

};

END_NAMESPACE_YM_SAT

#endif // YMSATBASE_H
