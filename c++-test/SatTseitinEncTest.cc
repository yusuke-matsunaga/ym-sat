
/// @file SatTseitinEncTest.cc
/// @brief SatTseitinEncTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatSolver.h"
#include "ym/SatTseitinEnc.h"
#include "ym/SatModel.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class SatTseitinEncTest :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief コンストラクタ
  SatTseitinEncTest();

  /// @brief デストラクタ
  ~SatTseitinEncTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 設定されたCNFが vals[] で示された真理値表と等しいか調べる．
  void
  check(int ni,
	const vector<int>& vals);

  /// @brief AND ゲートのチェックを行う．
  void
  check_and(int ni);

  /// @brief NAND ゲートのチェックを行う．
  void
  check_nand(int ni);

  /// @brief OR ゲートのチェックを行う．
  void
  check_or(int ni);

  /// @brief NOR ゲートのチェックを行う．
  void
  check_nor(int ni);

  /// @brief XOR ゲートのチェックを行う．
  void
  check_xor(int ni);

  /// @brief XNOR ゲートのチェックを行う．
  void
  check_xnor(int ni);

  /// @brief 論理ゲートの真理値表からチェック用のベクタを作る．
  void
  make_vals(int ni,
	    const vector<int>& tv,
	    bool inv,
	    vector<int>& vals);


public:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // Tseitin's encoder
  SatTseitinEnc mEnc;

  // 変数の数
  int mVarNum;

  // 変数のリスト
  vector<SatLiteral> mVarList;

  // 条件変数のリスト
  vector<SatLiteral> mCondVarList;
};

SatTseitinEncTest::SatTseitinEncTest() :
  mSolver(GetParam()),
  mEnc(mSolver),
  mVarNum(100),
  mVarList(mVarNum),
  mCondVarList(2)
{
  for ( int i: Range(mVarNum) ) {
    mVarList[i] = mSolver.new_variable();
  }
  mCondVarList[0] = mSolver.new_variable();
  mCondVarList[1] = mSolver.new_variable();
}

SatTseitinEncTest::~SatTseitinEncTest()
{
}

// @brief 設定されたCNFが vals[] で示された真理値表と等しいか調べる．
void
SatTseitinEncTest::check(int ni,
			 const vector<int>& vals)
{
  try {
    int np = 1U << ni;
    for ( int p: Range(np) ) {
      vector<SatLiteral> assumptions;
      for ( int i: Range(ni) ) {
	auto lit{mVarList[i]};
	if ( (p & (1 << i)) == 0 ) {
	  lit = ~lit;
	}
	assumptions.push_back(lit);
      }
      SatBool3 exp_ans = vals[p] ? SatBool3::True : SatBool3::False;
      SatModel model;
      SatBool3 stat = mSolver.solve(assumptions, model);
      EXPECT_EQ( exp_ans, stat );
    }
  }
  catch ( AssertError x ) {
    cout << x << endl;
  }
}

// @brief AND ゲートのチェックを行う．
void
SatTseitinEncTest::check_and(int ni)
{
  int np = 1 << ni;
  vector<int> tv(np);
  for ( int p: Range(np) ) {
    int val = 1;
    for ( int i: Range(ni) ) {
      if ( (p & (1 << i)) == 0 ) {
	val = 0;
	break;
      }
    }
    tv[p] = val;
  }

  vector<int> vals;
  make_vals(ni, tv, false, vals);

  check(ni + 1, vals);
}

// @brief NAND ゲートのチェックを行う．
void
SatTseitinEncTest::check_nand(int ni)
{
  int np = 1 << ni;
  vector<int> tv(np);
  for ( int p: Range(np) ) {
    int val = 1;
    for ( int i: Range(ni) ) {
      if ( (p & (1 << i)) == 0 ) {
	val = 0;
	break;
      }
    }
    tv[p] = val;
  }

  vector<int> vals;
  make_vals(ni, tv, true, vals);

  check(ni + 1, vals);
}

// @brief OR ゲートのチェックを行う．
void
SatTseitinEncTest::check_or(int ni)
{
  int np = 1 << ni;
  vector<int> tv(np);
  for ( int p: Range(np) ) {
    int val = 0;
    for ( int i: Range(ni) ) {
      if ( (p & (1 << i)) != 0 ) {
	val = 1;
	break;
      }
    }
    tv[p] = val;
  }

  vector<int> vals;
  make_vals(ni, tv, false, vals);

  check(ni + 1, vals);
}

// @brief NOR ゲートのチェックを行う．
void
SatTseitinEncTest::check_nor(int ni)
{
  int np = 1 << ni;
  vector<int> tv(np);
  for ( int p: Range(np) ) {
    int val = 0;
    for ( int i: Range(ni) ) {
      if ( (p & (1 << i)) != 0 ) {
	val = 1;
	break;
      }
    }
    tv[p] = val;
  }

  vector<int> vals;
  make_vals(ni, tv, true, vals);

  check(ni + 1, vals);
}

// @brief XOR ゲートのチェックを行う．
void
SatTseitinEncTest::check_xor(int ni)
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

  vector<int> vals;
  make_vals(ni, tv, false, vals);

  check(ni + 1, vals);
}

// @brief XNOR ゲートのチェックを行う．
void
SatTseitinEncTest::check_xnor(int ni)
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

  vector<int> vals;
  make_vals(ni, tv, true, vals);

  check(ni + 1, vals);
}

// @brief 論理ゲートの真理値表からチェック用のベクタを作る．
void
SatTseitinEncTest::make_vals(int ni,
			     const vector<int>& tv,
			     bool inv,
			     vector<int>& vals)
{
  int np = 1 << ni;
  int ni1 = ni + 1;
  int np1 = 1 << ni1;
  vals.clear();
  vals.resize(np1);
  int v0 = inv ? 1 : 0;
  int v1 = inv ? 0 : 1;
  for ( int p: Range(np) ) {
    vals[p +  0] = (tv[p] == v0);
    vals[p + np] = (tv[p] == v1);
  }
}

TEST_P(SatTseitinEncTest, add_buffgate)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mEnc.add_buffgate(lit1, lit2);

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

  mEnc.add_notgate(lit1, lit2);

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

  mEnc.add_andgate(olit, lit1, lit2);

  check_and(2);
}

TEST_P(SatTseitinEncTest, add_andgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mEnc.add_andgate(olit, lit1, lit2, lit3);

  check_and(3);
}

TEST_P(SatTseitinEncTest, add_andgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mEnc.add_andgate(olit, lit1, lit2, lit3, lit4);

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

  mEnc.add_andgate(olit, lits);

  check_and(5);
}

TEST_P(SatTseitinEncTest, add_nandgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mEnc.add_nandgate(olit, lit1, lit2);

  check_nand(2);
}

TEST_P(SatTseitinEncTest, add_nandgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mEnc.add_nandgate(olit, lit1, lit2, lit3);

  check_nand(3);
}

TEST_P(SatTseitinEncTest, add_nandgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mEnc.add_nandgate(olit, lit1, lit2, lit3, lit4);

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

  mEnc.add_nandgate(olit, lits);

  check_nand(5);
}

TEST_P(SatTseitinEncTest, add_orgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mEnc.add_orgate(olit, lit1, lit2);

  check_or(2);
}

TEST_P(SatTseitinEncTest, add_orgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mEnc.add_orgate(olit, lit1, lit2, lit3);

  check_or(3);
}

TEST_P(SatTseitinEncTest, add_orgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mEnc.add_orgate(olit, lit1, lit2, lit3, lit4);

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

  mEnc.add_orgate(olit, lits);

  check_or(5);
}

TEST_P(SatTseitinEncTest, add_norgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mEnc.add_norgate(olit, lit1, lit2);

  check_nor(2);
}

TEST_P(SatTseitinEncTest, add_norgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mEnc.add_norgate(olit, lit1, lit2, lit3);

  check_nor(3);
}

TEST_P(SatTseitinEncTest, add_norgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mEnc.add_norgate(olit, lit1, lit2, lit3, lit4);

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

  mEnc.add_norgate(olit, lits);

  check_nor(5);
}

TEST_P(SatTseitinEncTest, add_xorgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mEnc.add_xorgate(olit, lit1, lit2);

  check_xor(2);
}

TEST_P(SatTseitinEncTest, add_xorgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mEnc.add_xorgate(olit, lit1, lit2, lit3);

  check_xor(3);
}

TEST_P(SatTseitinEncTest, add_xorgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mEnc.add_xorgate(olit, lit1, lit2, lit3, lit4);

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

  mEnc.add_xorgate(olit, lits);

  check_xor(5);
}

TEST_P(SatTseitinEncTest, add_xnorgate2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mEnc.add_xnorgate(olit, lit1, lit2);

  check_xnor(2);
}

TEST_P(SatTseitinEncTest, add_xnorgate3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mEnc.add_xnorgate(olit, lit1, lit2, lit3);

  check_xnor(3);
}

TEST_P(SatTseitinEncTest, add_xnorgate4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mEnc.add_xnorgate(olit, lit1, lit2, lit3, lit4);

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

  mEnc.add_xnorgate(olit, lits);

  check_xnor(5);
}

INSTANTIATE_TEST_CASE_P(SatSolverTest,
			SatTseitinEncTest,
			::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					  "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
