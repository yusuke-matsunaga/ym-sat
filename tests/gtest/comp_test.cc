
/// @file comp_test.cc
/// @brief SatSolver::add_eq() 等の比較系のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include <random>


BEGIN_NAMESPACE_YM

class CompTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  CompTest() : SatTestFixture() {}

  /// @brief デストラクタ
  ~CompTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief eq のチェックを行う．
  void
  check_eq(
    int na,
    int nb
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    vector<SatLiteral> lit_b(nb);
    for ( int i = 0; i < nb; ++ i ) {
      lit_b[i] = mVarList[i + na];
    }
    mSolver.add_eq(lit_a, lit_b);
    check_common(na, nb, [=](int a, int b) { return a == b; });
  }

  /// @brief ne のチェックを行う．
  void
  check_ne(
    int na,
    int nb
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    vector<SatLiteral> lit_b(nb);
    for ( int i = 0; i < nb; ++ i ) {
      lit_b[i] = mVarList[i + na];
    }
    mSolver.add_ne(lit_a, lit_b);
    check_common(na, nb, [=](int a, int b) { return a != b; });
  }

  /// @brief lt のチェックを行う．
  void
  check_lt(
    int na,
    int nb
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    vector<SatLiteral> lit_b(nb);
    for ( int i = 0; i < nb; ++ i ) {
      lit_b[i] = mVarList[i + na];
    }
    mSolver.add_lt(lit_a, lit_b);
    check_common(na, nb, [=](int a, int b) { return a < b; });
  }

  /// @brief le のチェックを行う．
  void
  check_le(
    int na,
    int nb
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    vector<SatLiteral> lit_b(nb);
    for ( int i = 0; i < nb; ++ i ) {
      lit_b[i] = mVarList[i + na];
    }
    mSolver.add_le(lit_a, lit_b);
    check_common(na, nb, [=](int a, int b) { return a <= b; });
  }

  /// @brief gt のチェックを行う．
  void
  check_gt(
    int na,
    int nb
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    vector<SatLiteral> lit_b(nb);
    for ( int i = 0; i < nb; ++ i ) {
      lit_b[i] = mVarList[i + na];
    }
    mSolver.add_gt(lit_a, lit_b);
    check_common(na, nb, [=](int a, int b) { return a > b; });
  }

  /// @brief ge のチェックを行う．
  void
  check_ge(
    int na,
    int nb
  )
  {
    vector<SatLiteral> lit_a(na);
    for ( int i = 0; i < na; ++ i ) {
      lit_a[i] = mVarList[i];
    }
    vector<SatLiteral> lit_b(nb);
    for ( int i = 0; i < nb; ++ i ) {
      lit_b[i] = mVarList[i + na];
    }
    mSolver.add_ge(lit_a, lit_b);
    check_common(na, nb, [=](int a, int b) { return a >= b; });
  }

  /// @brief eq のチェックを行う．
  void
  check_eq_const(
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
  check_ne_const(
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
  check_lt_const(
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
  check_le_const(
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
  check_gt_const(
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
  check_ge_const(
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
    int nb,
    std::function<bool(int, int)> comp
  );

  /// @brief comp のチェックを行う．
  void
  check_common(
    int na,
    std::function<bool(int)> comp
  );

};

// @brief comp のチェックを行う．
void
CompTest::check_common(
  int na,
  int nb,
  std::function<bool(int, int)> comp
)
{
  int nall = na + nb;
  ASSERT_COND( nall < mVarNum );

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
      if ( comp(a, b) ) {
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
      int a = ud_a(randgen);
      int b = ud_b(randgen);
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
      auto exp_ans = comp(a, b) ? SatBool3::True : SatBool3::False;
      EXPECT_EQ( exp_ans, ans );
    }
  }
}

// @brief comp のチェックを行う．
void
CompTest::check_common(
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

TEST_P(CompTest, add_eq_1_1)
{
  check_eq(1, 1);
}

TEST_P(CompTest, add_eq_2_2)
{
  check_eq(2, 2);
}

TEST_P(CompTest, add_eq_5_5)
{
  check_eq(5, 5);
}

TEST_P(CompTest, add_eq_7_7)
{
  check_eq(7, 7);
}

TEST_P(CompTest, add_eq_7_4)
{
  check_eq(7, 4);
}

TEST_P(CompTest, add_eq_10_10)
{
  check_eq(10, 10);
}

TEST_P(CompTest, add_eq_10_5)
{
  check_eq(10, 5);
}

TEST_P(CompTest, add_ne_1_1)
{
  check_ne(1, 1);
}

TEST_P(CompTest, add_ne_2_2)
{
  check_ne(2, 2);
}

TEST_P(CompTest, add_ne_5_5)
{
  check_ne(5, 5);
}

TEST_P(CompTest, add_ne_7_7)
{
  check_ne(7, 7);
}

TEST_P(CompTest, add_ne_7_4)
{
  check_ne(7, 4);
}

TEST_P(CompTest, add_ne_10_10)
{
  check_ne(10, 10);
}

TEST_P(CompTest, add_ne_10_5)
{
  check_ne(10, 5);
}

TEST_P(CompTest, add_lt_1_1)
{
  check_lt(1, 1);
}

TEST_P(CompTest, add_lt_2_1)
{
  check_lt(2, 1);
}

TEST_P(CompTest, add_lt_1_2)
{
  check_lt(1, 2);
}

TEST_P(CompTest, add_lt_2_2)
{
  check_lt(2, 2);
}

TEST_P(CompTest, add_lt_5_5)
{
  check_lt(5, 5);
}

TEST_P(CompTest, add_lt_7_7)
{
  check_lt(7, 7);
}

TEST_P(CompTest, add_lt_7_4)
{
  check_lt(7, 4);
}

TEST_P(CompTest, add_lt_4_7)
{
  check_lt(7, 4);
}

TEST_P(CompTest, add_lt_10_10)
{
  check_lt(10, 10);
}

TEST_P(CompTest, add_lt_10_5)
{
  check_lt(10, 5);
}

TEST_P(CompTest, add_lt_5_10)
{
  check_lt(10, 5);
}

TEST_P(CompTest, add_le_1_1)
{
  check_le(1, 1);
}

TEST_P(CompTest, add_le_2_1)
{
  check_le(2, 1);
}

TEST_P(CompTest, add_le_1_2)
{
  check_le(1, 2);
}

TEST_P(CompTest, add_le_2_2)
{
  check_le(2, 2);
}

TEST_P(CompTest, add_le_5_5)
{
  check_le(5, 5);
}

TEST_P(CompTest, add_le_7_7)
{
  check_le(7, 7);
}

TEST_P(CompTest, add_le_7_4)
{
  check_le(7, 4);
}

TEST_P(CompTest, add_le_4_7)
{
  check_le(7, 4);
}

TEST_P(CompTest, add_le_10_10)
{
  check_le(10, 10);
}

TEST_P(CompTest, add_le_10_5)
{
  check_le(10, 5);
}

TEST_P(CompTest, add_le_5_10)
{
  check_le(10, 5);
}

TEST_P(CompTest, add_gt_1_1)
{
  check_gt(1, 1);
}

TEST_P(CompTest, add_gt_2_1)
{
  check_gt(2, 1);
}

TEST_P(CompTest, add_gt_1_2)
{
  check_gt(1, 2);
}

TEST_P(CompTest, add_gt_2_2)
{
  check_gt(2, 2);
}

TEST_P(CompTest, add_gt_5_5)
{
  check_gt(5, 5);
}

TEST_P(CompTest, add_gt_7_7)
{
  check_gt(7, 7);
}

TEST_P(CompTest, add_gt_7_4)
{
  check_gt(7, 4);
}

TEST_P(CompTest, add_gt_4_7)
{
  check_gt(7, 4);
}

TEST_P(CompTest, add_gt_10_10)
{
  check_gt(10, 10);
}

TEST_P(CompTest, add_gt_10_5)
{
  check_gt(10, 5);
}

TEST_P(CompTest, add_gt_5_10)
{
  check_gt(10, 5);
}

TEST_P(CompTest, add_ge_1_1)
{
  check_ge(1, 1);
}

TEST_P(CompTest, add_ge_2_1)
{
  check_ge(2, 1);
}

TEST_P(CompTest, add_ge_1_2)
{
  check_ge(1, 2);
}

TEST_P(CompTest, add_ge_2_2)
{
  check_ge(2, 2);
}

TEST_P(CompTest, add_ge_5_5)
{
  check_ge(5, 5);
}

TEST_P(CompTest, add_ge_7_7)
{
  check_ge(7, 7);
}

TEST_P(CompTest, add_ge_7_4)
{
  check_ge(7, 4);
}

TEST_P(CompTest, add_ge_4_7)
{
  check_ge(7, 4);
}

TEST_P(CompTest, add_ge_10_10)
{
  check_ge(10, 10);
}

TEST_P(CompTest, add_ge_10_5)
{
  check_ge(10, 5);
}

TEST_P(CompTest, add_ge_5_10)
{
  check_ge(10, 5);
}

TEST_P(CompTest, add_eq_const_1_1)
{
  check_eq_const(1, 1);
}

TEST_P(CompTest, add_eq_const_2_3)
{
  check_eq_const(2, 3);
}

TEST_P(CompTest, add_eq_const_5_6)
{
  check_eq_const(5, 6);
}

TEST_P(CompTest, add_eq_const_7_10)
{
  check_eq_const(7, 10);
}

TEST_P(CompTest, add_eq_const_7_13)
{
  check_eq_const(7, 13);
}

TEST_P(CompTest, add_eq_const_10_496)
{
  check_eq_const(10, 496);
}

TEST_P(CompTest, add_eq_const_15_0)
{
  check_eq_const(15, 0);
}

TEST_P(CompTest, add_ne_const_1_1)
{
  check_ne_const(1, 1);
}

TEST_P(CompTest, add_ne_const_2_3)
{
  check_ne_const(2, 3);
}

TEST_P(CompTest, add_ne_const_5_6)
{
  check_ne_const(5, 6);
}

TEST_P(CompTest, add_ne_const_7_10)
{
  check_ne_const(7, 10);
}

TEST_P(CompTest, add_ne_const_7_13)
{
  check_ne_const(7, 13);
}

TEST_P(CompTest, add_ne_const_10_496)
{
  check_ne_const(10, 496);
}

TEST_P(CompTest, add_ne_const_15_0)
{
  check_ne_const(15, 0);
}

TEST_P(CompTest, add_lt_const_1_1)
{
  check_lt_const(1, 1);
}

TEST_P(CompTest, add_lt_const_2_3)
{
  check_lt_const(2, 3);
}

TEST_P(CompTest, add_lt_const_5_6)
{
  check_lt_const(5, 6);
}

TEST_P(CompTest, add_lt_const_7_10)
{
  check_lt_const(7, 10);
}

TEST_P(CompTest, add_lt_const_7_13)
{
  check_lt_const(7, 13);
}

TEST_P(CompTest, add_lt_const_10_496)
{
  check_lt_const(10, 496);
}

TEST_P(CompTest, add_lt_const_15_0)
{
  check_lt_const(15, 0);
}

TEST_P(CompTest, add_le_const_1_1)
{
  check_le_const(1, 1);
}

TEST_P(CompTest, add_le_const_2_3)
{
  check_le_const(2, 3);
}

TEST_P(CompTest, add_le_const_5_6)
{
  check_le_const(5, 6);
}

TEST_P(CompTest, add_le_const_7_10)
{
  check_le_const(7, 10);
}

TEST_P(CompTest, add_le_const_7_13)
{
  check_le_const(7, 13);
}

TEST_P(CompTest, add_le_const_10_496)
{
  check_le_const(10, 496);
}

TEST_P(CompTest, add_le_const_15_0)
{
  check_le_const(15, 0);
}

TEST_P(CompTest, add_gt_const_1_1)
{
  check_gt_const(1, 1);
}

TEST_P(CompTest, add_gt_const_2_3)
{
  check_gt_const(2, 3);
}

TEST_P(CompTest, add_gt_const_5_6)
{
  check_gt_const(5, 6);
}

TEST_P(CompTest, add_gt_const_7_10)
{
  check_gt_const(7, 10);
}

TEST_P(CompTest, add_gt_const_7_13)
{
  check_gt_const(7, 13);
}

TEST_P(CompTest, add_gt_const_10_496)
{
  check_gt_const(10, 496);
}

TEST_P(CompTest, add_gt_const_15_0)
{
  check_gt_const(15, 0);
}

TEST_P(CompTest, add_ge_const_1_1)
{
  check_ge_const(1, 1);
}

TEST_P(CompTest, add_ge_const_2_3)
{
  check_ge_const(2, 3);
}

TEST_P(CompTest, add_ge_const_5_6)
{
  check_ge_const(5, 6);
}

TEST_P(CompTest, add_ge_const_7_10)
{
  check_ge_const(7, 10);
}

TEST_P(CompTest, add_ge_const_7_13)
{
  check_ge_const(7, 13);
}

TEST_P(CompTest, add_ge_const_10_496)
{
  check_ge_const(10, 496);
}

TEST_P(CompTest, add_ge_const_15_0)
{
  check_ge_const(15, 0);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 CompTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
