
/// @file SatLiteralTest.cc
/// @brief SatLiteralTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM

TEST(SatLiteralTest, empty_constructor)
{
  SatLiteral lit1;

  EXPECT_FALSE( lit1.is_valid() );

  EXPECT_EQ( kSatLiteralX, lit1 );
}

TEST(SatLiteralTest,  simple_constructor)
{
  for (int val = 0; val < 10; ++ val) {
    SatLiteral lit1(val);

    EXPECT_TRUE( lit1.is_valid() );
    EXPECT_EQ( val, lit1.varid() );
    EXPECT_EQ( true, lit1.is_positive() );
    EXPECT_EQ( false, lit1.is_negative() );
  }
}

END_NAMESPACE_YM
