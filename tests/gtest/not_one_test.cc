
/// @file not_one_test.cc
/// @brief SatSolver::not_one() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class NotOneTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  NotOneTest() : SatTestFixture() {}

  /// @brief デストラクタ
  ~NotOneTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief not_one のチェックを行う．
  void
  check(
    int n
  );

};

// @brief not_one のチェックを行う．
void
NotOneTest::check(
  int n
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
    auto exp_ans = (c != 1) ? SatBool3::True : SatBool3::False;
    auto stat = mSolver.solve(assumptions);
    EXPECT_EQ( exp_ans, stat );
  }
}

TEST_P(NotOneTest, add_not_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mSolver.add_not_one(lit1, lit2);

  check(2);
}

TEST_P(NotOneTest, add_not_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_not_one(lit1, lit2, lit3);

  check(3);
}

TEST_P(NotOneTest, add_not_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_not_one(lit1, lit2, lit3, lit4);

  check(4);
}

TEST_P(NotOneTest, add_not_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_not_one(lit1, lit2, lit3, lit4, lit5);

  check(5);
}

TEST_P(NotOneTest, add_not_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_not_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check(6);
}

TEST_P(NotOneTest, add_not_oneN)
{
  int n = 10;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_not_one(lits);

  check(n);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 NotOneTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
