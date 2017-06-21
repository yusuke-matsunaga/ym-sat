#ifndef SATLOGGER_H
#define SATLOGGER_H

/// @file SatLogger.h
/// @brief SatLogger のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatLogger SatLogger.h "SatLogger.h"
/// @brief SatSolver の動作を記録するクラス
//////////////////////////////////////////////////////////////////////
class SatLogger
{
public:

  /// @brief コンストラクタ
  SatLogger();

  /// @brief デストラクタ
  virtual
  ~SatLogger();


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
  new_variable(SatVarId);

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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_YM_SAT

#endif // SATLOGGER_H
