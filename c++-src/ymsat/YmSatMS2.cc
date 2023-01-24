
/// @file YmSatMS2.cc
/// @brief YmSatMS2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "YmSatMS2.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// YmSatMS2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSatMS2::YmSatMS2(
  const string& option
)
{
  set_controller("minisat2");
  set_analyzer("uip1");
  set_selecter("type:nega, phase_cache:");
}

// @brief デストラクタ
YmSatMS2::~YmSatMS2()
{
}

END_NAMESPACE_YM_SAT
