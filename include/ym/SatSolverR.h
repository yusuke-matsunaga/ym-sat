#ifndef YM_SATSOLVERR_H
#define YM_SATSOLVERR_H

/// @file ym/SatSolverR.h
/// @brief SatSolverR のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatSolverR SatSolverR.h "ym/SatSolverR.h"
/// @ingroup SatGroup
/// @brief CNF-SAT エンジンのインターフェイスを表すクラス
///
/// このクラスはログを記録する．
//////////////////////////////////////////////////////////////////////
class SatSolverR :
  public SatSolver
{
public:

  /// @brief コンストラクタ
  /// @param[in] type 実装タイプを表す文字列
  /// @param[in] option オプション文字列
  /// @param[in] rec_out ログを記録するストリームへのポインタ
  SatSolverR(const string& type = string(),
	     const string& option = string(),
	     ostream* rec_out = nullptr);

  /// @brief デストラクタ
  ~SatSolverR();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //
  // 内部状態を変更させるメソッド群
  // rec_outp が nullptr でない時はログに記録する．
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

  /// @brief 学習節をすべて削除する．
  void
  forget_learnt_clause();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief リテラルをログに出力する．
  /// @param[in] lit リテラル
  ///
  /// この関数は mRecOut != nullptr を仮定している．
  void
  put_lit(SatLiteral lit) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ログ出力用のストリーム
  // 記録しない時は nullptr
  ostream* mRecOut;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATSOLVERR_H
