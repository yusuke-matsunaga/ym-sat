
/// @file at_most_test.cc
/// @brief SatSolver::at_most_XXX() のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2017, 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "SatTestFixture.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class AtMostTest :
  public SatTestFixture
{
public:

  /// @brief コンストラクタ
  AtMostTest() : SatTestFixture() {}

  /// @brief デストラクタ
  ~AtMostTest() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief at_most_k のチェックを行う．
  void
  check_at_most(
    int n,
    int k
  )
  {
    check_common(n, [=](int c) { return c <= k; });
  }

  /// @brief at_least_k のチェックを行う．
  void
  check_at_least(
    int n,
    int k
  )
  {
    check_common(n, [=](int c) { return c >= k; });
  }

  /// @brief exact_k のチェックを行う．
  void
  check_exact(
    int n,
    int k
  )
  {
    check_common(n, [=](int c) { return c == k; });
  }

  /// @brief not_one のチェックを行う．
  void
  check_not_one(
    int n
  )
  {
    check_common(n, [=](int c) { return c != 1; });
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 下請け関数
  //////////////////////////////////////////////////////////////////////

  void
  check_common(
    int n,
    std::function<bool(int)> eval
  );

};

// @brief at_most_k のチェックを行う．
void
AtMostTest::check_common(
  int n,
  std::function<bool(int)> eval
)
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
    SatBool3 exp_ans = eval(c) ? SatBool3::True : SatBool3::False;
    SatBool3 stat = mSolver.solve(assumptions);
    if ( exp_ans != stat ) {
      cout << "c = " << c
	   << ", bpat = " << bpat
	   << ", exp_ans = " << exp_ans << endl;
    }
    EXPECT_EQ( exp_ans, stat );
  }
}

TEST_P(AtMostTest, add_at_most_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mSolver.add_at_most_one(lit1, lit2);

  check_at_most(2, 1);
}

TEST_P(AtMostTest, add_at_most_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_at_most_one(lit1, lit2, lit3);

  check_at_most(3, 1);
}

TEST_P(AtMostTest, add_at_most_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4);

  check_at_most(4, 1);
}

TEST_P(AtMostTest, add_at_most_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4, lit5);

  check_at_most(5, 1);
}

TEST_P(AtMostTest, add_at_most_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_at_most_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_most(6, 1);
}

TEST_P(AtMostTest, add_at_most_oneN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_one(lits);

  check_at_most(n, 1);
}

// add_at_most_two2 は自明なのでやらない．

TEST_P(AtMostTest, add_at_most_two3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_at_most_two(lit1, lit2, lit3);

  check_at_most(3, 2);
}

TEST_P(AtMostTest, add_at_most_two4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4);

  check_at_most(4, 2);
}

TEST_P(AtMostTest, add_at_most_two5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4, lit5);

  check_at_most(5, 2);
}

TEST_P(AtMostTest, add_at_most_two6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_at_most_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_most(6, 2);
}

TEST_P(AtMostTest, add_at_most_twoN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_two(lits);

  check_at_most(n, 2);
}

TEST_P(AtMostTest, add_at_most_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(AtMostTest, add_at_most_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(AtMostTest, add_at_most_15_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(AtMostTest, add_at_most_15_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(AtMostTest, add_at_most_15_10)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_most_k(lits, k);

  check_at_most(n, k);
}

TEST_P(AtMostTest, add_at_least_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mSolver.add_at_least_one(lit1, lit2);

  check_at_least(2, 1);
}

TEST_P(AtMostTest, add_at_least_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_at_least_one(lit1, lit2, lit3);

  check_at_least(3, 1);
}

TEST_P(AtMostTest, add_at_least_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_at_least_one(lit1, lit2, lit3, lit4);

  check_at_least(4, 1);
}

TEST_P(AtMostTest, add_at_least_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_at_least_one(lit1, lit2, lit3, lit4, lit5);

  check_at_least(5, 1);
}

TEST_P(AtMostTest, add_at_least_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_at_least_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_least(6, 1);
}

TEST_P(AtMostTest, add_at_least_oneN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_least_one(lits);

  check_at_least(n, 1);
}

// add_at_least_two2 は自明なのでやらない．

TEST_P(AtMostTest, add_at_least_two3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_at_least_two(lit1, lit2, lit3);

  check_at_least(3, 2);
}

TEST_P(AtMostTest, add_at_least_two4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_at_least_two(lit1, lit2, lit3, lit4);

  check_at_least(4, 2);
}

TEST_P(AtMostTest, add_at_least_two5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_at_least_two(lit1, lit2, lit3, lit4, lit5);

  check_at_least(5, 2);
}

TEST_P(AtMostTest, add_at_least_two6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_at_least_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_at_least(6, 2);
}

TEST_P(AtMostTest, add_at_least_twoN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_least_two(lits);

  check_at_least(n, 2);
}

TEST_P(AtMostTest, add_at_least_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(AtMostTest, add_at_least_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(AtMostTest, add_at_least_15_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(AtMostTest, add_at_least_15_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(AtMostTest, add_at_least_15_10)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_at_least_k(lits, k);

  check_at_least(n, k);
}

TEST_P(AtMostTest, add_exact_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mSolver.add_exact_one(lit1, lit2);

  check_exact(2, 1);
}

TEST_P(AtMostTest, add_exact_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_exact_one(lit1, lit2, lit3);

  check_exact(3, 1);
}

TEST_P(AtMostTest, add_exact_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_exact_one(lit1, lit2, lit3, lit4);

  check_exact(4, 1);
}

TEST_P(AtMostTest, add_exact_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_exact_one(lit1, lit2, lit3, lit4, lit5);

  check_exact(5, 1);
}

TEST_P(AtMostTest, add_exact_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_exact_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_exact(6, 1);
}

TEST_P(AtMostTest, add_exact_oneN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_one(lits);

  check_exact(7, 1);
}

// add_exact_two2 は自明なのでやらない．

TEST_P(AtMostTest, add_exact_two3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_exact_two(lit1, lit2, lit3);

  check_exact(3, 2);
}

TEST_P(AtMostTest, add_exact_two4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_exact_two(lit1, lit2, lit3, lit4);

  check_exact(4, 2);
}

TEST_P(AtMostTest, add_exact_two5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_exact_two(lit1, lit2, lit3, lit4, lit5);

  check_exact(5, 2);
}

TEST_P(AtMostTest, add_exact_two6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_exact_two(lit1, lit2, lit3, lit4, lit5, lit6);

  check_exact(6, 2);
}

TEST_P(AtMostTest, add_exact_twoN)
{
  int n = 7;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_two(lits);

  check_exact(7, 2);
}

TEST_P(AtMostTest, add_exact_10_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(AtMostTest, add_exact_10_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(AtMostTest, add_exact_15_3)
{
  int n = 10;
  int k = 3;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(AtMostTest, add_exact_15_5)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(AtMostTest, add_exact_15_10)
{
  int n = 10;
  int k = 5;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_exact_k(lits, k);

  check_exact(n, k);
}

TEST_P(AtMostTest, add_not_one2)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};

  mSolver.add_not_one(lit1, lit2);

  check_not_one(2);
}

TEST_P(AtMostTest, add_not_one3)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};

  mSolver.add_not_one(lit1, lit2, lit3);

  check_not_one(3);
}

TEST_P(AtMostTest, add_not_one4)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};

  mSolver.add_not_one(lit1, lit2, lit3, lit4);

  check_not_one(4);
}

TEST_P(AtMostTest, add_not_one5)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};

  mSolver.add_not_one(lit1, lit2, lit3, lit4, lit5);

  check_not_one(5);
}

TEST_P(AtMostTest, add_not_one6)
{
  auto lit1{mVarList[0]};
  auto lit2{mVarList[1]};
  auto lit3{mVarList[2]};
  auto lit4{mVarList[3]};
  auto lit5{mVarList[4]};
  auto lit6{mVarList[5]};

  mSolver.add_not_one(lit1, lit2, lit3, lit4, lit5, lit6);

  check_not_one(6);
}

TEST_P(AtMostTest, add_not_oneN)
{
  int n = 10;
  vector<SatLiteral> lits(n);
  for ( int i: Range(n) ) {
    lits[i] = mVarList[i];
  }

  mSolver.add_not_one(lits);

  check_not_one(n);
}

INSTANTIATE_TEST_SUITE_P(SatSolverTest,
			 AtMostTest,
			 ::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					   "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
