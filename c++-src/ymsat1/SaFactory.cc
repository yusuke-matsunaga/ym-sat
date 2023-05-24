
/// @file SaFactory.cc
/// @brief SaFactory の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SaUIP1.h"
#include "SaUIP2.h"


BEGIN_NAMESPACE_YM_SAT1

// @brief SatAnalyzerの派生クラスを生成する．
Analyzer*
SaFactory::gen_analyzer(
  YmSat* solver,
  const Json::Value& js_obj
)
{
  if ( js_obj.isMember("analyzer") ) {
    auto atype = js_obj["analyzer"].asString();
    if ( atype == "uip1" ) {
      return new SaUIP1{solver};
    }
    if ( atype == "uip2" ) {
      return new SaUIP2{solver};
    }
    ostringstream buf;
    buf << "SaFactory::gen_analyzer(): Unknown type: '" << atype << "'";
    throw std::invalid_argument{buf.str()};
  }
  // default fall-back
  return new SaUIP1{solver};
}

END_NAMESPACE_YM_SAT1
