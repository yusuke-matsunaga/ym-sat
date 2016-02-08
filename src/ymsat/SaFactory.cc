﻿
/// @file SaFactory.cc
/// @brief SaFactory の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 Yusuke Matsunaga
/// All rights reserved.


#include "SaUIP1.h"
#include "SaUIP2.h"


BEGIN_NAMESPACE_YM_SAT

// @brief SatAnalyzerの派生クラスを生成する．
// @param[in] solver SATソルバ
// @param[in] option どのクラスを生成するかを決めるオプション文字列
SatAnalyzer*
SaFactory::gen_analyzer(YmSat* solver,
			const string& option)
{
  if ( option == "uip1" ) {
    return new SaUIP1(solver);
  }
  if ( option == "uip2" ) {
    return new SaUIP2(solver);
  }
  // default fall-back
  return new SaUIP1(solver);
}

END_NAMESPACE_YM_SAT
