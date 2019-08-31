#ifndef SATSOLVERGLUEMINISAT2_H
#define SATSOLVERGLUEMINISAT2_H

/// @file SatSolverGlueMiniSat2.h
/// @brief SatSolverGlueMiniSat2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SatSolverImpl.h"
#include "Solver.h"


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
  /// @param[in] option オプション文字列
  SatSolverGlueMiniSat2(const string& option);

  /// @brief デストラクタ
  virtual
  ~SatSolverGlueMiniSat2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatSolver で定義されている仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const override;

  /// @brief 変数を追加する．
  /// @param[in] decision 決定変数の時に true とする．
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  SatVarId
  new_variable(bool decision) override;

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit_list 条件リテラルのリスト
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  void
  set_conditional_literals(const vector<SatLiteral>& lit_list) override;

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit_num リテラル数
  /// @param[in] lits リテラルの配列
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  void
  set_conditional_literals(int lit_num,
			   const SatLiteral* lits) override;

  /// @brief リテラルを 'フリーズ' する．
  ///
  /// lingeling 以外は無効
  void
  freeze_literal(SatLiteral lit) override;

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  void
  add_clause(const vector<SatLiteral>& lits) override;

  /// @brief 節を追加する．
  /// @param[in] lit_num リテラル数
  /// @param[in] lits リテラルの配列
  void
  add_clause(int lit_num,
	     const SatLiteral* lits) override;

  /// @brief SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
  /// @retval kB3True 充足した．
  /// @retval kB3False 充足不能が判明した．
  /// @retval kB3X わからなかった．
  /// @note i 番めの変数の割り当て結果は model[i] に入る．
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	SatModel& model,
	vector<SatLiteral>& conflicts) override;

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  void
  stop() override;

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  void
  get_stats(SatStats& stats) const override;

  /// @brief 変数の数を得る．
  int
  variable_num() const override;

  /// @brief 制約節の数を得る．
  int
  clause_num() const override;

  /// @brief 制約節のリテラルの総数を得る．
  int
  literal_num() const override;

  /// @brief DIMACS 形式で制約節を出力する．
  /// @param[in] s 出力先のストリーム
  void
  write_DIMACS(ostream& s) const override;

  /// @brief conflict_limit の最大値
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  int
  set_max_conflict(int val) override;

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  /// @param[in] msg_handler 登録するメッセージハンドラ
  void
  reg_msg_handler(SatMsgHandler* msg_handler) override;

  /// @brief 時間計測機能を制御する
  void
  timer_on(bool enable) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ソルバの本体
  Glueminisat::Solver mSolver;

  // 条件リテラルのリスト
  vector<SatLiteral> mCondLits;

};

END_NAMESPACE_YM_SAT

#endif // SATSOLVERGLUEMINISAT_H
