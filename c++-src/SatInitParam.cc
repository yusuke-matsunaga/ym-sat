
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

// @brief 空のコンストラクタ
SatInitParam::SatInitParam()
{
  //  1. 環境変数 YMSAT_CONF が設定されている場合，${YMSAT_CONF} の値
  //     を設定用の json ファイルとみなして読み込む．
  //     ファイルの読み込みに失敗した場合スキップする．
  {
    auto filename = getenv("YMSAT_CONF");
    if ( filename != nullptr ) {
      if ( read(filename) ) {
	// 成功
	return;
      }
    }
  }
  //  2. 環境変数 YMSAT_CONFDIR が設定されている場合，
  //     ${YMSAT_CONFDIR}/ymsat.json を設定用の json ファイルとみなして
  //     読み込む．
  //     ファイルの読み込みに失敗した場合スキップする．
  {
    auto dirname = getenv("YMSAT_CONFDIR");
    if ( dirname != nullptr ) {
      auto filename = string{dirname} + "/" + "ymsat.json";
      if ( read(filename) ) {
	// 成功
	return;
      }
    }
  }
  //  3. カレントディレクトリに ymsat.json ファイルがあれば読み込む
  //     ファイルの読み込みに失敗した場合スキップする．
  {
    auto filename = "./ymsat.json";
    if ( read(filename) ) {
      // 成功
      return;
    }
  }
  //  4. ハードコードされたデフォルト値を用いる．
  const char* conf_str = "{"
    "  'type': 'ymsat2'"
    "}";
  mJsObj = JsonValue::parse(conf_str);
}

// @brief コンストラクタ(タイプを表す文字列)
SatInitParam::SatInitParam(
  const string& type
)
{
  ostringstream buf;
  buf << "{'type': '" << type << "'}";
  mJsObj = JsonValue::parse(buf.str());
  check_type();
}

// @brief コンストラクタ(タイプを表す文字列)
SatInitParam::SatInitParam(
  const char* type
)
{
  ostringstream buf;
  buf << "{'type': '" << type << "'}";
  mJsObj = JsonValue::parse(buf.str());
  check_type();
}

// @brief コンストラクタ(Jsonオブジェクト)
SatInitParam::SatInitParam(
  const JsonValue& js_obj
) : mJsObj{js_obj}
{
  check_type();
}

// @brief json ファイルを読み込む．
SatInitParam
SatInitParam::from_json(
  const string& filename
)
{
  SatInitParam param;
  if ( !param.read(filename) ) {
    ostringstream buf;
    buf << "SatInitParam::from_json: Could not open file: " << filename;
    throw std::invalid_argument{buf.str()};
  }
  return param;
}

// @brief 実装タイプを返す．
string
SatInitParam::type() const
{
  if ( mJsObj.is_object() && mJsObj.has_key("type") ) {
    return mJsObj["type"].get_string();
  }
  // 未定義
  return string{};
}

// @brief json ファイルを読み込む．
bool
SatInitParam::read(
  const string& filename
)
{
  try {
    mJsObj = JsonValue::read(filename);
    return true;
  }
  catch ( std::invalid_argument err ) {
    return false;
  }
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
  else if ( t == "ymsat" ) {
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
  else {
    ostringstream buf;
    buf << "SatInitParam: unknown type '" << t << "', '";
    throw std::invalid_argument{buf.str()};
  }
}

END_NAMESPACE_YM_SAT
