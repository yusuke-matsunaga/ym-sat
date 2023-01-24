
/// @file comp_const_test.cc
/// @brief SatSolver::add_eq() 等の比較系のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include <random>


BEGIN_NAMESPACE_YM

class CompConstTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  CompConstTest() : SatTestFixture() {}

  /// @brief デストラクタ
  ~CompConstTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief eq のチェックを行う．
  void
  check_eq(
    int na,
    int b
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    mSolver.add_eq(lit_a, b);
    check_common(na, [=](int a) { return a == b; });
  }

  /// @brief ne のチェックを行う．
  void
  check_ne(
    int na,
    int b
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    mSolver.add_ne(lit_a, b);
    check_common(na, [=](int a) { return a != b; });
  }

  /// @brief lt のチェックを行う．
  void
  check_lt(
    int na,
    int b
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    mSolver.add_lt(lit_a, b);
    check_common(na, [=](int a) { return a < b; });
  }

  /// @brief le のチェックを行う．
  void
  check_le(
    int na,
    int b
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    mSolver.add_le(lit_a, b);
    check_common(na, [=](int a) { return a <= b; });
  }

  /// @brief gt のチェックを行う．
  void
  check_gt(
    int na,
    int b
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    mSolver.add_gt(lit_a, b);
    check_common(na, [=](int a) { return a > b; });
  }

  /// @brief geq のチェックを行う．
  void
  check_ge(
    int na,
    int b
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    mSolver.add_ge(lit_a, b);
    check_common(na, [=](int a) { return a >= b; });
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 下請け関数
  //////////////////////////////////////////////////////////////////////

  /// @brief comp のチェックを行う．
  void
  check_common(
    int na,
    std::function<bool(int)> comp
  );

};

// @brief comp のチェックを行う．
void
CompConstTest::check_common(
  int na,
  std::function<bool(int)> comp
)
{
  ASSERT_COND( na < mVarNum );
  int nexp = 1 << na;
  if ( na <= 15 ) {
    vector<int> vals(nexp);
    for ( int a = 0; a < nexp; ++ a ) {
      if ( comp(a) ) {
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
      auto exp_ans = comp(a) ? SatBool3::True : SatBool3::False;
      EXPECT_EQ( exp_ans, ans );
    }
  }
}

TEST_P(CompConstTest, add_eq_1_1)
{
  check_eq(1, 1);
}

TEST_P(CompConstTest, add_eq_2_3)
{
  check_eq(2, 3);
}

TEST_P(CompConstTest, add_eq_5_6)
{
  check_eq(5, 6);
}

TEST_P(CompConstTest, add_eq_7_10)
{
  check_eq(7, 10);
}

TEST_P(CompConstTest, add_eq_7_13)
{
  check_eq(7, 13);
}

TEST_P(CompConstTest, add_eq_10_496)
{
  check_eq(10, 496);
}

TEST_P(CompConstTest, add_eq_15_0)
{
  check_eq(15, 0);
}

TEST_P(CompConstTest, add_ne_1_1)
{
  check_ne(1, 1);
}

TEST_P(CompConstTest, add_ne_2_3)
{
  check_ne(2, 3);
}

TEST_P(CompConstTest, add_ne_5_6)
{
  check_ne(5, 6);
}

TEST_P(CompConstTest, add_ne_7_10)
{
  check_ne(7, 10);
}

TEST_P(CompConstTest, add_ne_7_13)
{
  check_ne(7, 13);
}

TEST_P(CompConstTest, add_ne_10_496)
{
  check_ne(10, 496);
}

TEST_P(CompConstTest, add_ne_15_0)
{
  check_ne(15, 0);
}

TEST_P(CompConstTest, add_lt_1_1)
{
  check_lt(1, 1);
}

TEST_P(CompConstTest, add_lt_2_3)
{
  check_lt(2, 3);
}

TEST_P(CompConstTest, add_lt_5_6)
{
  check_lt(5, 6);
}

TEST_P(CompConstTest, add_lt_7_10)
{
  check_lt(7, 10);
}

TEST_P(CompConstTest, add_lt_7_13)
{
  check_lt(7, 13);
}

TEST_P(CompConstTest, add_lt_10_496)
{
  check_lt(10, 496);
}

TEST_P(CompConstTest, add_lt_15_0)
{
  check_lt(15, 0);
}

TEST_P(CompConstTest, add_le_1_1)
{
  check_le(1, 1);
}

TEST_P(CompConstTest, add_le_2_3)
{
  check_le(2, 3);
}

TEST_P(CompConstTest, add_le_5_6)
{
  check_le(5, 6);
}

TEST_P(CompConstTest, add_le_7_10)
{
  check_le(7, 10);
}

TEST_P(CompConstTest, add_le_7_13)
{
  check_le(7, 13);
}

TEST_P(CompConstTest, add_le_10_496)
{
  check_le(10, 496);
}

TEST_P(CompConstTest, add_le_15_0)
{
  check_le(15, 0);
}

TEST_P(CompConstTest, add_gt_1_1)
{
  check_gt(1, 1);
}

TEST_P(CompConstTest, add_gt_2_3)
{
  check_gt(2, 3);
}

TEST_P(CompConstTest, add_gt_5_6)
{
  check_gt(5, 6);
}

TEST_P(CompConstTest, add_gt_7_10)
{
  check_gt(7, 10);
}

TEST_P(CompConstTest, add_gt_7_13)
{
  check_gt(7, 13);
}

TEST_P(CompConstTest, add_gt_10_496)
{
  check_gt(10, 496);
}

TEST_P(CompConstTest, add_gt_15_0)
{
  check_gt(15, 0);
}

TEST_P(CompConstTest, add_ge_1_1)
{
  check_ge(1, 1);
}

TEST_P(CompConstTest, add_ge_2_3)
{
  check_ge(2, 3);
}

TEST_P(CompConstTest, add_ge_5_6)
{
  check_ge(5, 6);
}

TEST_P(CompConstTest, add_ge_7_10)
{
  check_ge(7, 10);
}

TEST_P(CompConstTest, add_ge_7_13)
{
  check_ge(7, 13);
}

TEST_P(CompConstTest, add_ge_10_496)
{
  check_ge(10, 496);
}

TEST_P(CompConstTest, add_ge_15_0)
{
  check_ge(15, 0);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 CompConstTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat2old2", "ymsat1_old"));

END_NAMESPACE_YM
