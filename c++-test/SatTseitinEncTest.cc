
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

  /// @brief adder のチェックを行う．
  /// @param[in] na, nb 入力のビット幅
  /// @param[in] ns 出力のビット幅
  void
  check_adder(int na,
	      int nb,
	      int ns);

  /// @brief counter のチェックを行う．
  /// @param[in] ni 入力のビット幅
  /// @param[in] no 出力のビット幅
  void
  check_counter(int ni,
		int no);

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

// @brief adder のチェックを行う．
// @param[in] na, nb 入力のビット幅
// @param[in] ns 出力のビット幅
void
SatTseitinEncTest::check_adder(int na,
			       int nb,
			       int ns)
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

  mEnc.add_adder(alits, blits, ilit, slits, olit);

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

  check(vpos, vals);
}

// @brief counter のチェックを行う．
// @param[in] ni 入力のビット幅
// @param[in] no 出力のビット幅
void
SatTseitinEncTest::check_counter(int ni,
				 int no)
{
  int no_exp = (1 << no);

  ASSERT_COND( ni < no_exp );
  ASSERT_COND( ni + no < mVarNum );

  int vpos = 0;
  vector<SatLiteral> ilits(ni);
  vector<SatLiteral> olits(no);
  for ( int i = 0; i < ni; ++ i ) {
    ilits[i] = mVarList[vpos];
    ++ vpos;
  }
  for ( int i = 0; i < no; ++ i ) {
    olits[i] = mVarList[vpos];
    ++ vpos;
  }

  mEnc.add_counter(ilits, olits);

  int nexp = (1 << vpos);
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

  check(vpos, vals);
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

TEST_P(SatTseitinEncTest, add_half_adder)
{
  SatLiteral alit(mVarList[0]);
  SatLiteral blit(mVarList[1]);
  SatLiteral slit(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mEnc.add_half_adder(alit, blit, slit, olit);

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

  mEnc.add_full_adder(alit, blit, clit, slit, olit);

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

TEST_P(SatTseitinEncTest, add_adder_4_4_4)
{
  check_adder(4, 4, 4);
}

TEST_P(SatTseitinEncTest, add_adder_4_3_4)
{
  check_adder(4, 3, 4);
}

TEST_P(SatTseitinEncTest, add_adder_4_2_4)
{
  check_adder(4, 2, 4);
}

TEST_P(SatTseitinEncTest, add_adder_3_4_4)
{
  check_adder(3, 4, 4);
}

TEST_P(SatTseitinEncTest, add_adder_2_4_4)
{
  check_adder(2, 4, 4);
}

TEST_P(SatTseitinEncTest, add_adder_3_3_4)
{
  check_adder(3, 3, 4);
}

TEST_P(SatTseitinEncTest, add_adder_3_2_4)
{
  check_adder(3, 2, 4);
}

TEST_P(SatTseitinEncTest, add_adder_3_1_4)
{
  check_adder(3, 1, 4);
}

TEST_P(SatTseitinEncTest, add_adder_3_1_5)
{
  check_adder(3, 1, 5);
}

TEST_P(SatTseitinEncTest, add_counter_2_2)
{
  check_counter(2, 2);
}

TEST_P(SatTseitinEncTest, add_counter_2_4)
{
  check_counter(2, 4);
}

TEST_P(SatTseitinEncTest, add_counter_3_2)
{
  check_counter(3, 2);
}

TEST_P(SatTseitinEncTest, add_counter_4_3)
{
  check_counter(4, 3);
}

TEST_P(SatTseitinEncTest, add_counter_5_3)
{
  check_counter(5, 3);
}

TEST_P(SatTseitinEncTest, add_counter_6_3)
{
  check_counter(6, 3);
}

TEST_P(SatTseitinEncTest, add_counter_7_3)
{
  check_counter(7, 3);
}

TEST_P(SatTseitinEncTest, add_counter_8_4)
{
  check_counter(8, 4);
}

TEST_P(SatTseitinEncTest, add_counter_9_4)
{
  check_counter(9, 4);
}

TEST_P(SatTseitinEncTest, add_counter_10_4)
{
  check_counter(10, 4);
}

TEST_P(SatTseitinEncTest, add_counter_11_4)
{
  check_counter(11, 4);
}

TEST_P(SatTseitinEncTest, add_counter_12_4)
{
  check_counter(12, 4);
}

TEST_P(SatTseitinEncTest, add_counter_13_4)
{
  check_counter(13, 4);
}

TEST_P(SatTseitinEncTest, add_counter_14_4)
{
  check_counter(14, 4);
}

TEST_P(SatTseitinEncTest, add_counter_15_4)
{
  check_counter(15, 4);
}

TEST_P(SatTseitinEncTest, add_counter_16_5)
{
  check_counter(16, 5);
}

INSTANTIATE_TEST_CASE_P(SatSolverTest,
			SatTseitinEncTest,
			::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					  "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
