
/// @file exact_test.cc
/// @brief SatSolver::exact_XXX のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class ExactTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  ExactTest() : SatTestFixture() {}

  /// @brief デストラクタ
  ~ExactTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief exact_k のチェックを行う．
  void
  check(
    int n,
    int k
  );

};

// @brief exact_k のチェックを行う．
void
ExactTest::check(
  int n,
  int k
)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    for ( int i: Range(n) ) {
      auto lit{mVarList[i]};
      if ( p & (1U << i) ) {
	++ c;
      }
      else {
	lit = ~lit;
      }
      assumptions.push_back(lit);
    }
    SatBool3 exp_ans = (c == k) ? SatBool3::True : SatBool3::False;
    SatBool3 stat = mSolver.solve(assumptions);
    EXPECT_EQ( exp_ans, stat );
  }
}

TEST_P(ExactTest, add_exact_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mSolver.add_exact_one(lit1, lit2);

  check(2, 1);
}

TEST_P(ExactTest, add_exact_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_exact_one(lit1, lit2, lit3);

  check(3, 1);
}

TEST_P(ExactTest, add_exact_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_exact_one(lit1, lit2, lit3, lit4);

  check(4, 1);
}

TEST_P(ExactTest, add_exact_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_exact_one(lit1, lit2, lit3, lit4, lit5);

  check(5, 1);
}

TEST_P(ExactTest, add_exact_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_exact_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check(6, 1);
}

TEST_P(ExactTest, add_exact_oneN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_one(lits);

  check(7, 1);
}

// add_exact_two2 は自明なのでやらない．

TEST_P(ExactTest, add_exact_two3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_exact_two(lit1, lit2, lit3);

  check(3, 2);
}

TEST_P(ExactTest, add_exact_two4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_exact_two(lit1, lit2, lit3, lit4);

  check(4, 2);
}

TEST_P(ExactTest, add_exact_two5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_exact_two(lit1, lit2, lit3, lit4, lit5);

  check(5, 2);
}

TEST_P(ExactTest, add_exact_two6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_exact_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check(6, 2);
}

TEST_P(ExactTest, add_exact_twoN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_two(lits);

  check(7, 2);
}

TEST_P(ExactTest, add_exact_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check(n, k);
}

TEST_P(ExactTest, add_exact_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check(n, k);
}

TEST_P(ExactTest, add_exact_15_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check(n, k);
}

TEST_P(ExactTest, add_exact_15_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check(n, k);
}

TEST_P(ExactTest, add_exact_15_10)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check(n, k);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 ExactTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
