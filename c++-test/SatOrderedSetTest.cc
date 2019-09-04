
/// @file SatOrderedSetTest.cc
/// @brief SatOrderedSetTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatSolver.h"
#include "ym/SatModel.h"
#include "ym/SatOrderedSet.h"


BEGIN_NAMESPACE_YM_SAT

class SatOrderedSetTest :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief コンストラクタ
  SatOrderedSetTest();

  /// @brief デストラクタ
  ~SatOrderedSetTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  SatOrderedSet mVar;

};

// @brief コンストラクタ
SatOrderedSetTest::SatOrderedSetTest() :
  mSolver(GetParam())
{
}

// @brief デストラクタ
SatOrderedSetTest::~SatOrderedSetTest()
{
}

TEST_P(SatOrderedSetTest, basic1)
{
  int min = 0;
  int max = 5;
  SatOrderedSet var(mSolver, min, max);

  int n = max - min + 1;
  vector<SatLiteral> lits(n);
  for ( int i = 0; i < n; ++ i ) {
    lits[i] = var.var(i + min);
  }

  int nexp = (1 << n);
  for ( int bits = 0; bits < nexp; ++ bits ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(n);
    int c = 0;
    for ( int i = 0; i < n; ++ i ) {
      auto lit{lits[i]};
      if ( bits & (1 << i) ) {
	assumptions.push_back(lit);
	++ c;
      }
      else {
	assumptions.push_back(~lit);
      }
    }
    SatBool3 exp_ans = (c == 1) ? SatBool3::True : SatBool3::False;
    SatModel dummy;
    SatBool3 ans = mSolver.solve(assumptions, dummy);
    EXPECT_EQ( exp_ans, ans );
  }
}

TEST_P(SatOrderedSetTest, basic2)
{
  int min = 3;
  int max = 3;
  SatOrderedSet var(mSolver, min, max);

  int n = max - min + 1;
  vector<SatLiteral> lits(n);
  for ( int i = 0; i < n; ++ i ) {
    lits[i] = var.var(i + min);
  }

  int nexp = (1 << n);
  for ( int bits = 0; bits < nexp; ++ bits ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(n);
    int c = 0;
    for ( int i = 0; i < n; ++ i ) {
      auto lit{lits[i]};
      if ( bits & (1 << i) ) {
	assumptions.push_back(lit);
	++ c;
      }
      else {
	assumptions.push_back(~lit);
      }
    }
    SatBool3 exp_ans = (c == 1) ? SatBool3::True : SatBool3::False;
    SatModel dummy;
    SatBool3 ans = mSolver.solve(assumptions, dummy);
    EXPECT_EQ( exp_ans, ans );
  }
}

TEST_P(SatOrderedSetTest, ge1)
{
  int min = 3;
  int max = 10;
  SatOrderedSet var(mSolver, min, max);

  int lval = 7;
  var.add_ge_constraint(mSolver, lval);

  int n = max - min + 1;
  vector<SatLiteral> lits(n);
  for ( int i = 0; i < n; ++ i ) {
    lits[i] = var.var(i + min);
  }

  int nexp = (1 << n);
  for ( int bits = 0; bits < nexp; ++ bits ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(n);
    int c = 0;
    int val = -1;
    for ( int i = 0; i < n; ++ i ) {
      auto lit{lits[i]};
      if ( bits & (1 << i) ) {
	assumptions.push_back(lit);
	++ c;
	val = min + i;
      }
      else {
	assumptions.push_back(~lit);
      }
    }
    SatBool3 exp_ans = (c == 1) ? SatBool3::True : SatBool3::False;
    if ( val < lval ) {
      exp_ans = SatBool3::False;
    }
    SatModel dummy;
    SatBool3 ans = mSolver.solve(assumptions, dummy);
    EXPECT_EQ( exp_ans, ans );
  }
}

TEST_P(SatOrderedSetTest, le1)
{
  int min = 3;
  int max = 10;
  SatOrderedSet var(mSolver, min, max);

  int uval = 5;
  var.add_le_constraint(mSolver, uval);

  int n = max - min + 1;
  vector<SatLiteral> lits(n);
  for ( int i = 0; i < n; ++ i ) {
    lits[i] = var.var(i + min);
  }

  int nexp = (1 << n);
  for ( int bits = 0; bits < nexp; ++ bits ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(n);
    int c = 0;
    int val = -1;
    for ( int i = 0; i < n; ++ i ) {
      auto lit{lits[i]};
      if ( bits & (1 << i) ) {
	assumptions.push_back(lit);
	++ c;
	val = min + i;
      }
      else {
	assumptions.push_back(~lit);
      }
    }
    SatBool3 exp_ans = (c == 1) ? SatBool3::True : SatBool3::False;
    if ( val > uval ) {
      exp_ans = SatBool3::False;
    }
    SatModel dummy;
    SatBool3 ans = mSolver.solve(assumptions, dummy);
    EXPECT_EQ( exp_ans, ans );
  }
}

TEST_P(SatOrderedSetTest, droppoff1)
{
  int min = 3;
  int max = 10;
  SatOrderedSet var(mSolver, min, max);

  int uval = 5;
  int lval = 7;
  var.add_dropoff_constraint(mSolver, uval, lval);

  int n = max - min + 1;
  vector<SatLiteral> lits(n);
  for ( int i = 0; i < n; ++ i ) {
    lits[i] = var.var(i + min);
  }

  int nexp = (1 << n);
  for ( int bits = 0; bits < nexp; ++ bits ) {
    vector<SatLiteral> assumptions;
    assumptions.reserve(n);
    int c = 0;
    int val = -1;
    for ( int i = 0; i < n; ++ i ) {
      auto lit{lits[i]};
      if ( bits & (1 << i) ) {
	assumptions.push_back(lit);
	++ c;
	val = min + i;
      }
      else {
	assumptions.push_back(~lit);
      }
    }
    SatBool3 exp_ans = (c == 1) ? SatBool3::True : SatBool3::False;
    if ( uval < val && val < lval ) {
      exp_ans = SatBool3::False;
    }
    SatModel dummy;
    SatBool3 ans = mSolver.solve(assumptions, dummy);
    EXPECT_EQ( exp_ans, ans );
  }
}

INSTANTIATE_TEST_CASE_P(SatSolverTest,
			SatOrderedSetTest,
			::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					  "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM_SAT
