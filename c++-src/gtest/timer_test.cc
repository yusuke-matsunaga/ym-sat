
/// @file timer_test.cc
/// @brief timer_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include <gtest/gtest.h>
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM

class TimerTest :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief コンストラクタ
  TimerTest(
  ) : mSolver{GetParam()}
  {
  }

  /// @brief テスト本体
  SatBool3
  check();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SAT ソルバ
  SatSolver mSolver;

};

SatBool3
TimerTest::check()
{
  SizeType nh = 20;
  SizeType np = nh + 1;

  vector<SatLiteral> var_array(nh * np);
  for ( SizeType i = 0; i < np; ++ i ) {
    for ( SizeType j = 0; j < nh; ++ j ) {
      auto var = mSolver.new_variable(true);
      var_array[i * nh + j] = var;
    }
  }

  // 各々の鳩はいずれかの穴に入る．
  for ( SizeType i = 0; i < np; ++ i ) {
    vector<SatLiteral> tmp_lits(nh);
    for ( SizeType j = 0; j < nh; ++ j ) {
      tmp_lits[j] = var_array[i * nh + j];
    }
    mSolver.add_clause(tmp_lits);
  }

  // 各々の穴に入る鳩は一羽だけ
  for ( SizeType j = 0; j < nh; ++ j ) {
    for ( SizeType i1 = 0; i1 < np - 1; ++ i1 ) {
      auto var1 = var_array[i1 * nh + j];
      for ( SizeType i2 = i1 + 1; i2 < np; ++ i2 ) {
	auto var2 = var_array[i2 * nh + j];
	mSolver.add_clause(~var1, ~var2);
      }
    }
  }

  int time_limit = 5;
  auto res = mSolver.solve(time_limit);

  return res;
}

TEST_P(TimerTest, test1)
{
  auto res = check();
  EXPECT_EQ( SatBool3::X, res );
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 TimerTest,
			 ::testing::Values("minisat", "minisat2", "glueminisat2",
					   "ymsat1", "ymsat2", "ymsat1_old"));

END_NAMESPACE_YM
