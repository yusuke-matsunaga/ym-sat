
/// @file AddClauseTest.cc
/// @brief AddClauseTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"


BEGIN_NAMESPACE_YM

TEST_P(SatTestFixture, add_clause0)
{
  vector<int> vals(
    {
     // lit1 ans
     //   0    1
     //   1    1
     1,
     1
    }
  );

  check(1, vals);
}

TEST_P(SatTestFixture, add_clause1_1)
{
  SatLiteral lit1(mVarList[0]);

  mSolver.add_clause(lit1);

  vector<int> vals(
    {
     // lit1 ans
     //   0    0
     //   1    1
     0,
     1
    }
  );

  check(1, vals);
}

TEST_P(SatTestFixture, add_clause1_2)
{
  SatLiteral lit1(mVarList[0]);

  mSolver.add_clause(~lit1);

  vector<int> vals(
    {
     // lit1 ans
     //   0    1
     //   1    0
     1,
     0
    }
  );

  check(1, vals);
}

TEST_P(SatTestFixture, add_clause2_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_clause(lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    0
     //   0    1    1
     //   1    0    1
     //   1    1    1
     0,
     1,
     1,
     1
    }
  );

  check(2, vals);
}

TEST_P(SatTestFixture, add_clause2_2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_clause(~lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    1
     //   0    1    0
     //   1    0    1
     //   1    1    1
     1,
     0,
     1,
     1
    }
  );

  check(2, vals);
}

TEST_P(SatTestFixture, add_clause3_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_clause(lit1, lit2, lit3);

  vector<int> vals(
    {
     // lit3 lit2 lit1 ans
     //   0    0    0    0
     //   0    0    1    1
     //   0    1    0    1
     //   0    1    1    1
     //   1    0    0    1
     //   1    0    1    1
     //   1    1    0    1
     //   1    1    1    1
     0,
     1,
     1,
     1,
     1,
     1,
     1,
     1
    }
  );

  check(3, vals);
}

TEST_P(SatTestFixture, add_clause3_2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_clause(~lit1, lit2, ~lit3);

  vector<int> vals(
    {
     // lit3 lit2 lit1 ans
     //   0    0    0    1
     //   0    0    1    1
     //   0    1    0    1
     //   0    1    1    1
     //   1    0    0    1
     //   1    0    1    0
     //   1    1    0    1
     //   1    1    1    1
     1,
     1,
     1,
     1,
     1,
     0,
     1,
     1
    }
  );

  check(3, vals);
}

TEST_P(SatTestFixture, add_clause4_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_clause(lit1, lit2, lit3, lit4);

  vector<int> vals(
    {
     // lit4 lit3 lit2 lit1 ans
     //   0    0    0    0    0
     //   0    0    0    1    1
     //   0    0    1    0    1
     //   0    0    1    1    1
     //   0    1    0    0    1
     //   0    1    0    1    1
     //   0    1    1    0    1
     //   0    1    1    1    1
     //   1    0    0    0    1
     //   1    0    0    1    1
     //   1    0    1    0    1
     //   1    0    1    1    1
     //   1    1    0    0    1
     //   1    1    0    1    1
     //   1    1    1    0    1
     //   1    1    1    1    1
     0,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1
    }
  );

  check(4, vals);
}

TEST_P(SatTestFixture, add_clause5_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_clause(lit1, lit2, lit3, lit4, lit5);

  vector<int> vals(
    {
     // lit5 lit4 lit3 lit2 lit1  ans
     //    0    0    0    0    0    0
     //    0    0    0    0    1    1
     //    0    0    0    1    0    1
     //    0    0    0    1    1    1
     //    0    0    1    0    0    1
     //    0    0    1    0    1    1
     //    0    0    1    1    0    1
     //    0    0    1    1    1    1
     //    0    1    0    0    0    1
     //    0    1    0    0    1    1
     //    0    1    0    1    0    1
     //    0    1    0    1    1    1
     //    0    1    1    0    0    1
     //    0    1    1    0    1    1
     //    0    1    1    1    0    1
     //    0    1    1    1    1    1
     //    1    0    0    0    0    1
     //    1    0    0    0    1    1
     //    1    0    0    1    0    1
     //    1    0    0    1    1    1
     //    1    0    1    0    0    1
     //    1    0    1    0    1    1
     //    1    0    1    1    0    1
     //    1    0    1    1    1    1
     //    1    1    0    0    0    1
     //    1    1    0    0    1    1
     //    1    1    0    1    0    1
     //    1    1    0    1    1    1
     //    1    1    1    0    0    1
     //    1    1    1    0    1    1
     //    1    1    1    1    0    1
     //    1    1    1    1    1    1
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
  );

  check(5, vals);
}

TEST_P(SatTestFixture, add_clause5n_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  vector<SatLiteral> tmp_lits(5);
  tmp_lits[0] = lit1;
  tmp_lits[1] = lit2;
  tmp_lits[2] = lit3;
  tmp_lits[3] = lit4;
  tmp_lits[4] = lit5;

  mSolver.add_clause(tmp_lits);

  vector<int> vals(
    {
     // lit5 lit4 lit3 lit2 lit1  ans
     //    0    0    0    0    0    0
     //    0    0    0    0    1    1
     //    0    0    0    1    0    1
     //    0    0    0    1    1    1
     //    0    0    1    0    0    1
     //    0    0    1    0    1    1
     //    0    0    1    1    0    1
     //    0    0    1    1    1    1
     //    0    1    0    0    0    1
     //    0    1    0    0    1    1
     //    0    1    0    1    0    1
     //    0    1    0    1    1    1
     //    0    1    1    0    0    1
     //    0    1    1    0    1    1
     //    0    1    1    1    0    1
     //    0    1    1    1    1    1
     //    1    0    0    0    0    1
     //    1    0    0    0    1    1
     //    1    0    0    1    0    1
     //    1    0    0    1    1    1
     //    1    0    1    0    0    1
     //    1    0    1    0    1    1
     //    1    0    1    1    0    1
     //    1    0    1    1    1    1
     //    1    1    0    0    0    1
     //    1    1    0    0    1    1
     //    1    1    0    1    0    1
     //    1    1    0    1    1    1
     //    1    1    1    0    0    1
     //    1    1    1    0    1    1
     //    1    1    1    1    0    1
     //    1    1    1    1    1    1
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
  );

  check(5, vals);
}

TEST_P(SatTestFixture, add_clause_with_cond1_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);

  mSolver.add_clause(lit1);

  vector<int> vals(
    {
     // lit1 ans
     //   0    0
     //   1    1
     0,
     1
    }
  );

  check_with_cond1(1, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(SatTestFixture, add_clause_with_cond1_2)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);

  mSolver.add_clause(~lit1);

  vector<int> vals(
    {
     // lit1 ans
     //   0    1
     //   1    0
     1,
     0
    }
  );

  check_with_cond1(1, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(SatTestFixture, add_clause_with_cond2_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_clause(lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    0
     //   0    1    1
     //   1    0    1
     //   1    1    1
     0,
     1,
     1,
     1
    }
  );

  check_with_cond1(2, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(SatTestFixture, add_clause_with_cond2_2)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_clause(~lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    1
     //   0    1    0
     //   1    0    1
     //   1    1    1
     1,
     0,
     1,
     1
    }
  );

  check_with_cond1(2, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(SatTestFixture, add_clause_with_cond3_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_clause(lit1, lit2, lit3);

  vector<int> vals(
    {
     // lit3 lit2 lit1 ans
     //   0    0    0    0
     //   0    0    1    1
     //   0    1    0    1
     //   0    1    1    1
     //   1    0    0    1
     //   1    0    1    1
     //   1    1    0    1
     //   1    1    1    1
     0,
     1,
     1,
     1,
     1,
     1,
     1,
     1
    }
  );

  check_with_cond1(3, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(SatTestFixture, add_clause_with_cond3_2)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_clause(~lit1, lit2, ~lit3);

  vector<int> vals(
    {
     // lit3 lit2 lit1 ans
     //   0    0    0    1
     //   0    0    1    1
     //   0    1    0    1
     //   0    1    1    1
     //   1    0    0    1
     //   1    0    1    0
     //   1    1    0    1
     //   1    1    1    1
     1,
     1,
     1,
     1,
     1,
     0,
     1,
     1
    }
  );

  check_with_cond1(3, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(SatTestFixture, add_clause_with_cond4_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_clause(lit1, lit2, lit3, lit4);

  vector<int> vals(
    {
     // lit4 lit3 lit2 lit1 ans
     //   0    0    0    0    0
     //   0    0    0    1    1
     //   0    0    1    0    1
     //   0    0    1    1    1
     //   0    1    0    0    1
     //   0    1    0    1    1
     //   0    1    1    0    1
     //   0    1    1    1    1
     //   1    0    0    0    1
     //   1    0    0    1    1
     //   1    0    1    0    1
     //   1    0    1    1    1
     //   1    1    0    0    1
     //   1    1    0    1    1
     //   1    1    1    0    1
     //   1    1    1    1    1
     0,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1
    }
  );

  check_with_cond1(4, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(SatTestFixture, add_clause_with_cond5_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_clause(lit1, lit2, lit3, lit4, lit5);

  vector<int> vals(
    {
     // lit5 lit4 lit3 lit2 lit1  ans
     //    0    0    0    0    0    0
     //    0    0    0    0    1    1
     //    0    0    0    1    0    1
     //    0    0    0    1    1    1
     //    0    0    1    0    0    1
     //    0    0    1    0    1    1
     //    0    0    1    1    0    1
     //    0    0    1    1    1    1
     //    0    1    0    0    0    1
     //    0    1    0    0    1    1
     //    0    1    0    1    0    1
     //    0    1    0    1    1    1
     //    0    1    1    0    0    1
     //    0    1    1    0    1    1
     //    0    1    1    1    0    1
     //    0    1    1    1    1    1
     //    1    0    0    0    0    1
     //    1    0    0    0    1    1
     //    1    0    0    1    0    1
     //    1    0    0    1    1    1
     //    1    0    1    0    0    1
     //    1    0    1    0    1    1
     //    1    0    1    1    0    1
     //    1    0    1    1    1    1
     //    1    1    0    0    0    1
     //    1    1    0    0    1    1
     //    1    1    0    1    0    1
     //    1    1    0    1    1    1
     //    1    1    1    0    0    1
     //    1    1    1    0    1    1
     //    1    1    1    1    0    1
     //    1    1    1    1    1    1
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
  );

  check_with_cond1(5, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(SatTestFixture, add_clause_with_cond5n_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  vector<SatLiteral> tmp_lits(5);
  tmp_lits[0] = lit1;
  tmp_lits[1] = lit2;
  tmp_lits[2] = lit3;
  tmp_lits[3] = lit4;
  tmp_lits[4] = lit5;
  mSolver.add_clause(tmp_lits);

  vector<int> vals(
    {
     // lit5 lit4 lit3 lit2 lit1  ans
     //    0    0    0    0    0    0
     //    0    0    0    0    1    1
     //    0    0    0    1    0    1
     //    0    0    0    1    1    1
     //    0    0    1    0    0    1
     //    0    0    1    0    1    1
     //    0    0    1    1    0    1
     //    0    0    1    1    1    1
     //    0    1    0    0    0    1
     //    0    1    0    0    1    1
     //    0    1    0    1    0    1
     //    0    1    0    1    1    1
     //    0    1    1    0    0    1
     //    0    1    1    0    1    1
     //    0    1    1    1    0    1
     //    0    1    1    1    1    1
     //    1    0    0    0    0    1
     //    1    0    0    0    1    1
     //    1    0    0    1    0    1
     //    1    0    0    1    1    1
     //    1    0    1    0    0    1
     //    1    0    1    0    1    1
     //    1    0    1    1    0    1
     //    1    0    1    1    1    1
     //    1    1    0    0    0    1
     //    1    1    0    0    1    1
     //    1    1    0    1    0    1
     //    1    1    0    1    1    1
     //    1    1    1    0    0    1
     //    1    1    1    0    1    1
     //    1    1    1    1    0    1
     //    1    1    1    1    1    1
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
  );

  check_with_cond1(5, vals);

  mSolver.clear_conditional_literals();
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 SatTestFixture,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
