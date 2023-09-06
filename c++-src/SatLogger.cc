
/// @file SatLogger.cc
/// @brief SatLogger の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SatLogger.h"
#include "SatLoggerS.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatLogger
//////////////////////////////////////////////////////////////////////

// @brief 新しいインスタンスを生成するクラスメソッド
unique_ptr<SatLogger>
SatLogger::new_impl(
  const Json::Value& js_obj
)
{
  if ( js_obj.isMember("log") && js_obj["log"].isObject() ) {
    const auto& log_obj = js_obj["log"];
    if ( log_obj.isMember("file") ) {
      auto log_file = log_obj["file"].asString();
      auto fs = new ofstream{log_file};
      return unique_ptr<SatLogger>{new SatLoggerS{fs}};
    }
    if ( log_obj.isMember("stdout") && log_obj["stdout"].asBool() ) {
      return unique_ptr<SatLogger>{new SatLoggerS{&cout}};
    }
    if ( log_obj.isMember("stderr") && log_obj["stderr"].asBool() ) {
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
