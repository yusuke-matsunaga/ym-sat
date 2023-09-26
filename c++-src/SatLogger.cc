
/// @file SatLogger.cc
/// @brief SatLogger の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 2018 Yusuke Matsunaga
/// All rights reserved.

#include "SatLogger.h"
#include "SatLoggerS.h"
#include "ym/SatLiteral.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatLogger
//////////////////////////////////////////////////////////////////////

// @brief 新しいインスタンスを生成するクラスメソッド
unique_ptr<SatLogger>
SatLogger::new_impl(
  const JsonValue& js_obj
)
{
  if ( js_obj.has_key("log") && js_obj["log"].is_object() ) {
    const auto& log_obj = js_obj["log"];
    if ( log_obj.has_key("file") ) {
      auto log_file = log_obj["file"].get_string();
      auto fs = new ofstream{log_file};
      return unique_ptr<SatLogger>{new SatLoggerS{fs}};
    }
    if ( log_obj.has_key("stdout") && log_obj["stdout"].get_bool() ) {
      return unique_ptr<SatLogger>{new SatLoggerS{&cout}};
    }
    if ( log_obj.has_key("stderr") && log_obj["stderr"].get_bool() ) {
      return unique_ptr<SatLogger>{new SatLoggerS{&cerr}};
    }
  }
  // デフォルトフォールバック
  return unique_ptr<SatLogger>{new SatLogger};
}

// @brief 変数を追加する．
void
SatLogger::new_variable(
  SatLiteral lit
)
{
}

// @brief 節を追加する．
void
SatLogger::add_clause(
  const vector<SatLiteral>& lits
)
{
}

// @brief assumption 付きの SAT 問題を解く．
void
SatLogger::solve(
  const vector<SatLiteral>& assumptions
)
{
}

END_NAMESPACE_YM_SAT
