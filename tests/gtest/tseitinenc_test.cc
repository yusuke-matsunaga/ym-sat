
/// @file SatTseitinEncTest.cc
/// @brief SatTseitinEncTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class SatTseitinEncTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  SatTseitinEncTest() : SatTestFixture() { }

  /// @brief デストラクタ
  ~SatTseitinEncTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief AND ゲートのチェックを行う．
  void
  check_and(
    int ni
  );

  /// @brief NAND ゲートのチェックを行う．
  void
  check_nand(
    int ni
  );

  /// @brief OR ゲートのチェックを行う．
  void
  check_or(
    int ni
  );

  /// @brief NOR ゲートのチェックを行う．
  void
  check_nor(
    int ni
  );

  /// @brief XOR ゲートのチェックを行う．
  void
  check_xor(
    int ni
  );

  /// @brief XNOR ゲートのチェックを行う．
  void
  check_xnor(
    int ni
  );

  /// @brief AND ゲートの真理値表を作る．
  static
  vector<int>
  make_and_tv(
    int ni
  );

  /// @brief OR ゲートの真理値表を作る．
  static
  vector<int>
  make_or_tv(
    int ni
  );

  /// @brief XOR ゲートの真理値表を作る．
  static
  vector<int>
  make_xor_tv(
    int ni
  );

};


// @brief AND ゲートのチェックを行う．
void
SatTseitinEncTest::check_and(
  int ni
)
{
  auto tv = make_and_tv(ni);
  auto vals = make_vals(ni, tv, false);
  check(ni + 1, vals);
}

// @brief NAND ゲートのチェックを行う．
void
SatTseitinEncTest::check_nand(
  int ni
)
{
  auto tv = make_and_tv(ni);
  auto vals = make_vals(ni, tv, true);
  check(ni + 1, vals);
}

// @brief OR ゲートのチェックを行う．
void
SatTseitinEncTest::check_or(
  int ni
)
{
  auto tv = make_or_tv(ni);
  auto vals = make_vals(ni, tv, false);
  check(ni + 1, vals);
}

// @brief NOR ゲートのチェックを行う．
void
SatTseitinEncTest::check_nor(
  int ni
)
{
  auto tv = make_or_tv(ni);
  auto vals = make_vals(ni, tv, true);
  check(ni + 1, vals);
}

// @brief XOR ゲートのチェックを行う．
void
SatTseitinEncTest::check_xor(
  int ni
)
{
  auto tv = make_xor_tv(ni);
  auto vals = make_vals(ni, tv, false);
  check(ni + 1, vals);
}

// @brief XNOR ゲートのチェックを行う．
void
SatTseitinEncTest::check_xnor(
  int ni
)
{
  auto tv = make_xor_tv(ni);
  auto vals = make_vals(ni, tv, true);
  check(ni + 1, vals);
}

// @brief AND ゲートの真理値表を作る．
vector<int>
SatTseitinEncTest::make_and_tv(
  int ni
)
{
  int np = 1 << ni;
  vector<int> tv(np, 1);
  for ( int p: Range(np) ) {
    for ( int i: Range(ni) ) {
      if ( (p & (1 << i)) == 0 ) {
	tv[p] = 0;
	break;
      }
    }
  }
  return tv;
}

// @brief OR ゲートの真理値表を作る．
vector<int>
SatTseitinEncTest::make_or_tv(
  int ni
)
{
  int np = 1 << ni;
  vector<int> tv(np, 0);
  for ( int p: Range(np) ) {
    int val = 0;
    for ( int i: Range(ni) ) {
      if ( p & (1 << i) ) {
	tv[p] = 1;
	break;
      }
    }
  }
  return tv;
}

// @brief XOR ゲートの真理値表を作る．
vector<int>
SatTseitinEncTest::make_xor_tv(
  int ni
)
{
  int np = 1 << ni;
  vector<int> tv(np);
  for ( int p: Range(np) ) {
    int val = 0;
    for ( int i: Range(ni) ) {
      if ( (p & (1 << i)) != 0 ) {
	val ^= 1;
      }
    }
    tv[p] = val;
  }
  return tv;
}

TEST_P(SatTseitinEncTest, add_buffgate)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_buffgate(lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    1
     //   0    1    0
     //   1    0    0
     //   1    1    1
     1, 0, 0, 1
    }
  );

  check(2, vals);
}

TEST_P(SatTseitinEncTest, add_notgate)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_notgate(lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    0
     //   0    1    1
     //   1    0    1
     //   1    1    0
     0, 1, 1, 0
    }
  );

  check(2, vals);
}

TEST_P(SatTseitinEncTest, add_andgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_andgate(olit, lit1, lit2);

  check_and(2);
}

TEST_P(SatTseitinEncTest, add_andgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_andgate(olit, lit1, lit2, lit3);

  check_and(3);
}

TEST_P(SatTseitinEncTest, add_andgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_andgate(olit, lit1, lit2, lit3, lit4);

  check_and(4);
}

TEST_P(SatTseitinEncTest, add_andgate5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral olit(mVarList[5]);

  vector<SatLiteral> lits(5);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;

  mSolver.add_andgate(olit, lits);

  check_and(5);
}

TEST_P(SatTseitinEncTest, add_nandgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_nandgate(olit, lit1, lit2);

  check_nand(2);
}

TEST_P(SatTseitinEncTest, add_nandgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_nandgate(olit, lit1, lit2, lit3);

  check_nand(3);
}

TEST_P(SatTseitinEncTest, add_nandgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_nandgate(olit, lit1, lit2, lit3, lit4);

  check_nand(4);
}

TEST_P(SatTseitinEncTest, add_nandgate5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral olit(mVarList[5]);

  vector<SatLiteral> lits(5);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;

  mSolver.add_nandgate(olit, lits);

  check_nand(5);
}

TEST_P(SatTseitinEncTest, add_orgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_orgate(olit, lit1, lit2);

  check_or(2);
}

TEST_P(SatTseitinEncTest, add_orgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_orgate(olit, lit1, lit2, lit3);

  check_or(3);
}

TEST_P(SatTseitinEncTest, add_orgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_orgate(olit, lit1, lit2, lit3, lit4);

  check_or(4);
}

TEST_P(SatTseitinEncTest, add_orgate5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral olit(mVarList[5]);

  vector<SatLiteral> lits(5);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;

  mSolver.add_orgate(olit, lits);

  check_or(5);
}

TEST_P(SatTseitinEncTest, add_norgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_norgate(olit, lit1, lit2);

  check_nor(2);
}

TEST_P(SatTseitinEncTest, add_norgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_norgate(olit, lit1, lit2, lit3);

  check_nor(3);
}

TEST_P(SatTseitinEncTest, add_norgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_norgate(olit, lit1, lit2, lit3, lit4);

  check_nor(4);
}

TEST_P(SatTseitinEncTest, add_norgate5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral olit(mVarList[5]);

  vector<SatLiteral> lits(5);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;

  mSolver.add_norgate(olit, lits);

  check_nor(5);
}

TEST_P(SatTseitinEncTest, add_xorgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_xorgate(olit, lit1, lit2);

  check_xor(2);
}

TEST_P(SatTseitinEncTest, add_xorgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_xorgate(olit, lit1, lit2, lit3);

  check_xor(3);
}

TEST_P(SatTseitinEncTest, add_xorgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_xorgate(olit, lit1, lit2, lit3, lit4);

  check_xor(4);
}

TEST_P(SatTseitinEncTest, add_xorgate5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral olit(mVarList[5]);

  vector<SatLiteral> lits(5);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;

  mSolver.add_xorgate(olit, lits);

  check_xor(5);
}

TEST_P(SatTseitinEncTest, add_xnorgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_xnorgate(olit, lit1, lit2);

  check_xnor(2);
}

TEST_P(SatTseitinEncTest, add_xnorgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_xnorgate(olit, lit1, lit2, lit3);

  check_xnor(3);
}

TEST_P(SatTseitinEncTest, add_xnorgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_xnorgate(olit, lit1, lit2, lit3, lit4);

  check_xnor(4);
}

TEST_P(SatTseitinEncTest, add_xnorgate5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral olit(mVarList[5]);

  vector<SatLiteral> lits(5);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;

  mSolver.add_xnorgate(olit, lits);

  check_xnor(5);
}

TEST_P(SatTseitinEncTest, add_half_adder)
{
  SatLiteral alit(mVarList[0]);
  SatLiteral blit(mVarList[1]);
  SatLiteral slit(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_half_adder(alit, blit, slit, olit);

  vector<int> vals(16);
  // olit, slit, blit, alit
  for ( int bit = 0; bit < 16; ++ bit ) {
    int a = ((bit >> 0) & 1);
    int b = ((bit >> 1) & 1);
    int s = ((bit >> 2) & 1);
    int o = ((bit >> 3) & 1);
    int exp_s = (a + b) % 2;
    int exp_o = (a + b) / 2;
    if ( s == exp_s && o == exp_o ) {
      vals[bit] = 1;
    }
    else {
      vals[bit] = 0;
    }
  }

  check(4, vals);
}

TEST_P(SatTseitinEncTest, add_full_adder)
{
  SatLiteral alit(mVarList[0]);
  SatLiteral blit(mVarList[1]);
  SatLiteral clit(mVarList[2]);
  SatLiteral slit(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_full_adder(alit, blit, clit, slit, olit);

  vector<int> vals(32);
  // olit, slit, clit, blit, alit
  for ( int bit = 0; bit < 32; ++ bit ) {
    int a = ((bit >> 0) & 1);
    int b = ((bit >> 1) & 1);
    int c = ((bit >> 2) & 1);
    int s = ((bit >> 3) & 1);
    int o = ((bit >> 4) & 1);
    int exp_s = (a + b + c) % 2;
    int exp_o = (a + b + c) / 2;
    if ( s == exp_s && o == exp_o ) {
      vals[bit] = 1;
    }
    else {
      vals[bit] = 0;
    }
  }

  check(5, vals);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 SatTseitinEncTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
