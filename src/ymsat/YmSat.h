#ifndef YMSAT_H
#define YMSAT_H

/// @file YmSat.h
/// @brief YmSat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
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
  virtual
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
  /// @param[in] decision 決定変数の時に true とする．
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  virtual
  SatVarId
  new_variable(bool decision);

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  virtual
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 節を追加する．
  /// @param[in] lit_num リテラル数
  /// @param[in] lits リテラルの配列
  virtual
  void
  add_clause(ymuint lit_num,
	     const SatLiteral* lits);

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 動作制御を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

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
	vector<SatBool3>& model,
	vector<SatLiteral>& conflicts);

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  virtual
  void
  stop();

  /// @brief 時間計測機能を制御する
  virtual
  void
  timer_on(bool enable);

  /// @brief conflict_limit の最大値
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  virtual
  ymuint64
  set_max_conflict(ymuint64 val);

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  /// @param[in] msg_handler 登録するメッセージハンドラ
  virtual
  void
  reg_msg_handler(SatMsgHandler* msg_handler);

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内部状態の取得を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  virtual
  bool
  sane() const;

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  virtual
  void
  get_stats(SatStats& stats) const;

  /// @brief 変数の数を得る．
  virtual
  ymuint
  variable_num() const;

  /// @brief 制約節の数を得る．
  virtual
  ymuint
  clause_num() const;

  /// @brief 制約節のリテラルの総数を得る．
  virtual
  ymuint
  literal_num() const;

  /// @brief DIMACS 形式で制約節を出力する．
  /// @param[in] s 出力先のストリーム
  virtual
  void
  write_DIMACS(ostream& s) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief コアマネージャを返す．
  CoreMgr&
  mgr();

  /// @brief Controler をセットする．
  void
  set_controller(Controller* controller);

  /// @brief Analyzer をセットする．
  void
  set_analyzer(Analyzer* analyzer);

  /// @brief Selecter をセットする．
  void
  set_selecter(Selecter* selecter);

  /// @brief 制約節の数を得る．
  ymuint
  _clause_num() const;

  /// @brief 総矛盾数を返す．
  ymuint64
  conflict_num() const;

  /// @brief 総分岐数を返す．
  ymuint64
  decision_num() const;

  /// @brief 総インプリケーション数を返す．
  ymuint64
  propagation_num() const;

  /// @brief 矛盾回数の制限値を得る．
  ymuint64
  conflict_limit() const;

  /// @brief 学習節の制限値を得る．
  ymuint64
  learnt_limit() const;

  /// @brief conflict_limit の最大値を返す．
  ymuint64
  max_conflict() const;

  /// @brief 矛盾回数の制限値を設定する．
  /// @param[in] limit 設定する値
  void
  set_conflict_limit(ymuint64 limit);

  /// @brief 学習節の制限値を設定する．
  /// @param[in] limit 設定する値
  void
  set_learnt_limit(ymuint64 limit);


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


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コアマネージャを返す．
inline
CoreMgr&
YmSat::mgr()
{
  return mMgr;
}

// @brief Controler をセットする．
inline
void
YmSat::set_controller(Controller* controller)
{
  mController = controller;
}

// @brief Analyzer をセットする．
inline
void
YmSat::set_analyzer(Analyzer* analyzer)
{
  mAnalyzer = analyzer;
}

// @brief Selecter を返す．
inline
void
YmSat::set_selecter(Selecter* selecter)
{
  mSelecter = selecter;
}

// @brief 制約節の数を得る．
inline
ymuint
YmSat::_clause_num() const
{
  return mMgr.clause_num();
}

// @brief 総矛盾数を返す．
inline
ymuint64
YmSat::conflict_num() const
{
  return mMgr.conflict_num();
}

// @brief 総分岐数を返す．
inline
ymuint64
YmSat::decision_num() const
{
  return mMgr.decision_num();
}

// @brief 総インプリケーション数を返す．
inline
ymuint64
YmSat::propagation_num() const
{
  return mMgr.propagation_num();
}

// @brief 矛盾回数の制限値を得る．
inline
ymuint64
YmSat::conflict_limit() const
{
  return mMgr.conflict_limit();
}

// @brief 学習節の制限値を得る．
inline
ymuint64
YmSat::learnt_limit() const
{
  return mMgr.learnt_limit();
}

// @brief conflict_limit の最大値
// @param[in] val 設定する値
// @return 以前の設定値を返す．
inline
ymuint64
YmSat::max_conflict() const
{
  return mMgr.max_conflict();
}

// @brief 矛盾回数の制限値を設定する．
// @param[in] limit 設定する値
inline
void
YmSat::set_conflict_limit(ymuint64 limit)
{
  mMgr.set_conflict_limit(limit);
}

// @brief 学習節の制限値を設定する．
// @param[in] limit 設定する値
inline
void
YmSat::set_learnt_limit(ymuint64 limit)
{
  mMgr.set_learnt_limit(limit);
}

END_NAMESPACE_YM_SAT

#endif // YMSATBASE_H
