
/// @file SaFactory.cc
/// @brief SaFactory の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "SaUIP1.h"
#include "SaUIP2.h"


BEGIN_NAMESPACE_YM_SAT

// @brief Analyzerの派生クラスを生成する．
Analyzer*
SaFactory::gen_analyzer(
  CoreMgr& mgr,
  const string& option
)
{
  if ( option == "uip1" ) {
    return new SaUIP1{mgr};
  }
  if ( option == "uip2" ) {
    return new SaUIP2{mgr};
  }
  // default fall-back
  return new SaUIP2{mgr};
}

END_NAMESPACE_YM_SAT
