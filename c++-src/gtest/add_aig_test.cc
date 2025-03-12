
/// @file add_aig_test.cc
/// @brief add_aig_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "ym/SatSolver.h"
#include "ym/AigMgr.h"


BEGIN_NAMESPACE_YM

class SatSolverTest:
  public ::testing::Test
{
public:

  AigMgr mMgr;

  SatSolver mSolver;

};

TEST_F(SatSolverTest, add_aig_const0)
{
  auto aig = mMgr.make_zero();
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{};

  EXPECT_THROW( mSolver.add_aig(aig, lit_map), std::logic_error );
}

TEST_F(SatSolverTest, add_aig_const1)
{
  auto aig = mMgr.make_one();
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{};

  auto lits = mSolver.add_aig(aig, lit_map);
  EXPECT_TRUE( lits.empty() );
}

TEST_F(SatSolverTest, add_aig_lit1)
{
  auto aig = mMgr.make_input();
  auto lit0 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{{aig.input_id(), lit0}};

  auto lits = mSolver.add_aig(aig, lit_map);
  ASSERT_EQ( 1, lits.size() );
  EXPECT_EQ( lit0, lits.front() );
}

TEST_F(SatSolverTest, add_aig_lit2)
{
  auto aig = mMgr.make_input();
  auto lit0 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{{aig.input_id(), lit0}};

  auto lits = mSolver.add_aig(~aig, lit_map);
  ASSERT_EQ( 1, lits.size() );
  EXPECT_EQ( ~lit0, lits.front() );
}

TEST_F(SatSolverTest, add_aig_and1)
{
  auto aig_lit0 = mMgr.make_input();
  auto aig_lit1 = mMgr.make_input();
  auto aig = aig_lit0 & aig_lit1;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{
    {aig_lit0.input_id(), lit0},
    {aig_lit1.input_id(), lit1}
  };

  auto lits = mSolver.add_aig(aig, lit_map);
  ASSERT_EQ( 2, lits.size() );
  EXPECT_EQ( lit0, lits[0] );
  EXPECT_EQ( lit1, lits[1] );
}

TEST_F(SatSolverTest, add_aig_or1)
{
  auto aig_lit0 = mMgr.make_input();
  auto aig_lit1 = mMgr.make_input();
  auto aig = aig_lit0 | aig_lit1;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{
    {aig_lit0.input_id(), lit0},
    {aig_lit1.input_id(), lit1}
  };

  auto lits = mSolver.add_aig(aig, lit_map);
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

TEST_F(SatSolverTest, add_aig_xor1)
{
  auto aig_lit0 = mMgr.make_input();
  auto aig_lit1 = mMgr.make_input();
  auto aig = aig_lit0 ^ aig_lit1;
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{
    {aig_lit0.input_id(), lit0},
    {aig_lit1.input_id(), lit1}
  };

  auto lits = mSolver.add_aig(aig, lit_map);
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

TEST_F(SatSolverTest, add_aig_andor1)
{
  auto aig_lit0 = mMgr.make_input();
  auto aig_lit1 = mMgr.make_input();
  auto aig_lit2 = mMgr.make_input();
  auto aig_lit3 = mMgr.make_input();
  auto aig = (aig_lit0 & aig_lit1) | (aig_lit2 & aig_lit3);
  auto lit0 = mSolver.new_variable(true);
  auto lit1 = mSolver.new_variable(true);
  auto lit2 = mSolver.new_variable(true);
  auto lit3 = mSolver.new_variable(true);
  auto lit_map = std::unordered_map<SizeType, SatLiteral>{
    {aig_lit0.input_id(), lit0},
    {aig_lit1.input_id(), lit1},
    {aig_lit2.input_id(), lit2},
    {aig_lit3.input_id(), lit3}
  };

  auto lits = mSolver.add_aig(aig, lit_map);
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
