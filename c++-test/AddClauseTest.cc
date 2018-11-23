
/// @file AddClauseTest.cc
/// @brief AddClauseTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatSolver.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class AddClauseTest :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief コンストラクタ
  AddClauseTest();

  /// @brief デストラクタ
  ~AddClauseTest();


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

  /// @brief at_most_k のチェックを行う．
  void
  check_at_most(int n,
		int k);

  /// @brief at_least_k のチェックを行う．
  void
  check_at_least(int n,
		 int k);

  /// @brief exact_k のチェックを行う．
  void
  check_exact(int n,
	      int k);

  /// @brief not_one のチェックを行う．
  void
  check_not_one(int n);

  /// @brief check の条件リテラル付き版
  ///
  /// 最初の変数が1のときのみ意味を持つ．
  void
  check_with_cond1(int ni,
		   const vector<int>& vals);

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

  // 変数の数
  int mVarNum;

  // 変数のリスト
  vector<SatVarId> mVarList;

  // 条件変数のリスト
  vector<SatVarId> mCondVarList;
};

AddClauseTest::AddClauseTest() :
  mSolver(GetParam()),
  mVarNum(100),
  mVarList(mVarNum),
  mCondVarList(2)
{
  for ( int i: Range(mVarNum) ) {
    SatVarId var = mSolver.new_variable();
    mVarList[i] = var;
  }
  mCondVarList[0] = mSolver.new_variable();
  mCondVarList[1] = mSolver.new_variable();
}

AddClauseTest::~AddClauseTest()
{
}

// @brief 設定されたCNFが vals[] で示された真理値表と等しいか調べる．
void
AddClauseTest::check(int ni,
		     const vector<int>& vals)
{
  try {
    int np = 1U << ni;
    for ( int p: Range(np) ) {
      vector<SatLiteral> assumptions;
      for ( int i: Range(ni) ) {
	bool inv = (p & (1U << i)) ? false : true;
	assumptions.push_back(SatLiteral(mVarList[i], inv));
      }
      SatBool3 exp_ans = vals[p] ? SatBool3::True : SatBool3::False;
      vector<SatBool3> model;
      SatBool3 stat = mSolver.solve(assumptions, model);
      EXPECT_EQ( exp_ans, stat );
    }
  }
  catch ( AssertError x ) {
    cout << x << endl;
  }
}

// @brief check の条件リテラル付き版
//
// 最初の変数が1のときのみ意味を持つ．
void
AddClauseTest::check_with_cond1(int ni,
				const vector<int>& vals)
{
  try {
    int np = 1U << ni;
    for ( int p: Range(np) ) {
      vector<SatLiteral> assumptions;
      assumptions.push_back(~SatLiteral(mCondVarList[0]));
      for ( int i: Range(ni) ) {
	bool inv = (p & (1U << i)) ? false : true;
	assumptions.push_back(SatLiteral(mVarList[i], inv));
      }
      SatBool3 exp_ans = SatBool3::True;
      vector<SatBool3> model;
      SatBool3 stat = mSolver.solve(assumptions, model);
      EXPECT_EQ( exp_ans, stat );
    }
    for ( int p: Range(np) ) {
      vector<SatLiteral> assumptions;
      assumptions.push_back(SatLiteral(mCondVarList[0]));
      for ( int i: Range(ni) ) {
	bool inv = (p & (1U << i)) ? false : true;
	assumptions.push_back(SatLiteral(mVarList[i], inv));
      }
      SatBool3 exp_ans = vals[p] ? SatBool3::True : SatBool3::False;
      vector<SatBool3> model;
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
AddClauseTest::check_and(int ni)
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
AddClauseTest::check_nand(int ni)
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
AddClauseTest::check_or(int ni)
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
AddClauseTest::check_nor(int ni)
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
AddClauseTest::check_xor(int ni)
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
AddClauseTest::check_xnor(int ni)
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

// @brief at_most_k のチェックを行う．
void
AddClauseTest::check_at_most(int n,
			     int k)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    for ( int i: Range(n) ) {
      bool inv;
      if ( p & (1U << i) ) {
	++ c;
	inv = false;
      }
      else {
	inv = true;
      }
      assumptions.push_back(SatLiteral(mVarList[i], inv));
    }
    SatBool3 exp_ans = (c <= k) ? SatBool3::True : SatBool3::False;
    vector<SatBool3> model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    EXPECT_EQ( exp_ans, stat );
  }
}

// @brief at_least_k のチェックを行う．
void
AddClauseTest::check_at_least(int n,
			      int k)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    for ( int i: Range(n) ) {
      bool inv;
      if ( p & (1U << i) ) {
	++ c;
	inv = false;
      }
      else {
	inv = true;
      }
      assumptions.push_back(SatLiteral(mVarList[i], inv));
    }
    SatBool3 exp_ans = (c >= k) ? SatBool3::True : SatBool3::False;
    vector<SatBool3> model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    EXPECT_EQ( exp_ans, stat );
  }
}

// @brief exact_k のチェックを行う．
void
AddClauseTest::check_exact(int n,
			   int k)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    for ( int i: Range(n) ) {
      bool inv;
      if ( p & (1U << i) ) {
	++ c;
	inv = false;
      }
      else {
	inv = true;
      }
      assumptions.push_back(SatLiteral(mVarList[i], inv));
    }
    SatBool3 exp_ans = (c == k) ? SatBool3::True : SatBool3::False;
    vector<SatBool3> model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    EXPECT_EQ( exp_ans, stat );
  }
}

// @brief not_one のチェックを行う．
void
AddClauseTest::check_not_one(int n)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    for ( int i: Range(n) ) {
      bool inv;
      if ( p & (1U << i) ) {
	++ c;
	inv = false;
      }
      else {
	inv = true;
      }
      assumptions.push_back(SatLiteral(mVarList[i], inv));
    }
    SatBool3 exp_ans = (c != 1) ? SatBool3::True : SatBool3::False;
    vector<SatBool3> model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    EXPECT_EQ( exp_ans, stat );
  }
}

// @brief 論理ゲートの真理値表からチェック用のベクタを作る．
void
AddClauseTest::make_vals(int ni,
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

TEST_P(AddClauseTest, add_clause1_1)
{
  SatLiteral lit1(mVarList[0]);

  mSolver.add_clause(lit1);

  vector<int> vals(
    {
     // lit1 ans
     //   0    0
     //   1    1
     0,
     1
    }
  );

  check(1, vals);
}

TEST_P(AddClauseTest, add_clause1_2)
{
  SatLiteral lit1(mVarList[0]);

  mSolver.add_clause(~lit1);

  vector<int> vals(
    {
     // lit1 ans
     //   0    1
     //   1    0
     1,
     0
    }
  );

  check(1, vals);
}

TEST_P(AddClauseTest, add_clause2_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_clause(lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    0
     //   0    1    1
     //   1    0    1
     //   1    1    1
     0,
     1,
     1,
     1
    }
  );

  check(2, vals);
}

TEST_P(AddClauseTest, add_clause2_2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_clause(~lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    1
     //   0    1    0
     //   1    0    1
     //   1    1    1
     1,
     0,
     1,
     1
    }
  );

  check(2, vals);
}

TEST_P(AddClauseTest, add_clause3_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_clause(lit1, lit2, lit3);

  vector<int> vals(
    {
     // lit3 lit2 lit1 ans
     //   0    0    0    0
     //   0    0    1    1
     //   0    1    0    1
     //   0    1    1    1
     //   1    0    0    1
     //   1    0    1    1
     //   1    1    0    1
     //   1    1    1    1
     0,
     1,
     1,
     1,
     1,
     1,
     1,
     1
    }
  );

  check(3, vals);
}

TEST_P(AddClauseTest, add_clause3_2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_clause(~lit1, lit2, ~lit3);

  vector<int> vals(
    {
     // lit3 lit2 lit1 ans
     //   0    0    0    1
     //   0    0    1    1
     //   0    1    0    1
     //   0    1    1    1
     //   1    0    0    1
     //   1    0    1    0
     //   1    1    0    1
     //   1    1    1    1
     1,
     1,
     1,
     1,
     1,
     0,
     1,
     1
    }
  );

  check(3, vals);
}

TEST_P(AddClauseTest, add_clause4_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_clause(lit1, lit2, lit3, lit4);

  vector<int> vals(
    {
     // lit4 lit3 lit2 lit1 ans
     //   0    0    0    0    0
     //   0    0    0    1    1
     //   0    0    1    0    1
     //   0    0    1    1    1
     //   0    1    0    0    1
     //   0    1    0    1    1
     //   0    1    1    0    1
     //   0    1    1    1    1
     //   1    0    0    0    1
     //   1    0    0    1    1
     //   1    0    1    0    1
     //   1    0    1    1    1
     //   1    1    0    0    1
     //   1    1    0    1    1
     //   1    1    1    0    1
     //   1    1    1    1    1
     0,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1
    }
  );

  check(4, vals);
}

TEST_P(AddClauseTest, add_clause5_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_clause(lit1, lit2, lit3, lit4, lit5);

  vector<int> vals(
    {
     // lit5 lit4 lit3 lit2 lit1  ans
     //    0    0    0    0    0    0
     //    0    0    0    0    1    1
     //    0    0    0    1    0    1
     //    0    0    0    1    1    1
     //    0    0    1    0    0    1
     //    0    0    1    0    1    1
     //    0    0    1    1    0    1
     //    0    0    1    1    1    1
     //    0    1    0    0    0    1
     //    0    1    0    0    1    1
     //    0    1    0    1    0    1
     //    0    1    0    1    1    1
     //    0    1    1    0    0    1
     //    0    1    1    0    1    1
     //    0    1    1    1    0    1
     //    0    1    1    1    1    1
     //    1    0    0    0    0    1
     //    1    0    0    0    1    1
     //    1    0    0    1    0    1
     //    1    0    0    1    1    1
     //    1    0    1    0    0    1
     //    1    0    1    0    1    1
     //    1    0    1    1    0    1
     //    1    0    1    1    1    1
     //    1    1    0    0    0    1
     //    1    1    0    0    1    1
     //    1    1    0    1    0    1
     //    1    1    0    1    1    1
     //    1    1    1    0    0    1
     //    1    1    1    0    1    1
     //    1    1    1    1    0    1
     //    1    1    1    1    1    1
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
  );

  check(5, vals);
}

TEST_P(AddClauseTest, add_clause5n_1)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  vector<SatLiteral> tmp_lits(5);
  tmp_lits[0] = lit1;
  tmp_lits[1] = lit2;
  tmp_lits[2] = lit3;
  tmp_lits[3] = lit4;
  tmp_lits[4] = lit5;

  mSolver.add_clause(tmp_lits);

  vector<int> vals(
    {
     // lit5 lit4 lit3 lit2 lit1  ans
     //    0    0    0    0    0    0
     //    0    0    0    0    1    1
     //    0    0    0    1    0    1
     //    0    0    0    1    1    1
     //    0    0    1    0    0    1
     //    0    0    1    0    1    1
     //    0    0    1    1    0    1
     //    0    0    1    1    1    1
     //    0    1    0    0    0    1
     //    0    1    0    0    1    1
     //    0    1    0    1    0    1
     //    0    1    0    1    1    1
     //    0    1    1    0    0    1
     //    0    1    1    0    1    1
     //    0    1    1    1    0    1
     //    0    1    1    1    1    1
     //    1    0    0    0    0    1
     //    1    0    0    0    1    1
     //    1    0    0    1    0    1
     //    1    0    0    1    1    1
     //    1    0    1    0    0    1
     //    1    0    1    0    1    1
     //    1    0    1    1    0    1
     //    1    0    1    1    1    1
     //    1    1    0    0    0    1
     //    1    1    0    0    1    1
     //    1    1    0    1    0    1
     //    1    1    0    1    1    1
     //    1    1    1    0    0    1
     //    1    1    1    0    1    1
     //    1    1    1    1    0    1
     //    1    1    1    1    1    1
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
  );

  check(5, vals);
}

TEST_P(AddClauseTest, add_eq_rel)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_eq_rel(lit1, lit2);

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

TEST_P(AddClauseTest, add_neq_rel)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_neq_rel(lit1, lit2);

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

TEST_P(AddClauseTest, add_andgate_rel2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_andgate_rel(olit, lit1, lit2);

  check_and(2);
}

TEST_P(AddClauseTest, add_andgate_rel3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_andgate_rel(olit, lit1, lit2, lit3);

  check_and(3);
}

TEST_P(AddClauseTest, add_andgate_rel4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_andgate_rel(olit, lit1, lit2, lit3, lit4);

  check_and(4);
}

TEST_P(AddClauseTest, add_andgate_rel5)
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

  mSolver.add_andgate_rel(olit, lits);

  check_and(5);
}

TEST_P(AddClauseTest, add_nandgate_rel2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_nandgate_rel(olit, lit1, lit2);

  check_nand(2);
}

TEST_P(AddClauseTest, add_nandgate_rel3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_nandgate_rel(olit, lit1, lit2, lit3);

  check_nand(3);
}

TEST_P(AddClauseTest, add_nandgate_rel4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_nandgate_rel(olit, lit1, lit2, lit3, lit4);

  check_nand(4);
}

TEST_P(AddClauseTest, add_nandgate_rel5)
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

  mSolver.add_nandgate_rel(olit, lits);

  check_nand(5);
}

TEST_P(AddClauseTest, add_orgate_rel2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_orgate_rel(olit, lit1, lit2);

  check_or(2);
}

TEST_P(AddClauseTest, add_orgate_rel3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_orgate_rel(olit, lit1, lit2, lit3);

  check_or(3);
}

TEST_P(AddClauseTest, add_orgate_rel4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_orgate_rel(olit, lit1, lit2, lit3, lit4);

  check_or(4);
}

TEST_P(AddClauseTest, add_orgate_rel5)
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

  mSolver.add_orgate_rel(olit, lits);

  check_or(5);
}

TEST_P(AddClauseTest, add_norgate_rel2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_norgate_rel(olit, lit1, lit2);

  check_nor(2);
}

TEST_P(AddClauseTest, add_norgate_rel3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_norgate_rel(olit, lit1, lit2, lit3);

  check_nor(3);
}

TEST_P(AddClauseTest, add_norgate_rel4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_norgate_rel(olit, lit1, lit2, lit3, lit4);

  check_nor(4);
}

TEST_P(AddClauseTest, add_norgate_rel5)
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

  mSolver.add_norgate_rel(olit, lits);

  check_nor(5);
}

TEST_P(AddClauseTest, add_xorgate_rel2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_xorgate_rel(olit, lit1, lit2);

  check_xor(2);
}

TEST_P(AddClauseTest, add_xorgate_rel3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_xorgate_rel(olit, lit1, lit2, lit3);

  check_xor(3);
}

TEST_P(AddClauseTest, add_xorgate_rel4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_xorgate_rel(olit, lit1, lit2, lit3, lit4);

  check_xor(4);
}

TEST_P(AddClauseTest, add_xorgate_rel5)
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

  mSolver.add_xorgate_rel(olit, lits);

  check_xor(5);
}

TEST_P(AddClauseTest, add_xnorgate_rel2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral olit(mVarList[2]);

  mSolver.add_xnorgate_rel(olit, lit1, lit2);

  check_xnor(2);
}

TEST_P(AddClauseTest, add_xnorgate_rel3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral olit(mVarList[3]);

  mSolver.add_xnorgate_rel(olit, lit1, lit2, lit3);

  check_xnor(3);
}

TEST_P(AddClauseTest, add_xnorgate_rel4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral olit(mVarList[4]);

  mSolver.add_xnorgate_rel(olit, lit1, lit2, lit3, lit4);

  check_xnor(4);
}

TEST_P(AddClauseTest, add_xnorgate_rel5)
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

  mSolver.add_xnorgate_rel(olit, lits);

  check_xnor(5);
}

TEST_P(AddClauseTest, add_at_most_one2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_at_most_one(lit1, lit2);

  check_at_most(2, 1);
}

TEST_P(AddClauseTest, add_at_most_one3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_at_most_one(lit1, lit2, lit3);

  check_at_most(3, 1);
}

TEST_P(AddClauseTest, add_at_most_one4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4);

  check_at_most(4, 1);
}

TEST_P(AddClauseTest, add_at_most_one5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4, lit5);

  check_at_most(5, 1);
}

TEST_P(AddClauseTest, add_at_most_one6)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_most(6, 1);
}

TEST_P(AddClauseTest, add_at_most_oneN)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);
  SatLiteral lit7(mVarList[6]);

  vector<SatLiteral> lits(7);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;
  lits[5] = lit6;
  lits[6] = lit7;

  mSolver.add_at_most_one(lits);

  check_at_most(7, 1);
}

// add_at_most_two2 は自明なのでやらない．

TEST_P(AddClauseTest, add_at_most_two3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_at_most_two(lit1, lit2, lit3);

  check_at_most(3, 2);
}

TEST_P(AddClauseTest, add_at_most_two4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4);

  check_at_most(4, 2);
}

TEST_P(AddClauseTest, add_at_most_two5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4, lit5);

  check_at_most(5, 2);
}

TEST_P(AddClauseTest, add_at_most_two6)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_most(6, 2);
}

TEST_P(AddClauseTest, add_at_most_twoN)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);
  SatLiteral lit7(mVarList[6]);

  vector<SatLiteral> lits(7);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;
  lits[5] = lit6;
  lits[6] = lit7;

  mSolver.add_at_most_two(lits);

  check_at_most(7, 2);
}

TEST_P(AddClauseTest, add_at_most_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = SatLiteral(mVarList[i]);
  }

  mSolver.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(AddClauseTest, add_at_most_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = SatLiteral(mVarList[i]);
  }

  mSolver.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(AddClauseTest, add_at_least_one2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_at_least_one(lit1, lit2);

  check_at_least(2, 1);
}

TEST_P(AddClauseTest, add_at_least_one3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_at_least_one(lit1, lit2, lit3);

  check_at_least(3, 1);
}

TEST_P(AddClauseTest, add_at_least_one4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_at_least_one(lit1, lit2, lit3, lit4);

  check_at_least(4, 1);
}

TEST_P(AddClauseTest, add_at_least_one5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_at_least_one(lit1, lit2, lit3, lit4, lit5);

  check_at_least(5, 1);
}

TEST_P(AddClauseTest, add_at_least_one6)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);

  mSolver.add_at_least_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_least(6, 1);
}

TEST_P(AddClauseTest, add_at_least_oneN)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);
  SatLiteral lit7(mVarList[6]);

  vector<SatLiteral> lits(7);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;
  lits[5] = lit6;
  lits[6] = lit7;

  mSolver.add_at_least_one(lits);

  check_at_least(7, 1);
}

TEST_P(AddClauseTest, add_at_least_two2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_at_least_two(lit1, lit2);

  check_at_least(2, 2);
}

TEST_P(AddClauseTest, add_at_least_two3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_at_least_two(lit1, lit2, lit3);

  check_at_least(3, 2);
}

TEST_P(AddClauseTest, add_at_least_two4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_at_least_two(lit1, lit2, lit3, lit4);

  check_at_least(4, 2);
}

TEST_P(AddClauseTest, add_at_least_two5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_at_least_two(lit1, lit2, lit3, lit4, lit5);

  check_at_least(5, 2);
}

TEST_P(AddClauseTest, add_at_least_two6)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);

  mSolver.add_at_least_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_least(6, 2);
}

TEST_P(AddClauseTest, add_at_least_twoN)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);
  SatLiteral lit7(mVarList[6]);

  vector<SatLiteral> lits(7);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;
  lits[5] = lit6;
  lits[6] = lit7;

  mSolver.add_at_least_two(lits);

  check_at_least(7, 2);
}

TEST_P(AddClauseTest, add_at_least_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = SatLiteral(mVarList[i]);
  }

  mSolver.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(AddClauseTest, add_at_least_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = SatLiteral(mVarList[i]);
  }

  mSolver.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(AddClauseTest, add_exact_one2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_exact_one(lit1, lit2);

  check_exact(2, 1);
}

TEST_P(AddClauseTest, add_exact_one3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_exact_one(lit1, lit2, lit3);

  check_exact(3, 1);
}

TEST_P(AddClauseTest, add_exact_one4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_exact_one(lit1, lit2, lit3, lit4);

  check_exact(4, 1);
}

TEST_P(AddClauseTest, add_exact_one5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_exact_one(lit1, lit2, lit3, lit4, lit5);

  check_exact(5, 1);
}

TEST_P(AddClauseTest, add_exact_one6)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);

  mSolver.add_exact_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_exact(6, 1);
}

TEST_P(AddClauseTest, add_exact_oneN)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);
  SatLiteral lit7(mVarList[6]);

  vector<SatLiteral> lits(7);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;
  lits[5] = lit6;
  lits[6] = lit7;

  mSolver.add_exact_one(lits);

  check_exact(7, 1);
}

// add_exact_two2 は自明なのでやらない．

TEST_P(AddClauseTest, add_exact_two3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_exact_two(lit1, lit2, lit3);

  check_exact(3, 2);
}

TEST_P(AddClauseTest, add_exact_two4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_exact_two(lit1, lit2, lit3, lit4);

  check_exact(4, 2);
}

TEST_P(AddClauseTest, add_exact_two5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_exact_two(lit1, lit2, lit3, lit4, lit5);

  check_exact(5, 2);
}

TEST_P(AddClauseTest, add_exact_two6)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);

  mSolver.add_exact_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_exact(6, 2);
}

TEST_P(AddClauseTest, add_exact_twoN)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);
  SatLiteral lit7(mVarList[6]);

  vector<SatLiteral> lits(7);
  lits[0] = lit1;
  lits[1] = lit2;
  lits[2] = lit3;
  lits[3] = lit4;
  lits[4] = lit5;
  lits[5] = lit6;
  lits[6] = lit7;

  mSolver.add_exact_two(lits);

  check_exact(7, 2);
}

TEST_P(AddClauseTest, add_exact_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = SatLiteral(mVarList[i]);
  }

  mSolver.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(AddClauseTest, add_exact_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = SatLiteral(mVarList[i]);
  }

  mSolver.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(AddClauseTest, add_not_one2)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_not_one(lit1, lit2);

  check_not_one(2);
}

TEST_P(AddClauseTest, add_not_one3)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_not_one(lit1, lit2, lit3);

  check_not_one(3);
}

TEST_P(AddClauseTest, add_not_one4)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_not_one(lit1, lit2, lit3, lit4);

  check_not_one(4);
}

TEST_P(AddClauseTest, add_not_one5)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_not_one(lit1, lit2, lit3, lit4, lit5);

  check_not_one(5);
}

TEST_P(AddClauseTest, add_not_one6)
{
  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);
  SatLiteral lit6(mVarList[5]);

  mSolver.add_not_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_not_one(6);
}

TEST_P(AddClauseTest, add_not_oneN)
{
  int n = 10;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = SatLiteral(mVarList[i]);
  }

  mSolver.add_not_one(lits);

  check_not_one(n);
}


TEST_P(AddClauseTest, add_clause_with_cond1_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);

  mSolver.add_clause(lit1);

  vector<int> vals(
    {
     // lit1 ans
     //   0    0
     //   1    1
     0,
     1
    }
  );

  check_with_cond1(1, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(AddClauseTest, add_clause_with_cond1_2)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);

  mSolver.add_clause(~lit1);

  vector<int> vals(
    {
     // lit1 ans
     //   0    1
     //   1    0
     1,
     0
    }
  );

  check_with_cond1(1, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(AddClauseTest, add_clause_with_cond2_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_clause(lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    0
     //   0    1    1
     //   1    0    1
     //   1    1    1
     0,
     1,
     1,
     1
    }
  );

  check_with_cond1(2, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(AddClauseTest, add_clause_with_cond2_2)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);

  mSolver.add_clause(~lit1, lit2);

  vector<int> vals(
    {
     // lit2 lit1 ans
     //   0    0    1
     //   0    1    0
     //   1    0    1
     //   1    1    1
     1,
     0,
     1,
     1
    }
  );

  check_with_cond1(2, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(AddClauseTest, add_clause_with_cond3_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_clause(lit1, lit2, lit3);

  vector<int> vals(
    {
     // lit3 lit2 lit1 ans
     //   0    0    0    0
     //   0    0    1    1
     //   0    1    0    1
     //   0    1    1    1
     //   1    0    0    1
     //   1    0    1    1
     //   1    1    0    1
     //   1    1    1    1
     0,
     1,
     1,
     1,
     1,
     1,
     1,
     1
    }
  );

  check_with_cond1(3, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(AddClauseTest, add_clause_with_cond3_2)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);

  mSolver.add_clause(~lit1, lit2, ~lit3);

  vector<int> vals(
    {
     // lit3 lit2 lit1 ans
     //   0    0    0    1
     //   0    0    1    1
     //   0    1    0    1
     //   0    1    1    1
     //   1    0    0    1
     //   1    0    1    0
     //   1    1    0    1
     //   1    1    1    1
     1,
     1,
     1,
     1,
     1,
     0,
     1,
     1
    }
  );

  check_with_cond1(3, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(AddClauseTest, add_clause_with_cond4_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);

  mSolver.add_clause(lit1, lit2, lit3, lit4);

  vector<int> vals(
    {
     // lit4 lit3 lit2 lit1 ans
     //   0    0    0    0    0
     //   0    0    0    1    1
     //   0    0    1    0    1
     //   0    0    1    1    1
     //   0    1    0    0    1
     //   0    1    0    1    1
     //   0    1    1    0    1
     //   0    1    1    1    1
     //   1    0    0    0    1
     //   1    0    0    1    1
     //   1    0    1    0    1
     //   1    0    1    1    1
     //   1    1    0    0    1
     //   1    1    0    1    1
     //   1    1    1    0    1
     //   1    1    1    1    1
     0,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1,
     1
    }
  );

  check_with_cond1(4, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(AddClauseTest, add_clause_with_cond5_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  mSolver.add_clause(lit1, lit2, lit3, lit4, lit5);

  vector<int> vals(
    {
     // lit5 lit4 lit3 lit2 lit1  ans
     //    0    0    0    0    0    0
     //    0    0    0    0    1    1
     //    0    0    0    1    0    1
     //    0    0    0    1    1    1
     //    0    0    1    0    0    1
     //    0    0    1    0    1    1
     //    0    0    1    1    0    1
     //    0    0    1    1    1    1
     //    0    1    0    0    0    1
     //    0    1    0    0    1    1
     //    0    1    0    1    0    1
     //    0    1    0    1    1    1
     //    0    1    1    0    0    1
     //    0    1    1    0    1    1
     //    0    1    1    1    0    1
     //    0    1    1    1    1    1
     //    1    0    0    0    0    1
     //    1    0    0    0    1    1
     //    1    0    0    1    0    1
     //    1    0    0    1    1    1
     //    1    0    1    0    0    1
     //    1    0    1    0    1    1
     //    1    0    1    1    0    1
     //    1    0    1    1    1    1
     //    1    1    0    0    0    1
     //    1    1    0    0    1    1
     //    1    1    0    1    0    1
     //    1    1    0    1    1    1
     //    1    1    1    0    0    1
     //    1    1    1    0    1    1
     //    1    1    1    1    0    1
     //    1    1    1    1    1    1
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
  );

  check_with_cond1(5, vals);

  mSolver.clear_conditional_literals();
}

TEST_P(AddClauseTest, add_clause_with_cond5n_1)
{
  SatLiteral clit1(mCondVarList[0]);
  mSolver.set_conditional_literals(clit1);

  SatLiteral lit1(mVarList[0]);
  SatLiteral lit2(mVarList[1]);
  SatLiteral lit3(mVarList[2]);
  SatLiteral lit4(mVarList[3]);
  SatLiteral lit5(mVarList[4]);

  vector<SatLiteral> tmp_lits(5);
  tmp_lits[0] = lit1;
  tmp_lits[1] = lit2;
  tmp_lits[2] = lit3;
  tmp_lits[3] = lit4;
  tmp_lits[4] = lit5;
  mSolver.add_clause(tmp_lits);

  vector<int> vals(
    {
     // lit5 lit4 lit3 lit2 lit1  ans
     //    0    0    0    0    0    0
     //    0    0    0    0    1    1
     //    0    0    0    1    0    1
     //    0    0    0    1    1    1
     //    0    0    1    0    0    1
     //    0    0    1    0    1    1
     //    0    0    1    1    0    1
     //    0    0    1    1    1    1
     //    0    1    0    0    0    1
     //    0    1    0    0    1    1
     //    0    1    0    1    0    1
     //    0    1    0    1    1    1
     //    0    1    1    0    0    1
     //    0    1    1    0    1    1
     //    0    1    1    1    0    1
     //    0    1    1    1    1    1
     //    1    0    0    0    0    1
     //    1    0    0    0    1    1
     //    1    0    0    1    0    1
     //    1    0    0    1    1    1
     //    1    0    1    0    0    1
     //    1    0    1    0    1    1
     //    1    0    1    1    0    1
     //    1    0    1    1    1    1
     //    1    1    0    0    0    1
     //    1    1    0    0    1    1
     //    1    1    0    1    0    1
     //    1    1    0    1    1    1
     //    1    1    1    0    0    1
     //    1    1    1    0    1    1
     //    1    1    1    1    0    1
     //    1    1    1    1    1    1
     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    }
  );

  check_with_cond1(5, vals);

  mSolver.clear_conditional_literals();
}

INSTANTIATE_TEST_CASE_P(SatSolverTest,
			AddClauseTest,
			::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					  "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
