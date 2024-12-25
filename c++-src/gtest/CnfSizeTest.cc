
/// @file CnfSizeTest.cc
/// @brief CnfSize のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "ym/CnfSize.h"


BEGIN_NAMESPACE_YM

// 空のコンストラクタ
TEST(CnfSizeTest, empty_constr)
{
  auto size = CnfSize{};

  EXPECT_EQ( 0, size.clause_num );
  EXPECT_EQ( 0, size.literal_num );
}

// zero()
TEST(CnfSizeTest, zero)
{
  auto size = CnfSize::zero();

  EXPECT_EQ( 0, size.clause_num );
  EXPECT_EQ( 0, size.literal_num );
}

// 値を指定したコンストラクタ
TEST(CnfSizeTest, constr1)
{
  SizeType cnum = 2;
  SizeType lnum = 4;
  auto size = CnfSize{cnum, lnum};

  EXPECT_EQ( cnum, size.clause_num );
  EXPECT_EQ( lnum, size.literal_num );
}

// 加算
TEST(CnfSizeTest, add)
{
  SizeType cnum1 = 2;
  SizeType lnum1 = 4;
  auto size1 = CnfSize{cnum1, lnum1};

  SizeType cnum2 = 3;
  SizeType lnum2 = 9;
  auto size2 = CnfSize{cnum2, lnum2};

  auto size = size1 + size2;

  EXPECT_EQ( cnum1 + cnum2, size.clause_num );
  EXPECT_EQ( lnum1 + lnum2, size.literal_num );
}

// 加算つき代入
TEST(CnfSizeTest, add_int)
{
  SizeType cnum1 = 2;
  SizeType lnum1 = 4;
  auto size = CnfSize{cnum1, lnum1};

  SizeType cnum2 = 3;
  SizeType lnum2 = 9;
  auto size1 = CnfSize{cnum2, lnum2};

  size += size1;

  EXPECT_EQ( cnum1 + cnum2, size.clause_num );
  EXPECT_EQ( lnum1 + lnum2, size.literal_num );
}

// 減算
TEST(CnfSizeTest, sub)
{
  SizeType cnum1 = 12;
  SizeType lnum1 = 14;
  auto size1 = CnfSize{cnum1, lnum1};

  SizeType cnum2 = 3;
  SizeType lnum2 = 9;
  auto size2 = CnfSize{cnum2, lnum2};

  auto size = size1 - size2;

  EXPECT_EQ( cnum1 - cnum2, size.clause_num );
  EXPECT_EQ( lnum1 - lnum2, size.literal_num );
}

// 減算つき代入
TEST(CnfSizeTest, sub_int)
{
  SizeType cnum1 = 12;
  SizeType lnum1 = 14;
  auto size = CnfSize{cnum1, lnum1};

  SizeType cnum2 = 3;
  SizeType lnum2 = 9;
  auto size1 = CnfSize{cnum2, lnum2};

  size -= size1;

  EXPECT_EQ( cnum1 - cnum2, size.clause_num );
  EXPECT_EQ( lnum1 - lnum2, size.literal_num );
}

// 乗算
TEST(CnfSizeTest, mult)
{
  SizeType cnum1 = 12;
  SizeType lnum1 = 14;
  auto size1 = CnfSize{cnum1, lnum1};

  SizeType m = 5;

  auto size = size1 * m;

  EXPECT_EQ( cnum1 * m, size.clause_num );
  EXPECT_EQ( lnum1 * m, size.literal_num );
}

// 乗算つき代入
TEST(CnfSizeTest, mult_int)
{
  SizeType cnum1 = 12;
  SizeType lnum1 = 14;
  auto size = CnfSize{cnum1, lnum1};

  SizeType m = 5;

  size *= m;

  EXPECT_EQ( cnum1 * m, size.clause_num );
  EXPECT_EQ( lnum1 * m, size.literal_num );
}

// 等価比較
TEST(CnfSizeType, eq)
{
  auto size1 = CnfSize{1, 1};
  auto size2 = CnfSize{1, 2};
  auto size3 = CnfSize{1, 1};

  EXPECT_FALSE( size1 == size2 );
  EXPECT_TRUE( size1 == size3 );
  EXPECT_FALSE( size2 == size3 );
}

END_NAMESPACE_YM
