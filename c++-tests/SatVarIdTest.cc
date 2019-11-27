
/// @file SatVarIdTest.cc
/// @brief SatVarIdTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatVarId.h"


BEGIN_NAMESPACE_YM

TEST(SatVarIdTest, empty_constructor)
{
  SatVarId var1;

  EXPECT_FALSE( var1.is_valid() );
  EXPECT_EQ( kSatVarIdIllegal, var1 );
}

TEST(SatVarIdTest,  simple_constructor)
{
  for (int val = 0; val < 10; ++ val) {
    SatVarId var1(val);

    EXPECT_TRUE( var1.is_valid() );
    EXPECT_EQ( val, var1.val() );
  }
}

END_NAMESPACE_YM
