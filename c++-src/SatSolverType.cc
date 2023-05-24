
/// @file SatSolverType.cc
/// @brief SatSolverType の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// SatSolverType
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ(タイプを表す文字列)
SatSolverType::SatSolverType(
  const string& type
)
{
  mJsObj["type"] = Json::Value{type};
  check_type();
}

// @brief コンストラクタ(Jsonオブジェクト)
SatSolverType::SatSolverType(
  const Json::Value& js_obj
) : mJsObj{js_obj}
{
  check_type();
}

// @brief 実装タイプを返す．
const string&
SatSolverType::type() const
{
  if ( mJsObj.isMember("type") ) {
    return
}

SatSolverType::SatSolverType(
  const string& type,
  const string& option,
  ostream* log_out
) : mType{type},
    mOption{option},
    mLogOut{log_out}
{
}

// @brief type のチェックを行う．
void
SatSolverType::check_type()
{
  if ( mType == "minisat" ) {
    // minisat-1.4
    ;
  }
  else if ( mType == "minisat2" ) {
    // minisat-2.2
    ;
  }
  else if ( mType == "glueminisat2" ) {
    // glueminisat-2.2.8
    ;
  }
  else if ( mType == "lingeling" ) {
    ;
  }
  else if ( mType == "ymsat1" ) {
    ;
  }
  else if ( mType == "ymsat2" ) {
    ;
  }
  else if ( mType == "ymsat1_old" ) {
    ;
  }
  else if ( mType == "" ) {
    // lingeling 今はデフォルトにしている．
    mType = "lingeling";
  }
  else {
    mType = "lingeling";
    cerr << "SatSolver: unknown type '" << type << "', '"
	 << mType << "' is used, instead." << endl;
  }
}

END_NAMESPACE_YM_SAT
