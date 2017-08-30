#ifndef SATLOGGERS_H
#define SATLOGGERS_H

/// @file SatLoggerS.h
/// @brief SatLoggerS のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SatLogger.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatLoggerS SatLoggerS.h "SatLoggerS.h"
/// @brief SatSolver の動作を記録するクラス
//////////////////////////////////////////////////////////////////////
class SatLoggerS :
  public SatLogger
{
public:

  /// @brief コンストラクタ
  /// @param[in] s 出力先のストリーム
  SatLoggerS(ostream* s);

  /// @brief デストラクタ
  virtual
  ~SatLoggerS();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を追加する．
  /// @param[in] decision 決定変数の時に true とする．
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  virtual
  void
  new_variable(SatVarId id);

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  virtual
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 1項の節(リテラル)を追加する．
  virtual
  void
  add_clause(SatLiteral lit1);

  /// @brief 2項の節を追加する．
  virtual
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2);

  /// @brief 3項の節を追加する．
  virtual
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3);

  /// @brief 4項の節を追加する．
  virtual
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4);

  /// @brief 5項の節を追加する．
  virtual
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4,
	     SatLiteral lit5);

  /// @brief 6項の節を追加する．
  virtual
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4,
	     SatLiteral lit5,
	     SatLiteral lit6);

  /// @brief assumption 付きの SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @retval kSat 充足した．
  /// @retval kUnsat 充足不能が判明した．
  /// @retval kUndet わからなかった．
  ///
  /// i 番めの変数の割り当て結果は model[i] に入る．
  virtual
  void
  solve(const vector<SatLiteral>& assumptions);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief リテラルをログに出力する．
  /// @param[in] lit リテラル
  void
  put_lit(SatLiteral lit) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力先のストリーム
  ostream* mS;

};

END_NAMESPACE_YM_SAT

#endif // SATLOGGER_H
