﻿#ifndef YM_SATSOLVER_H
#define YM_SATSOLVER_H

/// @file ym/SatSolver.h
/// @brief SatSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2017, 2018, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatModel.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_YM_SAT

class SatSolverImpl;
class SatLogger;

//////////////////////////////////////////////////////////////////////
/// @class SatSolver SatSolver.h "ym/SatSolver.h"
/// @ingroup SatGroup
/// @brief CNF-SAT エンジンのインターフェイスを表すクラス
///
/// 実際の処理は SatSolverImpl (の継承クラス)が行う．
//////////////////////////////////////////////////////////////////////
class SatSolver
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] solver_type 実装タイプ
  ///
  /// @sa SatSolverType
  SatSolver(const SatSolverType& solver_type = SatSolverType());

  /// @brief デストラクタ
  ~SatSolver();

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
  /// @return 新しい変数を表すリテラルを返す．
  ///
  /// * 変数番号は 0 から始まる．
  /// * lingeling においては decision = true のリテラルは freeze される．
  SatLiteral
  new_variable(bool decision = true);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit_list 条件リテラルのリスト
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(const vector<SatLiteral>& lit_list);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit1 条件リテラル
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(SatLiteral lit1);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit1, lit2 条件リテラル
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(SatLiteral lit1,
			   SatLiteral lit2);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit1, lit2, lit3 条件リテラル
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit1, lit2, lit3, lit4 条件リテラル
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4);

  /// @brief 条件リテラルをクリアする．
  void
  clear_conditional_literals();

  /// @brief リテラルを 'フリーズ' する．
  ///
  /// lingeling 以外は無効
  void
  freeze_literal(SatLiteral lit);

  /// @brief リテラルを 'フリーズ' する．
  ///
  /// lingeling 以外は無効
  void
  freeze_literal(const vector<SatLiteral>& lits);

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 1項の節(リテラル)を追加する．
  /// @param[in] lit1 リテラル
  void
  add_clause(SatLiteral lit1);

  /// @brief 2項の節を追加する．
  /// @param[in] lit1, lit2 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2);

  /// @brief 3項の節を追加する．
  /// @param[in] lit1, lit2, lit3 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3);

  /// @brief 4項の節を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4);

  /// @brief 5項の節を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4,
	     SatLiteral lit5);

  /// @brief 6項の節を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4,
	     SatLiteral lit5,
	     SatLiteral lit6);

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 動作制御を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief SAT 問題を解く．
  /// @param[in] time_limit 時間制約(秒) 0 で制約なし
  /// @return 結果(SatBool3)を返す．
  ///
  /// 結果の意味は以下の通り
  /// * kB3True  充足した．
  /// * kB3False 充足不能が判明した．
  /// * kB3X     わからなかった．
  SatBool3
  solve(int time_limit = 0);

  /// @brief assumption 付きの SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[in] time_limit 時間制約(秒) 0 で制約なし
  /// @return 結果(SatBool3)を返す．
  ///
  /// 結果の意味は以下の通り
  /// * kB3True  充足した．
  /// * kB3False 充足不能が判明した．
  /// * kB3X     わからなかった．
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	int time_limit = 0);

  /// @brief 直前に解いた問題のモデルを返す．
  const SatModel&
  model();

  /// @brief 直前に解いた問題のサイズを返す．
  int
  model_size();

  /// @brief 直前に解いた問題の値を返す．
  /// @param[in] lit リテラル
  SatBool3
  read_model(SatLiteral lit);

  /// @brief 直前の問題の矛盾の原因のリテラルを返す．
  const vector<SatLiteral>&
  conflict_literals();

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  void
  stop();

  /// @brief 時間計測機能を制御する
  void
  timer_on(bool enable);

  /// @brief conflict_limit の最大値
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  int
  set_max_conflict(int val);

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  /// @param[in] msg_handler 登録するメッセージハンドラ
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

  /// @brief SolverType を取り出す．
  SatSolverType
  type() const;

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const;

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  void
  get_stats(SatStats& stats) const;

  /// @brief 変数の数を得る．
  int
  variable_num() const;

  /// @brief 制約節の数を得る．
  int
  clause_num() const;

  /// @brief 制約節のリテラルの総数を得る．
  int
  literal_num() const;

  /// @brief DIMACS 形式で制約節を出力する．
  /// @param[in] s 出力先のストリーム
  void
  write_DIMACS(ostream& s) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////

private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief set_conditional_literals() の下請け関数
  /// @param[in] n conditional_literal の数
  /// @param[in] lits conditional_literal の配列
  void
  _set_conditional_literals(int n,
			    SatLiteral lits[]);

  /// @brief add_clause() の下請け関数
  /// @param[in] lits リテラルのリスト
  void
  _add_clause(const vector<SatLiteral>& lits);

  /// @brief add_clause() の下請け関数
  /// @param[in] n リテラル数
  /// @param[in] lits リテラルの配列
  void
  _add_clause(int n,
	      SatLiteral lits[]);

  /// @brief _add_clause() の下請け関数
  /// @param[in] lits リテラルのリスト
  void
  _add_clause_sub(const vector<SatLiteral>& lits);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ソルバの型
  SatSolverType mType;

  // 実際の機能を実装しているクラス
  unique_ptr<SatSolverImpl> mImpl;

  // ロガー
  unique_ptr<SatLogger> mLogger;

  // 条件リテラル
  vector<SatLiteral> mConditionalLits;

  // 直前の問題のモデル
  SatModel mModel;

  // 直前の矛盾の原因
  vector<SatLiteral> mConflictLiterals;

  // 変数の数(リポート用)
  int mVariableNum{0};

  // 節のリスト(リポート用)
  vector<vector<SatLiteral>> mClauseList;

  // リテラル数(リポート用)
  int mLiteralNum{0};

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief SolverType を取り出す．
inline
SatSolverType
SatSolver::type() const
{
  return mType;
}

// @brief 条件リテラルを設定する．
// @param[in] lit_list 条件リテラルのリスト
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
inline
void
SatSolver::set_conditional_literals(const vector<SatLiteral>& lit_list)
{
  mConditionalLits = lit_list;
}

// @brief 条件リテラルを設定する．
// @param[in] lit1 条件リテラル
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
inline
void
SatSolver::set_conditional_literals(SatLiteral lit1)
{
  _set_conditional_literals(1, &lit1);
}

// @brief 条件リテラルを設定する．
// @param[in] lit1, lit2 条件リテラル
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
inline
void
SatSolver::set_conditional_literals(SatLiteral lit1,
				    SatLiteral lit2)
{
  SatLiteral tmp_lits[] = {lit1, lit2};

  _set_conditional_literals(2, tmp_lits);
}

// @brief 条件リテラルを設定する．
// @param[in] lit1, lit2, lit3 条件リテラル
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
inline
void
SatSolver::set_conditional_literals(SatLiteral lit1,
				    SatLiteral lit2,
				    SatLiteral lit3)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3};

  _set_conditional_literals(3, tmp_lits);
}

// @brief 条件リテラルを設定する．
// @param[in] lit1, lit2, lit3, lit4 条件リテラル
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
inline
void
SatSolver::set_conditional_literals(SatLiteral lit1,
				    SatLiteral lit2,
				    SatLiteral lit3,
				    SatLiteral lit4)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4};

  _set_conditional_literals(4, tmp_lits);
}

// @brief 条件リテラルをクリアする．
inline
void
SatSolver::clear_conditional_literals()
{
  mConditionalLits.clear();
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
inline
void
SatSolver::add_clause(const vector<SatLiteral>& lits)
{
  _add_clause(lits);
}

// @brief 1項の節を追加する．
inline
void
SatSolver::add_clause(SatLiteral lit1)
{
  _add_clause(1, &lit1);
}

// @brief 2項の節を追加する．
inline
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2)
{
  SatLiteral tmp_lits[] = {lit1, lit2};

  _add_clause(2, tmp_lits);
}

// @brief 3項の節を追加する．
inline
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3};

  _add_clause(3, tmp_lits);
}

// @brief 4項の節を追加する．
inline
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4};

  _add_clause(4, tmp_lits);
}

// @brief 5項の節を追加する．
inline
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4,
		      SatLiteral lit5)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4, lit5};

  _add_clause(5, tmp_lits);
}

// @brief 6項の節を追加する．
inline
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4,
		      SatLiteral lit5,
		      SatLiteral lit6)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4, lit5, lit6};

  _add_clause(6, tmp_lits);
}

// @brief SAT 問題を解く．
// @param[in] time_limit 時間制約(秒) 0 で制約なし
// @return 結果(SatBool3)を返す．
//
// 結果の意味は以下の通り
// * kB3True  充足した．
// * kB3False 充足不能が判明した．
// * kB3X     わからなかった．
inline
SatBool3
SatSolver::solve(int time_limit)
{
  // 空の assumptions を付けて solve() を呼ぶだけ
  return solve(vector<SatLiteral>(), time_limit);
}

// @brief 直前に解いた問題のモデルを返す．
inline
const SatModel&
SatSolver::model()
{
  return mModel;
}

// @brief 直前に解いた問題のサイズを返す．
inline
int
SatSolver::model_size()
{
  return mModel.size();
}

// @brief 直前に解いた問題の値を返す．
// @param[in] lit リテラル
inline
SatBool3
SatSolver::read_model(SatLiteral lit)
{
  return mModel.get(lit);
}

// @brief 直前の問題の矛盾の原因のリテラルを返す．
inline
const vector<SatLiteral>&
SatSolver::conflict_literals()
{
  return mConflictLiterals;
}

// @brief 変数の数を返す．
inline
int
SatSolver::variable_num() const
{
  return mVariableNum;
}

// @brief 節の数を返す．
inline
int
SatSolver::clause_num() const
{
  return mClauseList.size();
}

// @brief リテラルの数を返す．
inline
int
SatSolver::literal_num() const
{
  return mLiteralNum;
}

END_NAMESPACE_YM_SAT

#endif // YM_SATSOLVER_H
