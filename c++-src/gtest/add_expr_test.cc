
/// @file add_expr_test.cc
/// @brief add_expr_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "ym/SatSolver.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM

class SatSolverTest:
  public ::testing::Test
{
public:

  SatSolver mSolver;

};

TEST_F(SatSolverTest, add_expr_const0)
{
  auto expr = Expr::zero();
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{};

  EXPECT_THROW( mSolver.add_expr(expr, lit_map), std::logic_error );
}

TEST_F(SatSolverTest, add_expr_const1)
{
  auto expr = Expr::one();
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{};

  auto lits = mSolver.add_expr(expr, lit_map);
  EXPECT_TRUE( lits.empty() );
}

TEST_F(SatSolverTest, add_expr_lit1)
{
  auto expr = Expr::literal(0);
  auto lit0 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{{0, lit0}};

  auto lits = mSolver.add_expr(expr, lit_map);
  ASSERT_EQ( 1, lits.size() );
  EXPECT_EQ( lit0, lits.front() );
}

TEST_F(SatSolverTest, add_expr_lit2)
{
  auto expr = Expr::literal(1, true);
  auto lit0 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{{1, lit0}};

  auto lits = mSolver.add_expr(expr, lit_map);
  ASSERT_EQ( 1, lits.size() );
  EXPECT_EQ( ~lit0, lits.front() );
}

TEST_F(SatSolverTest, add_expr_and1)
{
  auto expr_lit0 = Expr::literal(0);
  auto expr_lit1 = Expr::literal(1);
  auto expr = expr_lit0 & expr_lit1;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{
    {0, lit0},
    {1, lit1}
  };

  auto lits = mSolver.add_expr(expr, lit_map);
  ASSERT_EQ( 2, lits.size() );
  EXPECT_EQ( lit0, lits[0] );
  EXPECT_EQ( lit1, lits[1] );
}

TEST_F(SatSolverTest, add_expr_or1)
{
  auto expr_lit0 = Expr::literal(0);
  auto expr_lit1 = Expr::literal(1);
  auto expr = expr_lit0 | expr_lit1;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{
    {0, lit0},
    {1, lit1}
  };

  auto lits = mSolver.add_expr(expr, lit_map);
  ASSERT_EQ( 1, lits.size() );
  auto lit2 = lits.front();

  for ( SizeType b = 0; b < 4; ++ b ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(3);
    assumptions.push_back(lit2);
    if ( b % 2 ) {
      assumptions.push_back(lit0);
    }
    else {
      assumptions.push_back(~lit0);
    }
    if ( b / 2 ) {
      assumptions.push_back(lit1);
    }
    else {
      assumptions.push_back(~lit1);
    }
    auto res = mSolver.solve(assumptions);
    auto exp_res = ( b != 0 ) ? SatBool3::True : SatBool3::False;
    EXPECT_EQ( exp_res, res );
  }
}

TEST_F(SatSolverTest, add_expr_xor1)
{
  auto expr_lit0 = Expr::literal(0);
  auto expr_lit1 = Expr::literal(1);
  auto expr = expr_lit0 ^ expr_lit1;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{
    {0, lit0},
    {1, lit1}
  };

  auto lits = mSolver.add_expr(expr, lit_map);
  ASSERT_EQ( 1, lits.size() );
  auto lit2 = lits.front();

  for ( SizeType b = 0; b < 4; ++ b ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(3);
    assumptions.push_back(lit2);
    if ( b % 2 ) {
      assumptions.push_back(lit0);
    }
    else {
      assumptions.push_back(~lit0);
    }
    if ( b / 2 ) {
      assumptions.push_back(lit1);
    }
    else {
      assumptions.push_back(~lit1);
    }
    auto res = mSolver.solve(assumptions);
    auto exp_res = ( b == 1 || b == 2 ) ? SatBool3::True : SatBool3::False;
    EXPECT_EQ( exp_res, res );
  }
}

TEST_F(SatSolverTest, add_expr_andor1)
{
  auto expr_lit0 = Expr::literal(0);
  auto expr_lit1 = Expr::literal(1);
  auto expr_lit2 = Expr::literal(2);
  auto expr_lit3 = Expr::literal(3);
  auto expr = (expr_lit0 & expr_lit1) | (expr_lit2 & expr_lit3);
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  auto lit2 = mSolver.new_variable(true);
  auto lit3 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{
    {0, lit0},
    {1, lit1},
    {2, lit2},
    {3, lit3}
  };

  auto lits = mSolver.add_expr(expr, lit_map);
  ASSERT_EQ( 1, lits.size() );
  auto lit_o = lits.front();

  for ( SizeType b = 0; b < 16; ++ b ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(5);
    assumptions.push_back(lit_o);
    if ( b % 2 ) {
      assumptions.push_back(lit0);
    }
    else {
      assumptions.push_back(~lit0);
    }
    if ( (b >> 1) % 2 ) {
      assumptions.push_back(lit1);
    }
    else {
      assumptions.push_back(~lit1);
    }
    if ( (b >> 2) % 2 ) {
      assumptions.push_back(lit2);
    }
    else {
      assumptions.push_back(~lit2);
    }
    if ( (b >> 3) % 2 ) {
      assumptions.push_back(lit3);
    }
    else {
      assumptions.push_back(~lit3);
    }
    auto res = mSolver.solve(assumptions);
    auto exp_res = ( (b % 4) == 3 || ((b >>2) % 4) == 3 ) ?
      SatBool3::True : SatBool3::False;
    EXPECT_EQ( exp_res, res );
  }
}

END_NAMESPACE_YM
