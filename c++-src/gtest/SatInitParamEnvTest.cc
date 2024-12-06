
/// @file SatInitParamTest_1.cc
/// @brief SatInitParam のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_YM

TEST(SatInitParamTest, empty_const1)
{
  SatInitParam param;

  EXPECT_EQ( "glueminisat2", param.type() );

  auto js_obj = param.js_obj();
  EXPECT_EQ( "bar", js_obj["foo"].get_string() );
}

END_NAMESPACE_YM
