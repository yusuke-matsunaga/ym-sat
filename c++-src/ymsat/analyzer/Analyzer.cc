﻿
/// @file Analyzer.cc
/// @brief Analyzer の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Analyzer.h"
#include "SaUIP1.h"
#include "SaUIP2.h"


BEGIN_NAMESPACE_YM_SAT

// @brief Analyzerの派生クラスを生成する．
Analyzer*
Analyzer::new_obj(
  SatCore& core,
  const Json::Value& js_obj
)
{
  if ( js_obj.isMember("analyzer") ) {
    auto type = js_obj["analyzer"].asString();
    if ( type == "uip1" ) {
      return new SaUIP1{core};
    }
    if ( type == "uip2" ) {
      return new SaUIP2{core};
    }
    cerr << type << ": Unknown type, ignored." << endl;
  }
  // default fall-back
  return new SaUIP1{core};
}

END_NAMESPACE_YM_SAT
