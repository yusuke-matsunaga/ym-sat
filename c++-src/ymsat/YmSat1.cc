
/// @file libym_logic/sat/ymsat/YmSat1.cc
/// @brief YmSat1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "YmSat1.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// YmSat1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSat1::YmSat1(
  const string& option
)
{
  set_controller("minisat1");
  set_analyzer(option);
  set_selecter("type:nega, phase_cache:");
}

// @brief デストラクタ
YmSat1::~YmSat1()
{
}

END_NAMESPACE_YM_SAT
