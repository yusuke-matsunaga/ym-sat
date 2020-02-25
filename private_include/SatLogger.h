#ifndef SATLOGGER_H
#define SATLOGGER_H

/// @file SatLogger.h
/// @brief SatLogger のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatLogger SatLogger.h "SatLogger.h"
/// @brief SatSolver の動作を記録するクラス
//////////////////////////////////////////////////////////////////////
class SatLogger
{
public:

  /// @brief 新しいインスタンスを生成するクラスメソッド
  /// @param[in] solver_type SATソルバのタイプ
  static
  unique_ptr<SatLogger>
  new_impl(const SatSolverType& solver_type);

  /// @brief コンストラクタ
  SatLogger() = default;

  /// @brief デストラクタ
  virtual
  ~SatLogger() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を追加する．
  /// @param[in] lit 変数を表すリテラル
  virtual
  void
  new_variable(SatLiteral lit);

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  virtual
  void
  add_clause(const vector<SatLiteral>& lits);

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


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SAT

#endif // SATLOGGER_H
