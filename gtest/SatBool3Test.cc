
/// @file SatBool3Test.cc
/// @brief SatBool3Test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM

TEST(SatBool3Test, negation)
{
  EXPECT_EQ( SatBool3::True,  ~SatBool3::False );
  EXPECT_EQ( SatBool3::False, ~SatBool3::True );
  EXPECT_EQ( SatBool3::X,     ~SatBool3::X );
}

TEST(SatBool3RTest, xor_op)
{
  SatBool3 v_list[] = { SatBool3::False, SatBool3::True, SatBool3::X };

  for ( auto i: { 0, 1, 2 } ) {
    auto v1 = v_list[i];
    for ( auto j: { 0, 1, 2 } ) {
      auto v2 = v_list[j];
      auto v3 = v1 ^ v2;
      auto ans = SatBool3::X;
      if ( v1 == SatBool3::True ) {
	if ( v2 == SatBool3::True ) {
	  ans = SatBool3::False;
	}
	else if ( v2 == SatBool3::False ) {
	  ans = SatBool3::True;
	}
      }
      else if ( v1 == SatBool3::False ) {
	if ( v2 == SatBool3::True ) {
	  ans = SatBool3::True;
	}
	else if ( v2 == SatBool3::False ) {
	  ans = SatBool3::False;
	}
      }
      EXPECT_EQ( ans, v3 );
    }
  }
}

TEST(SatBool3Test,  xor_intop)
{
  SatBool3 v_list[] = { SatBool3::False, SatBool3::True, SatBool3::X };

  for ( auto i: { 0, 1, 2 } ) {
    auto v1 = v_list[i];
    for ( auto j: { 0, 1, 2 } ) {
      auto v2 = v_list[j];
      auto ans = v1 ^ v2;
      v1 ^= v2;
      EXPECT_EQ( ans, v1 );
    }
  }
}

TEST(SatBool3Test, b2B3)
{
  EXPECT_EQ( SatBool3::False, b2B3(false) );
  EXPECT_EQ( SatBool3::True,  b2B3(true) );
}

END_NAMESPACE_YM
