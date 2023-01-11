
/// @file lt_test.cc
/// @brief SatSolver::add_lt() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include <random>


BEGIN_NAMESPACE_YM

class LtTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  LtTest() : SatTestFixture() {}

  /// @brief デストラクタ
  ~LtTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief lt のチェックを行う．
  void
  check(
    int na,
    int nb
  );

};

// @brief lt のチェックを行う．
void
LtTest::check(
  int na,
  int nb
)
{
  int nall = na + nb;
  ASSERT_COND( nall < mVarNum );
  vector<SatLiteral> lit_a(na);
  for ( int i = 0; i < na; ++ i ) {
    lit_a[i] = mVarList[i];
  }
  vector<SatLiteral> lit_b(nb);
  for ( int i = 0; i < nb; ++ i ) {
    lit_b[i] = mVarList[i + na];
  }
  mSolver.add_lt(lit_a, lit_b);

  if ( nall <= 15 ) {
    int nexp = 1 << nall;
    int amask = (1 << na) - 1;
    int ashift = 0;
    int bmask = (1 << nb) - 1;
    int bshift = na;
    vector<int> vals(nexp);
    for ( int p = 0; p < nexp; ++ p ) {
      int a = (p >> ashift) & amask;
      int b = (p >> bshift) & bmask;
      if ( a < b ) {
	vals[p] = 1;
      }
      else {
	vals[p] = 0;
      }
    }
    SatTestFixture::check(nall, vals);
  }
  else {
    std::mt19937 randgen;
    SizeType num = 20000;
    std::uniform_int_distribution<int> ud_a(0, (1 << na) - 1);
    std::uniform_int_distribution<int> ud_b(0, (1 << nb) - 1);
    for ( int j = 0; j < num; ++ j ) {
      unsigned int a = ud_a(randgen);
      unsigned int b = ud_b(randgen);
      vector<SatLiteral> assumptions;
      for ( int i = 0; i < na; ++ i ) {
	auto lit = mVarList[i];
	if ( (a & (1 << i)) == 0 ) {
	  lit = ~lit;
	}
	assumptions.push_back(lit);
      }
      for ( int i = 0; i < nb; ++ i ) {
	auto lit = mVarList[i + na];
	if ( (b & (1 << i)) == 0 ) {
	  lit = ~lit;
	}
	assumptions.push_back(lit);
      }
      auto ans = mSolver.solve(assumptions);
      auto exp_ans = (a < b) ? SatBool3::True : SatBool3::False;
      EXPECT_EQ( exp_ans, ans ) << "a = " << a << ", b = " << b;
    }
  }
}

TEST_P(LtTest, add_lt_1_1)
{
  check(1, 1);
}

TEST_P(LtTest, add_lt_2_1)
{
  check(2, 1);
}

TEST_P(LtTest, add_lt_1_2)
{
  check(1, 2);
}

TEST_P(LtTest, add_lt_2_2)
{
  check(2, 2);
}

TEST_P(LtTest, add_lt_5_5)
{
  check(5, 5);
}

TEST_P(LtTest, add_lt_7_7)
{
  check(7, 7);
}

TEST_P(LtTest, add_lt_7_4)
{
  check(7, 4);
}

TEST_P(LtTest, add_lt_4_7)
{
  check(7, 4);
}

TEST_P(LtTest, add_lt_10_10)
{
  check(10, 10);
}

TEST_P(LtTest, add_lt_10_5)
{
  check(10, 5);
}

TEST_P(LtTest, add_lt_5_10)
{
  check(10, 5);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 LtTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
