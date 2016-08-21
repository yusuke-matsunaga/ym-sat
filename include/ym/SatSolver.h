#ifndef YM_SATSOLVER_H
#define YM_SATSOLVER_H

/// @file ym/SatSolver.h
/// @brief SatSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

class SatSolverImpl;
class SatLogger;

//////////////////////////////////////////////////////////////////////
/// @class SatSolver SatSolver.h "ym/SatSolver.h"
/// @ingroup SatGroup
/// @brief CNF-SAT エンジンのインターフェイスを表すクラス
//////////////////////////////////////////////////////////////////////
class SatSolver
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] type 実装タイプを表す文字列
  /// @param[in] option オプション文字列
  /// @param[in] rec_out ログを記録するストリームへのポインタ
  SatSolver(const string& type = string(),
	    const string& option = string(),
	    ostream* rec_out = nullptr);

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
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  SatVarId
  new_var(bool decision = true);

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 1項の節(リテラル)を追加する．
  void
  add_clause(SatLiteral lit1);

  /// @brief 2項の節を追加する．
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2);

  /// @brief 3項の節を追加する．
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3);

  /// @brief 4項の節を追加する．
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4);

  /// @brief 5項の節を追加する．
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4,
	     SatLiteral lit5);

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 動作制御を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief SAT 問題を解く．
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @retval kB3True  充足した．
  /// @retval kB3False 充足不能が判明した．
  /// @retval kB3X     わからなかった．
  ///
  /// i 番めの変数の割り当て結果は model[i] に入る．
  /// 通常は complete な割り当てなので値は true/false だけになるはず
  SatBool3
  solve(vector<SatBool3>& model);

  /// @brief assumption 付きの SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @retval kSat 充足した．
  /// @retval kUnsat 充足不能が判明した．
  /// @retval kUndet わからなかった．
  ///
  /// i 番めの変数の割り当て結果は model[i] に入る．
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	vector<SatBool3>& model);

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
  ymuint64
  set_max_conflict(ymuint64 val);

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

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const;

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  void
  get_stats(SatStats& stats) const;

  /// @brief 変数の数を得る．
  ymuint
  variable_num() const;

  /// @brief 制約節の数を得る．
  ymuint
  clause_num() const;

  /// @brief 制約節のリテラルの総数を得る．
  ymuint
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
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実際の機能を実装しているクラス
  SatSolverImpl* mImpl;

  // ロガー
  SatLogger* mLogger;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATSOLVER_H
