
/// @file conflict_test.cc
/// @brief SatSolver の矛盾解析テスト
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"


BEGIN_NAMESPACE_YM

TEST_P(SatTestFixture, conflict_test1)
{
  auto ilit1 = mVarList[0];
  auto ilit2 = mVarList[1];
  auto olit1 = mVarList[2];
  auto olit2 = mVarList[3];

  mSolver.add_andgate(olit1, ilit1, ilit2);
  mSolver.add_nandgate(olit2, ilit1, ilit2);

  vector<SatLiteral> assumptions{olit1, olit2};
  auto r = mSolver.solve(assumptions);

  EXPECT_EQ( SatBool3::False, r );

  auto& conf_lits = mSolver.conflict_literals();
  EXPECT_EQ( 2, conf_lits.size() );
  EXPECT_EQ( ~olit1, conf_lits[0] );
  EXPECT_EQ( ~olit2, conf_lits[1] );
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 SatTestFixture,
			 ::testing::Values("glueminisat2", "minisat2",
					   "ymsat1", "ymsat2"));

END_NAMESPACE_YM
