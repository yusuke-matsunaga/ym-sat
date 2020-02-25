#ifndef SATLOGGERS_H
#define SATLOGGERS_H

/// @file SatLoggerS.h
/// @brief SatLoggerS のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
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
  ~SatLoggerS();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を追加する．
  /// @param[in] lit 変数を表すリテラル
  void
  new_variable(SatLiteral lit) override;

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  void
  add_clause(const vector<SatLiteral>& lits) override;

  /// @brief assumption 付きの SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @retval kSat 充足した．
  /// @retval kUnsat 充足不能が判明した．
  /// @retval kUndet わからなかった．
  ///
  /// i 番めの変数の割り当て結果は model[i] に入る．
  void
  solve(const vector<SatLiteral>& assumptions) override;


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
