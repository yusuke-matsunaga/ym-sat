
/// @file at_most_test.cc
/// @brief SatSolver::at_most_XXX() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class AtMostTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  AtMostTest() : SatTestFixture() {}

  /// @brief デストラクタ
  ~AtMostTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief at_most_k のチェックを行う．
  void
  check(
    int n,
    int k
  );

};

// @brief at_most_k のチェックを行う．
void
AtMostTest::check(
  int n,
  int k
)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    string bpat{};
    for ( int i: Range(n) ) {
      auto lit{mVarList[i]};
      if ( p & (1U << i) ) {
	++ c;
	bpat += "1";
      }
      else {
	lit = ~lit;
	bpat += "0";
      }
      assumptions.push_back(lit);
    }
    SatBool3 exp_ans = (c <= k) ? SatBool3::True : SatBool3::False;
    SatBool3 stat = mSolver.solve(assumptions);
    if ( exp_ans != stat ) {
      cout << "c = " << c
	   << ", bpat = " << bpat
	   << ", exp_ans = " << exp_ans << endl;
    }
    EXPECT_EQ( exp_ans, stat );
  }
}

TEST_P(AtMostTest, add_at_most_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mSolver.add_at_most_one(lit1, lit2);

  check(2, 1);
}

TEST_P(AtMostTest, add_at_most_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_at_most_one(lit1, lit2, lit3);

  check(3, 1);
}

TEST_P(AtMostTest, add_at_most_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4);

  check(4, 1);
}

TEST_P(AtMostTest, add_at_most_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4, lit5);

  check(5, 1);
}

TEST_P(AtMostTest, add_at_most_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check(6, 1);
}

TEST_P(AtMostTest, add_at_most_oneN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_one(lits);

  check(n, 1);
}

// add_at_most_two2 は自明なのでやらない．

TEST_P(AtMostTest, add_at_most_two3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_at_most_two(lit1, lit2, lit3);

  check(3, 2);
}

TEST_P(AtMostTest, add_at_most_two4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4);

  check(4, 2);
}

TEST_P(AtMostTest, add_at_most_two5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4, lit5);

  check(5, 2);
}

TEST_P(AtMostTest, add_at_most_two6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check(6, 2);
}

TEST_P(AtMostTest, add_at_most_twoN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_two(lits);

  check(n, 2);
}

TEST_P(AtMostTest, add_at_most_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check(n, k);
}

TEST_P(AtMostTest, add_at_most_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check(n, k);
}

TEST_P(AtMostTest, add_at_most_15_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check(n, k);
}

TEST_P(AtMostTest, add_at_most_15_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check(n, k);
}

TEST_P(AtMostTest, add_at_most_15_10)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check(n, k);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 AtMostTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
