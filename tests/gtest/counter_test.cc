
/// @file counter_test.cc
/// @brief SatSolver::add_counter() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include "ym/Range.h"
#include <random>


BEGIN_NAMESPACE_YM

class CounterTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  CounterTest() : SatTestFixture() { }

  /// @brief デストラクタ
  ~CounterTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief counter のチェックを行う．
  void
  check(
    int ni, ///< [in] 入力のビット幅
    int no  ///< [in] 出力のビット幅
  );

};


// @brief counter のチェックを行う．
void
CounterTest::check(
  int ni,
  int no
)
{
  int no_exp = (1 << no);

  ASSERT_COND( ni < no_exp );
  ASSERT_COND( ni + no < mVarNum );

  vector<SatLiteral> ilits(ni);
  for ( int i = 0; i < ni; ++ i ) {
    ilits[i] = mVarList[i];
  }

  auto tmp_olits = mSolver.add_counter(ilits);

  for ( int i = 0; i < no; ++ i ) {
    auto olit = mVarList[i + ni];
    mSolver.add_clause( olit, ~tmp_olits[i]);
    mSolver.add_clause(~olit,  tmp_olits[i]);
  }

  if ( ni + no <= 15 ) {
    int nexp = (1 << (ni + no));
    vector<int> vals(nexp);
    for ( int bits = 0; bits < nexp; ++ bits ) {
      int c_exp = 0;
      for ( int i = 0; i < ni; ++ i ) {
	if ( bits & (1 << i) ) {
	  ++ c_exp;
	}
      }
      int c = 0;
      for ( int i = 0; i < no; ++ i ) {
	if ( bits & (1 << (i + ni)) ) {
	  c |= (1 << i);
	}
      }
      if ( c == c_exp ) {
	vals[bits] = 1;
      }
      else {
	vals[bits] = 0;
      }
    }

    SatTestFixture::check((ni + no), vals);
  }
  else {
    std::mt19937 randgen;
    SizeType num = 20000;
    std::uniform_int_distribution<int> ud(0, 1);
    for ( int j = 0; j < num; ++ j ) {
      int c_exp = 0;
      vector<SatLiteral> assumptions;
      vector<int> ivals(ni, 0);
      for ( int i = 0; i < ni; ++ i ) {
	auto lit = mVarList[i];
	if ( ud(randgen) ) {
	  ++ c_exp;
	}
	else {
	  lit = ~lit;
	}
	assumptions.push_back(lit);
      }
      for ( int i = 0; i < no; ++ i ) {
	auto lit = mVarList[i + ni];
	if ( (c_exp & (1 << i)) == 0 ) {
	  lit = ~lit;
	}
	assumptions.push_back(lit);
      }
      SatBool3 stat = mSolver.solve(assumptions);
      EXPECT_EQ( SatBool3::True, stat );
      bool ng = false;
      for ( SizeType i = 0; i < no; ++ i ) {
	auto olit = mVarList[i + ni];
	auto v = mSolver.read_model(olit);
	SatBool3 exp_val;
	if ( c_exp & (1 << i) ) {
	  exp_val = SatBool3::True;
	}
	else {
	  exp_val = SatBool3::False;
	}
	if ( exp_val != v ) {
	  ng = true;
	}
      }
      if ( ng ) {
	int c = 0;
	vector<int> ovals(no, 0);
	for ( SizeType i = 0; i < no; ++ i ) {
	  auto olit = mVarList[i + ni];
	  auto v = mSolver.read_model(olit);
	  if ( v == SatBool3::True ) {
	    c |= (1 << i);
	    ovals[i] = 1;
	  }
	}
	cout << "ivals: ";
	for ( SizeType i = 0; i < ni; ++ i ) {
	  cout << " " << ivals[i];
	}
	cout << endl;
	cout << "ovals: ";
	for ( SizeType i = 0; i < no; ++ i ) {
	  cout << " " << ovals[i];
	}
	cout << endl;
	cout << "c_exp = " << c_exp
	     << ", c = " << c << endl;
      }
      EXPECT_FALSE( ng );
    }
  }
}

TEST_P(CounterTest, add_counter_2_2)
{
  check(2, 2);
}

TEST_P(CounterTest, add_counter_3_2)
{
  check(3, 2);
}

TEST_P(CounterTest, add_counter_4_3)
{
  check(4, 3);
}

TEST_P(CounterTest, add_counter_5_3)
{
  check(5, 3);
}

TEST_P(CounterTest, add_counter_6_3)
{
  check(6, 3);
}

TEST_P(CounterTest, add_counter_7_3)
{
  check(7, 3);
}

TEST_P(CounterTest, add_counter_8_4)
{
  check(8, 4);
}

TEST_P(CounterTest, add_counter_9_4)
{
  check(9, 4);
}

TEST_P(CounterTest, add_counter_10_4)
{
  check(10, 4);
}

TEST_P(CounterTest, add_counter_11_4)
{
  check(11, 4);
}

TEST_P(CounterTest, add_counter_12_4)
{
  check(12, 4);
}

TEST_P(CounterTest, add_counter_13_4)
{
  check(13, 4);
}

TEST_P(CounterTest, add_counter_14_4)
{
  check(14, 4);
}

TEST_P(CounterTest, add_counter_15_4)
{
  check(15, 4);
}

TEST_P(CounterTest, add_counter_20_5)
{
  check(20, 5);
}

TEST_P(CounterTest, add_counter_50_6)
{
  check(50, 6);
}

TEST_P(CounterTest, add_counter_100_7)
{
  check(100, 7);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 CounterTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
