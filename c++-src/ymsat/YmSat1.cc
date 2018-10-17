
/// @file libym_logic/sat/ymsat/YmSat1.cc
/// @brief YmSat1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ymsat/YmSat1.h"
#include "ymsat/Controller.h"
#include "ymsat/Analyzer.h"
#include "ymsat/Selecter.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// YmSat1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSat1::YmSat1(const string& option)
{
  set_controller(new_ControllerMS1(mgr()));
  set_analyzer(SaFactory::gen_analyzer(mgr(), option));
  set_selecter(new_Selecter1(mgr(), true));
}

// @brief デストラクタ
YmSat1::~YmSat1()
{
}

END_NAMESPACE_YM_SAT
