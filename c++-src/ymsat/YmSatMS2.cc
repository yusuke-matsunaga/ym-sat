
/// @file YmSatMS2.cc
/// @brief YmSatMS2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ymsat/YmSatMS2.h"
#include "ymsat/Controller.h"
#include "ymsat/Analyzer.h"
#include "ymsat/Selecter.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// YmSatMS2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] option オプション文字列
YmSatMS2::YmSatMS2(const string& option)
{
  bool phase_cache = true;
  if ( option == "no_phase_cache" ) {
    phase_cache = false;
  }

  set_controller(new_ControllerMS2(mgr()));
  set_analyzer(SaFactory::gen_analyzer(mgr(), option));
  set_selecter(new_Selecter1(mgr(), phase_cache));
}

// @brief デストラクタ
YmSatMS2::~YmSatMS2()
{
}

END_NAMESPACE_YM_SAT
