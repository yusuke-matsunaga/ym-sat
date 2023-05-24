#ifndef SATLOGGER_H
#define SATLOGGER_H

/// @file SatLogger.h
/// @brief SatLogger のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include <json/json.h>


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatLogger SatLogger.h "SatLogger.h"
/// @brief SatSolver の動作を記録するクラス
//////////////////////////////////////////////////////////////////////
class SatLogger
{
public:

  /// @brief 新しいインスタンスを生成するクラスメソッド
  static
  unique_ptr<SatLogger>
  new_impl(
    const Json::Value& js_obj ///< [in] 初期化パラメータ
  );

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
  virtual
  void
  new_variable(
    SatLiteral lit ///< [in] 変数を表すリテラル
  );

  /// @brief 節を追加する．
  virtual
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] リテラルのベクタ
  );

  /// @brief assumption 付きの SAT 問題を解く．
  virtual
  void
  solve(
    const vector<SatLiteral>& assumptions ///< [in] あらかじめ仮定する変数の値割り当てリスト
  );

};

END_NAMESPACE_YM_SAT

#endif // SATLOGGER_H
