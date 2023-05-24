
/// @file SatInitParam.cc
/// @brief SatInitParam の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// SatInitParam
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ(タイプを表す文字列)
SatInitParam::SatInitParam(
  const string& type
)
{
  mJsObj["type"] = Json::Value{type};
  check_type();
}

// @brief コンストラクタ(タイプを表す文字列)
SatInitParam::SatInitParam(
  const char* type
)
{
  mJsObj["type"] = Json::Value{type};
  check_type();
}

// @brief コンストラクタ(Jsonオブジェクト)
SatInitParam::SatInitParam(
  const Json::Value& js_obj
) : mJsObj{js_obj}
{
  check_type();
}

// @brief 実装タイプを返す．
string
SatInitParam::type() const
{
  if ( mJsObj.isMember("type") ) {
    return mJsObj["type"].asString();
  }
  // 未定義
  return string{};
}

// @brief type のチェックを行う．
void
SatInitParam::check_type()
{
  auto t = type();
  if ( t == "minisat" ) {
    // minisat-1.4
    ;
  }
  else if ( t == "minisat2" ) {
    // minisat-2.2
    ;
  }
  else if ( t == "glueminisat2" ) {
    // glueminisat-2.2.8
    ;
  }
  else if ( t == "lingeling" ) {
    ;
  }
  else if ( t == "ymsat1" ) {
    ;
  }
  else if ( t == "ymsat2" ) {
    ;
  }
  else if ( t == "ymsat1_old" ) {
    ;
  }
  else if ( t == string{} ) {
    // lingeling 今はデフォルトにしている．
    mJsObj["type"] = Json::Value{"lingelinge"};
  }
  else {
    ostringstream buf;
    buf << "SatInitParam: unknown type '" << t << "', '";
    throw std::invalid_argument{buf.str()};
  }
}

END_NAMESPACE_YM_SAT
