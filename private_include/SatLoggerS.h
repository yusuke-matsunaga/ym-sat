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
  SatLoggerS(
    ostream* s ///< [in] 出力先のストリーム
  );

  /// @brief デストラクタ
  ~SatLoggerS();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を追加する．
  void
  new_variable(
    SatLiteral lit
  ) override;

  /// @brief 節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits
  ) override;

  /// @brief assumption 付きの SAT 問題を解く．
  void
  solve(
    const vector<SatLiteral>& assumptions
  ) override;


  /// @brief solve() の結果を出力する．
  void
  solve_result(
    SatBool3 res ///< [in] 結果
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief リテラルをログに出力する．
  void
  put_lit(
    SatLiteral lit
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力先のストリーム
  ostream* mS;

};

END_NAMESPACE_YM_SAT

#endif // SATLOGGER_H
