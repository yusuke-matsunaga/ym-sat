
/// @file ge_const_test.cc
/// @brief SatSolver::add_ge() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include <random>


BEGIN_NAMESPACE_YM

class GeConstTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  GeConstTest() : SatTestFixture() {}

  /// @brief デストラクタ
  ~GeConstTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief gt のチェックを行う．
  void
  check(
    int na,
    int bval
  );

};

// @brief gt のチェックを行う．
void
GeConstTest::check(
  int na,
  int bval
)
{
  ASSERT_COND( na < mVarNum );
  vector<SatLiteral> lit_a(na);
  for ( int i = 0; i < na; ++ i ) {
    lit_a[i] = mVarList[i];
  }
  mSolver.add_ge(lit_a, bval);

  int nexp = 1 << na;
  if ( na <= 15 ) {
    vector<int> vals(nexp);
    for ( int a = 0; a < nexp; ++ a ) {
      if ( a >= bval ) {
	vals[a] = 1;
      }
      else {
	vals[a] = 0;
      }
    }
    SatTestFixture::check(na, vals);
  }
  else {
    std::mt19937 randgen;
    SizeType num = 20000;
    std::uniform_int_distribution<int> ud_a(0, nexp - 1);
    for ( int j = 0; j < num; ++ j ) {
      int a = ud_a(randgen);
      vector<SatLiteral> assumptions;
      for ( int i = 0; i < na; ++ i ) {
	auto lit = mVarList[i];
	if ( (a & (1 << i)) == 0 ) {
	  lit = ~lit;
	}
	assumptions.push_back(lit);
      }
      auto ans = mSolver.solve(assumptions);
      auto exp_ans = (a >= bval) ? SatBool3::True : SatBool3::False;
      EXPECT_EQ( exp_ans, ans );
    }
  }
}

TEST_P(GeConstTest, add_ge_1_1)
{
  check(1, 1);
}

TEST_P(GeConstTest, add_ge_2_3)
{
  check(2, 3);
}

TEST_P(GeConstTest, add_ge_5_6)
{
  check(5, 6);
}

TEST_P(GeConstTest, add_ge_7_10)
{
  check(7, 10);
}

TEST_P(GeConstTest, add_ge_7_13)
{
  check(7, 13);
}

TEST_P(GeConstTest, add_ge_10_496)
{
  check(10, 496);
}

TEST_P(GeConstTest, add_ge_15_0)
{
  check(10, 0);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 GeConstTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
