
/// @file SatCountEncTest.cc
/// @brief SatCountEncTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatSolver.h"
#include "ym/SatCountEnc.h"
#include "ym/SatModel.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class SatCountEncTest :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief コンストラクタ
  SatCountEncTest();

  /// @brief デストラクタ
  ~SatCountEncTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 設定されたCNFが vals[] で示された真理値表と等しいか調べる．
  void
  check(int ni,
	const vector<int>& vals);

  /// @brief at_most_k のチェックを行う．
  void
  check_at_most(int n,
		int k);

  /// @brief at_most_k のチェックを行う．
  void
  check_at_most_sub(int n,
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

  // count encoder
  SatCountEnc mEnc;

  // 変数の数
  int mVarNum;

  // 変数のリスト
  vector<SatLiteral> mVarList;

};

SatCountEncTest::SatCountEncTest() :
  mSolver(GetParam()),
  mEnc(mSolver),
  mVarNum(100),
  mVarList(mVarNum)
{
  for ( int i: Range(mVarNum) ) {
    mVarList[i] = mSolver.new_variable();
  }
}

SatCountEncTest::~SatCountEncTest()
{
}

// @brief 設定されたCNFが vals[] で示された真理値表と等しいか調べる．
void
SatCountEncTest::check(int ni,
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

// @brief at_most_k のチェックを行う．
void
SatCountEncTest::check_at_most(int n,
			       int k)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    string bpat{};
    for ( int i: Range(n) ) {
      auto lit{mVarList[i]};
      if ( p & (1U << i) ) {
	++ c;
	bpat += "1";
      }
      else {
	lit = ~lit;
	bpat += "0";
      }
      assumptions.push_back(lit);
    }
    SatBool3 exp_ans = (c <= k) ? SatBool3::True : SatBool3::False;
    SatModel model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    if ( exp_ans != stat ) {
      cout << "c = " << c
	   << ", bpat = " << bpat
	   << ", exp_ans = " << exp_ans << endl;
    }
    EXPECT_EQ( exp_ans, stat );
  }
}

// @brief at_least_k のチェックを行う．
void
SatCountEncTest::check_at_least(int n,
				int k)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    for ( int i: Range(n) ) {
      auto lit{mVarList[i]};
      if ( p & (1U << i) ) {
	++ c;
      }
      else {
	lit = ~lit;
      }
      assumptions.push_back(lit);
    }
    SatBool3 exp_ans = (c >= k) ? SatBool3::True : SatBool3::False;
    SatModel model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    EXPECT_EQ( exp_ans, stat );
  }
}

// @brief exact_k のチェックを行う．
void
SatCountEncTest::check_exact(int n,
			     int k)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    for ( int i: Range(n) ) {
      auto lit{mVarList[i]};
      if ( p & (1U << i) ) {
	++ c;
      }
      else {
	lit = ~lit;
      }
      assumptions.push_back(lit);
    }
    SatBool3 exp_ans = (c == k) ? SatBool3::True : SatBool3::False;
    SatModel model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    EXPECT_EQ( exp_ans, stat );
  }
}

// @brief not_one のチェックを行う．
void
SatCountEncTest::check_not_one(int n)
{
  int np = 1U << n;
  for ( int p: Range(np) ) {
    vector<SatLiteral> assumptions;
    int c = 0;
    for ( int i: Range(n) ) {
      auto lit{mVarList[i]};
      if ( p & (1U << i) ) {
	++ c;
      }
      else {
	lit = ~lit;
      }
      assumptions.push_back(lit);
    }
    SatBool3 exp_ans = (c != 1) ? SatBool3::True : SatBool3::False;
    SatModel model;
    SatBool3 stat = mSolver.solve(assumptions, model);
    EXPECT_EQ( exp_ans, stat );
  }
}

// @brief 論理ゲートの真理値表からチェック用のベクタを作る．
void
SatCountEncTest::make_vals(int ni,
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

TEST_P(SatCountEncTest, add_at_most_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mEnc.add_at_most_one(lit1, lit2);

  check_at_most(2, 1);
}

TEST_P(SatCountEncTest, add_at_most_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mEnc.add_at_most_one(lit1, lit2, lit3);

  check_at_most(3, 1);
}

TEST_P(SatCountEncTest, add_at_most_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mEnc.add_at_most_one(lit1, lit2, lit3, lit4);

  check_at_most(4, 1);
}

TEST_P(SatCountEncTest, add_at_most_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mEnc.add_at_most_one(lit1, lit2, lit3, lit4, lit5);

  check_at_most(5, 1);
}

TEST_P(SatCountEncTest, add_at_most_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mEnc.add_at_most_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_most(6, 1);
}

TEST_P(SatCountEncTest, add_at_most_oneN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_at_most_one(lits);

  check_at_most(n, 1);
}

// add_at_most_two2 は自明なのでやらない．

TEST_P(SatCountEncTest, add_at_most_two3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mEnc.add_at_most_two(lit1, lit2, lit3);

  check_at_most(3, 2);
}

TEST_P(SatCountEncTest, add_at_most_two4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mEnc.add_at_most_two(lit1, lit2, lit3, lit4);

  check_at_most(4, 2);
}

TEST_P(SatCountEncTest, add_at_most_two5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mEnc.add_at_most_two(lit1, lit2, lit3, lit4, lit5);

  check_at_most(5, 2);
}

TEST_P(SatCountEncTest, add_at_most_two6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mEnc.add_at_most_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_most(6, 2);
}

TEST_P(SatCountEncTest, add_at_most_twoN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_at_most_two(lits);

  check_at_most(n, 2);
}

TEST_P(SatCountEncTest, add_at_most_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(SatCountEncTest, add_at_most_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(SatCountEncTest, add_at_least_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mEnc.add_at_least_one(lit1, lit2);

  check_at_least(2, 1);
}

TEST_P(SatCountEncTest, add_at_least_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mEnc.add_at_least_one(lit1, lit2, lit3);

  check_at_least(3, 1);
}

TEST_P(SatCountEncTest, add_at_least_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mEnc.add_at_least_one(lit1, lit2, lit3, lit4);

  check_at_least(4, 1);
}

TEST_P(SatCountEncTest, add_at_least_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mEnc.add_at_least_one(lit1, lit2, lit3, lit4, lit5);

  check_at_least(5, 1);
}

TEST_P(SatCountEncTest, add_at_least_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mEnc.add_at_least_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_least(6, 1);
}

TEST_P(SatCountEncTest, add_at_least_oneN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_at_least_one(lits);

  check_at_least(7, 1);
}

TEST_P(SatCountEncTest, add_at_least_two2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mEnc.add_at_least_two(lit1, lit2);

  check_at_least(2, 2);
}

TEST_P(SatCountEncTest, add_at_least_two3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mEnc.add_at_least_two(lit1, lit2, lit3);

  check_at_least(3, 2);
}

TEST_P(SatCountEncTest, add_at_least_two4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mEnc.add_at_least_two(lit1, lit2, lit3, lit4);

  check_at_least(4, 2);
}

TEST_P(SatCountEncTest, add_at_least_two5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mEnc.add_at_least_two(lit1, lit2, lit3, lit4, lit5);

  check_at_least(5, 2);
}

TEST_P(SatCountEncTest, add_at_least_two6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mEnc.add_at_least_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_least(6, 2);
}

TEST_P(SatCountEncTest, add_at_least_twoN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_at_least_two(lits);

  check_at_least(7, 2);
}

TEST_P(SatCountEncTest, add_at_least_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(SatCountEncTest, add_at_least_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(SatCountEncTest, add_exact_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mEnc.add_exact_one(lit1, lit2);

  check_exact(2, 1);
}

TEST_P(SatCountEncTest, add_exact_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mEnc.add_exact_one(lit1, lit2, lit3);

  check_exact(3, 1);
}

TEST_P(SatCountEncTest, add_exact_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mEnc.add_exact_one(lit1, lit2, lit3, lit4);

  check_exact(4, 1);
}

TEST_P(SatCountEncTest, add_exact_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mEnc.add_exact_one(lit1, lit2, lit3, lit4, lit5);

  check_exact(5, 1);
}

TEST_P(SatCountEncTest, add_exact_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mEnc.add_exact_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_exact(6, 1);
}

TEST_P(SatCountEncTest, add_exact_oneN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_exact_one(lits);

  check_exact(7, 1);
}

// add_exact_two2 は自明なのでやらない．

TEST_P(SatCountEncTest, add_exact_two3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mEnc.add_exact_two(lit1, lit2, lit3);

  check_exact(3, 2);
}

TEST_P(SatCountEncTest, add_exact_two4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mEnc.add_exact_two(lit1, lit2, lit3, lit4);

  check_exact(4, 2);
}

TEST_P(SatCountEncTest, add_exact_two5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mEnc.add_exact_two(lit1, lit2, lit3, lit4, lit5);

  check_exact(5, 2);
}

TEST_P(SatCountEncTest, add_exact_two6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mEnc.add_exact_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_exact(6, 2);
}

TEST_P(SatCountEncTest, add_exact_twoN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_exact_two(lits);

  check_exact(7, 2);
}

TEST_P(SatCountEncTest, add_exact_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(SatCountEncTest, add_exact_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(SatCountEncTest, add_not_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mEnc.add_not_one(lit1, lit2);

  check_not_one(2);
}

TEST_P(SatCountEncTest, add_not_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mEnc.add_not_one(lit1, lit2, lit3);

  check_not_one(3);
}

TEST_P(SatCountEncTest, add_not_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mEnc.add_not_one(lit1, lit2, lit3, lit4);

  check_not_one(4);
}

TEST_P(SatCountEncTest, add_not_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mEnc.add_not_one(lit1, lit2, lit3, lit4, lit5);

  check_not_one(5);
}

TEST_P(SatCountEncTest, add_not_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mEnc.add_not_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_not_one(6);
}

TEST_P(SatCountEncTest, add_not_oneN)
{
  int n = 10;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mEnc.add_not_one(lits);

  check_not_one(n);
}

INSTANTIATE_TEST_CASE_P(SatSolverTest,
			SatCountEncTest,
			::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					  "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
