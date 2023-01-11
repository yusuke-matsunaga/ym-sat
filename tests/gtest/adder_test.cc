
/// @file adder_test.cc
/// @brief SatSolver::add_adder() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class AdderTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  AdderTest() : SatTestFixture() { }

  /// @brief デストラクタ
  ~AdderTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief adder のチェックを行う．
  void
  check(
    int na, ///< [in] a のビット幅
    int nb, ///< [in] b のビット幅
    int ns  ///< [in] s のビット幅
  );

};

// @brief adder のチェックを行う．
void
AdderTest::check(
  int na,
  int nb,
  int ns
)
{
  ASSERT_COND( na <= ns );
  ASSERT_COND( nb <= ns );

  ASSERT_COND( na + nb + ns + 2 < mVarNum );

  int vpos = 0;
  vector<SatLiteral> alits(na);
  for ( int i = 0; i < na; ++ i ) {
    alits[i] = mVarList[vpos];
    ++ vpos;
  }
  vector<SatLiteral> blits(nb);
  for ( int i = 0; i < nb; ++ i ) {
    blits[i] = mVarList[vpos];
    ++ vpos;
  }
  SatLiteral ilit = mVarList[vpos];
  ++ vpos;
  vector<SatLiteral> slits(ns);
  for ( int i = 0; i < ns; ++ i ) {
    slits[i] = mVarList[vpos];
    ++ vpos;
  }
  SatLiteral olit = mVarList[vpos];
  ++ vpos;

  mSolver.add_adder(alits, blits, ilit, slits, olit);

  int nexp = (1 << vpos);
  vector<int> vals(nexp);
  int amask = (1 << na) - 1;
  int ashift = 0;
  int bmask = (1 << nb) - 1;
  int bshift = na;
  int ishift = na + nb;
  int smask = (1 << ns) - 1;
  int sshift = na + nb + 1;
  int oshift = na + nb + 1 + ns;
  for ( int bit = 0; bit < nexp; ++ bit ) {
    int a = (bit >> ashift) & amask;
    int b = (bit >> bshift) & bmask;
    int i = (bit >> ishift) & 1;
    int s = (bit >> sshift) & smask;
    int o = (bit >> oshift) & 1;
    int s_exp = a + b + i;
    int o_exp = 0;
    if ( s_exp >= (1 << ns) ) {
      o_exp = 1;
      s_exp -= (1 << ns);
    }
    if ( s == s_exp && o == o_exp ) {
      vals[bit] = 1;
    }
    else {
      vals[bit] = 0;
    }
  }

  SatTestFixture::check(vpos, vals);
}

TEST_P(AdderTest, add_adder_4_4_4)
{
  check(4, 4, 4);
}

TEST_P(AdderTest, add_adder_4_3_4)
{
  check(4, 3, 4);
}

TEST_P(AdderTest, add_adder_4_2_4)
{
  check(4, 2, 4);
}

TEST_P(AdderTest, add_adder_3_4_4)
{
  check(3, 4, 4);
}

TEST_P(AdderTest, add_adder_2_4_4)
{
  check(2, 4, 4);
}

TEST_P(AdderTest, add_adder_3_3_4)
{
  check(3, 3, 4);
}

TEST_P(AdderTest, add_adder_3_2_4)
{
  check(3, 2, 4);
}

TEST_P(AdderTest, add_adder_3_1_4)
{
  check(3, 1, 4);
}

TEST_P(AdderTest, add_adder_3_1_5)
{
  check(3, 1, 5);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 AdderTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
