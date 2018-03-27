
/// @file DimacsTest.cc
/// @brief SatDimacst のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatDimacs.h"


BEGIN_NAMESPACE_YM

TEST(DimacsTest, null)
{
  SatDimacs dimacs;

  EXPECT_EQ( 0, dimacs.variable_num() );
  EXPECT_EQ( 0, dimacs.clause_num() );
  EXPECT_TRUE( dimacs.clause_list().empty() );
}

TEST(DimacsTest, add_clause1)
{
  SatDimacs dimacs;

  vector<int> tmp_lits;
  tmp_lits.push_back(1);

  dimacs.add_clause(tmp_lits);

  EXPECT_EQ( 1, dimacs.variable_num() );
  EXPECT_EQ( 1, dimacs.clause_num() );

  const vector<int>& lits1 = dimacs.clause_list()[0];
  EXPECT_EQ( 1, lits1.size() );
  EXPECT_EQ( 1, lits1[0] );
}

TEST(DimacsTest, add_clause2)
{
  SatDimacs dimacs;

  vector<int> tmp_lits;
  tmp_lits.push_back(1);
  tmp_lits.push_back(-2);

  dimacs.add_clause(tmp_lits);

  EXPECT_EQ( 1, dimacs.variable_num() );
  EXPECT_EQ( 1, dimacs.clause_num() );

  const vector<int>& lits1 = dimacs.clause_list()[0];
  EXPECT_EQ( 2, lits1.size() );
  EXPECT_EQ( 1, lits1[0] );
  EXPECT_EQ( -2, lits1[1] );
}

TEST(DimacsTest, read_dimacs)
{
}

END_NAMESPACE_YM
