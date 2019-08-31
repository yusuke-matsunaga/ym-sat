
/// @file SatBvEncTest.cc
/// @brief SatBvEncTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatSolver.h"
#include "ym/SatBvEnc.h"
#include "ym/SatModel.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

class SatBvEncTest :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief コンストラクタ
  SatBvEncTest();

  /// @デストラクタ
  ~SatBvEncTest();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief add_eq(vector, vector) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_size b_vec のサイズ
  void
  check_add_eq1(int a_size,
		int b_size);

  /// @brief add_eq(vector, val) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_val bの値
  void
  check_add_eq2(int a_size,
		int b_val);

  /// @brief add_ne(vector, vector) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_size b_vec のサイズ
  void
  check_add_ne1(int a_size,
		int b_size);

  /// @brief add_ne(vector, val) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_val bの値
  void
  check_add_ne2(int a_size,
		int b_val);

  /// @brief add_lt(vector, vector) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_size b_vec のサイズ
  void
  check_add_lt1(int a_size,
		int b_size);

  /// @brief add_lt(vector, val) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_val bの値
  void
  check_add_lt2(int a_size,
		int b_val);

  /// @brief add_le(vector, vector) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_size b_vec のサイズ
  void
  check_add_le1(int a_size,
		int b_size);

  /// @brief add_le(vector, val) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_val bの値
  void
  check_add_le2(int a_size,
		int b_val);

  /// @brief add_gt(vector, vector) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_size b_vec のサイズ
  void
  check_add_gt1(int a_size,
		int b_size);

  /// @brief add_gt(vector, val) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_val bの値
  void
  check_add_gt2(int a_size,
		int b_val);

  /// @brief add_ge(vector, vector) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_size b_vec のサイズ
  void
  check_add_ge1(int a_size,
		int b_size);

  /// @brief add_ge(vector, val) のチェック
  /// @param[in] a_size a_vec のサイズ
  /// @param[in] b_val bの値
  void
  check_add_ge2(int a_size,
		int b_val);


public:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

};

// @brief コンストラクタ
SatBvEncTest::SatBvEncTest() :
  mSolver(GetParam())
{
  //cout << "Running on SatSolver(" << GetParam() << ")" << endl;
}

// @デストラクタ
SatBvEncTest::~SatBvEncTest()
{
}

// @brief add_eq(vector, vector) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_size b_vec のサイズ
void
SatBvEncTest::check_add_eq1(int a_size,
			    int b_size)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  vector<SatLiteral> b_vec(b_size);
  for ( int i: Range(b_size) ) {
    b_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_eq(a_vec, b_vec);

  int exp_a = 1 << a_size;
  int exp_b = 1 << b_size;
  int nall = exp_a * exp_b;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    int b_val = 0;
    for ( int i: Range(b_size) ) {
      auto lit{b_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	b_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val == b_val );

    mark[a_val * exp_b + b_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    for ( int b_val: Range(exp_b) ) {
      int idx = a_val * exp_b + b_val;
      if ( a_val == b_val ) {
	EXPECT_TRUE( mark[idx] );
      }
      else {
	EXPECT_FALSE( mark[idx] );
      }
    }
  }
}

// @brief add_eq(vector, val) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_val bの値
void
SatBvEncTest::check_add_eq2(int a_size,
			    int b_val)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_eq(a_vec, b_val);

  int exp_a = 1 << a_size;
  int nall = exp_a;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back( lit);
      }
    }

    EXPECT_TRUE( a_val == b_val );

    mark[a_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    int idx = a_val;
    if ( a_val == b_val ) {
      EXPECT_TRUE( mark[idx] );
    }
    else {
      EXPECT_FALSE( mark[idx] );
    }
  }
}

// @brief add_ne(vector, vector) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_size b_vec のサイズ
void
SatBvEncTest::check_add_ne1(int a_size,
			    int b_size)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  vector<SatLiteral> b_vec(b_size);
  for ( int i: Range(b_size) ) {
    b_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_ne(a_vec, b_vec);

  int exp_a = 1 << a_size;
  int exp_b = 1 << b_size;
  int nall = exp_a * exp_b;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    int b_val = 0;
    for ( int i: Range(b_size) ) {
      auto lit{b_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	b_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val != b_val );

    mark[a_val * exp_b + b_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    for ( int b_val: Range(exp_b) ) {
      int idx = a_val * exp_b + b_val;
      if ( a_val != b_val ) {
	EXPECT_TRUE( mark[idx] );
      }
      else {
	EXPECT_FALSE( mark[idx] );
      }
    }
  }
}

// @brief add_ne(vector, val) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_val bの値
void
SatBvEncTest::check_add_ne2(int a_size,
			    int b_val)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_ne(a_vec, b_val);

  int exp_a = 1 << a_size;
  int nall = exp_a;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    if ( a_val == b_val ) {
      cout << "a_val = " << a_val << ", b_val = " << b_val << endl;
    }
    EXPECT_TRUE( a_val != b_val );

    mark[a_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    int idx = a_val;
    if ( a_val != b_val ) {
      EXPECT_TRUE( mark[idx] );
    }
    else {
      EXPECT_FALSE( mark[idx] );
    }
  }
}

// @brief add_lt(vector, vector) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_size b_vec のサイズ
void
SatBvEncTest::check_add_lt1(int a_size,
			    int b_size)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  vector<SatLiteral> b_vec(b_size);
  for ( int i: Range(b_size) ) {
    b_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_lt(a_vec, b_vec);

  int exp_a = 1 << a_size;
  int exp_b = 1 << b_size;
  int nall = exp_a * exp_b;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    int b_val = 0;
    for ( int i: Range(b_size) ) {
      auto lit{b_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	b_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val < b_val );

    mark[a_val * exp_b + b_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    for ( int b_val: Range(exp_b) ) {
      int idx = a_val * exp_b + b_val;
      if ( a_val < b_val ) {
	if ( !mark[idx] ) {
	  cout << "a_val = " << a_val << ", b_val = " << b_val << endl;
	}
	EXPECT_TRUE( mark[idx] );
      }
      else {
	if ( mark[idx] ) {
	  cout << "a_val = " << a_val << ", b_val = " << b_val << endl;
	}
	EXPECT_FALSE( mark[idx] );
      }
    }
  }
}

// @brief add_lt(vector, val) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_val bの値
void
SatBvEncTest::check_add_lt2(int a_size,
			    int b_val)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_lt(a_vec, b_val);

  int exp_a = 1 << a_size;
  int nall = exp_a;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val < b_val );

    mark[a_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    int idx = a_val;
    if ( a_val < b_val ) {
      EXPECT_TRUE( mark[idx] );
    }
    else {
      EXPECT_FALSE( mark[idx] );
    }
  }
}

// @brief add_le(vector, vector) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_size b_vec のサイズ
void
SatBvEncTest::check_add_le1(int a_size,
			    int b_size)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  vector<SatLiteral> b_vec(b_size);
  for ( int i: Range(b_size) ) {
    b_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_eq(a_vec, b_vec);

  int exp_a = 1 << a_size;
  int exp_b = 1 << b_size;
  int nall = exp_a * exp_b;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    int b_val = 0;
    for ( int i: Range(b_size) ) {
      auto lit{b_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	b_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val == b_val );

    mark[a_val * exp_b + b_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    for ( int b_val: Range(exp_b) ) {
      int idx = a_val * exp_b + b_val;
      if ( a_val == b_val ) {
	EXPECT_TRUE( mark[idx] );
      }
      else {
	EXPECT_FALSE( mark[idx] );
      }
    }
  }
}

// @brief add_le(vector, val) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_val bの値
void
SatBvEncTest::check_add_le2(int a_size,
			    int b_val)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_le(a_vec, b_val);

  int exp_a = 1 << a_size;
  int nall = exp_a;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val <= b_val );

    mark[a_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    int idx = a_val;
    if ( a_val <= b_val ) {
      EXPECT_TRUE( mark[idx] );
    }
    else {
      EXPECT_FALSE( mark[idx] );
    }
  }
}

// @brief add_gt(vector, vector) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_size b_vec のサイズ
void
SatBvEncTest::check_add_gt1(int a_size,
			    int b_size)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  vector<SatLiteral> b_vec(b_size);
  for ( int i: Range(b_size) ) {
    b_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_gt(a_vec, b_vec);

  int exp_a = 1 << a_size;
  int exp_b = 1 << b_size;
  int nall = exp_a * exp_b;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    int b_val = 0;
    for ( int i: Range(b_size) ) {
      auto lit{b_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	b_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val > b_val );

    mark[a_val * exp_b + b_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    for ( int b_val: Range(exp_b) ) {
      int idx = a_val * exp_b + b_val;
      if ( a_val > b_val ) {
	if ( !mark[idx] ) {
	  cout << "a_val = " << a_val << ", b_val = " << b_val << endl;
	}
	EXPECT_TRUE( mark[idx] );
      }
      else {
	if ( mark[idx] ) {
	  cout << "a_val = " << a_val << ", b_val = " << b_val << endl;
	}
	EXPECT_FALSE( mark[idx] );
      }
    }
  }
}

// @brief add_gt(vector, val のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_val bの値
void
SatBvEncTest::check_add_gt2(int a_size,
			    int b_val)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_gt(a_vec, b_val);

  int exp_a = 1 << a_size;
  int nall = exp_a;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val > b_val );

    mark[a_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    int idx = a_val;
    if ( a_val > b_val ) {
      EXPECT_TRUE( mark[idx] );
    }
    else {
      EXPECT_FALSE( mark[idx] );
    }
  }
}

// @brief add_ge(vector, vector) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_size b_vec のサイズ
void
SatBvEncTest::check_add_ge1(int a_size,
			    int b_size)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  vector<SatLiteral> b_vec(b_size);
  for ( int i: Range(b_size) ) {
    b_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_ge(a_vec, b_vec);

  int exp_a = 1 << a_size;
  int exp_b = 1 << b_size;
  int nall = exp_a * exp_b;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    int b_val = 0;
    for ( int i: Range(b_size) ) {
      auto lit{b_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	b_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val >= b_val );

    mark[a_val * exp_b + b_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    for ( int b_val: Range(exp_b) ) {
      int idx = a_val * exp_b + b_val;
      if ( a_val >= b_val ) {
	if ( !mark[idx] ) {
	  cout << "a_val = " << a_val << ", b_val = " << b_val << endl;
	}
	EXPECT_TRUE( mark[idx] );
      }
      else {
	if ( mark[idx] ) {
	  cout << "a_val = " << a_val << ", b_val = " << b_val << endl;
	}
	EXPECT_FALSE( mark[idx] );
      }
    }
  }
}

// @brief add_ge(vector, val) のチェック
// @param[in] a_size a_vec のサイズ
// @param[in] b_val bの値
void
SatBvEncTest::check_add_ge2(int a_size,
			    int b_val)
{
  vector<SatLiteral> a_vec(a_size);
  for ( int i: Range(a_size) ) {
    a_vec[i] = mSolver.new_variable();
  }

  SatBvEnc bvenc(mSolver);
  bvenc.add_ge(a_vec, b_val);

  int exp_a = 1 << a_size;
  int nall = exp_a;
  vector<bool> mark(nall, false);
  while ( true ) {
    SatModel model;
    SatBool3 stat = mSolver.solve(model);
    if ( stat == SatBool3::False ) {
      break;
    }

    vector<SatLiteral> tmp_lits;

    int a_val = 0;
    for ( int i: Range(a_size) ) {
      auto lit{a_vec[i]};
      if ( model[lit] == SatBool3::True ) {
	a_val |= (1 << i);
	tmp_lits.push_back(~lit);
      }
      else {
	tmp_lits.push_back(lit);
      }
    }

    EXPECT_TRUE( a_val >= b_val );

    mark[a_val] = true;

    mSolver.add_clause(tmp_lits);
  }

  for ( int a_val: Range(exp_a) ) {
    int idx = a_val;
    if ( a_val >= b_val ) {
      EXPECT_TRUE( mark[idx] );
    }
    else {
      EXPECT_FALSE( mark[idx] );
    }
  }
}

TEST_P(SatBvEncTest, add_eq1_4_4)
{
  check_add_eq1(4, 4);
}

TEST_P(SatBvEncTest, add_eq1_2_4)
{
  check_add_eq1(2, 4);
}

TEST_P(SatBvEncTest, add_eq1_4_2)
{
  check_add_eq1(4, 2);
}

TEST_P(SatBvEncTest, add_eq2_4_5)
{
  check_add_eq2(4, 5);
}

TEST_P(SatBvEncTest, add_eq2_4_15)
{
  check_add_eq2(4, 15);
}

TEST_P(SatBvEncTest, add_eq2_4_24)
{
  check_add_eq2(4, 24);
}

TEST_P(SatBvEncTest, add_ne1_4_4)
{
  check_add_ne1(4, 4);
}

TEST_P(SatBvEncTest, add_ne1_2_4)
{
  check_add_ne1(2, 4);
}

TEST_P(SatBvEncTest, add_ne1_4_2)
{
  check_add_ne1(4, 2);
}

TEST_P(SatBvEncTest, add_ne2_4_5)
{
  check_add_ne2(4, 5);
}

TEST_P(SatBvEncTest, add_ne2_4_15)
{
  check_add_ne2(4, 15);
}

TEST_P(SatBvEncTest, add_ne2_4_24)
{
  check_add_ne2(4, 24);
}

TEST_P(SatBvEncTest, add_lt1_4_4)
{
  check_add_lt1(4, 4);
}

TEST_P(SatBvEncTest, add_lt1_2_4)
{
  check_add_lt1(2, 4);
}

TEST_P(SatBvEncTest, add_lt1_4_2)
{
  check_add_lt1(4, 2);
}

TEST_P(SatBvEncTest, add_lt1_1_1)
{
  check_add_lt1(1, 1);
}

TEST_P(SatBvEncTest, add_lt1_1_4)
{
  check_add_lt1(1, 4);
}

TEST_P(SatBvEncTest, add_lt1_4_1)
{
  check_add_lt1(4, 1);
}

TEST_P(SatBvEncTest, add_lt2_4_5)
{
  check_add_lt2(4, 5);
}

TEST_P(SatBvEncTest, add_lt2_4_15)
{
  check_add_lt2(4, 15);
}

TEST_P(SatBvEncTest, add_lt2_4_24)
{
  check_add_lt2(4, 24);
}

TEST_P(SatBvEncTest, add_le1_4_4)
{
  check_add_le1(4, 4);
}

TEST_P(SatBvEncTest, add_le1_2_4)
{
  check_add_le1(2, 4);
}

TEST_P(SatBvEncTest, add_le1_4_2)
{
  check_add_le1(4, 2);
}

TEST_P(SatBvEncTest, add_le1_1_1)
{
  check_add_le1(1, 1);
}

TEST_P(SatBvEncTest, add_le1_1_4)
{
  check_add_le1(1, 4);
}

TEST_P(SatBvEncTest, add_le1_4_1)
{
  check_add_le1(4, 1);
}

TEST_P(SatBvEncTest, add_le2_4_5)
{
  check_add_le2(4, 5);
}

TEST_P(SatBvEncTest, add_le2_4_15)
{
  check_add_le2(4, 15);
}

TEST_P(SatBvEncTest, add_le2_4_24)
{
  check_add_le2(4, 24);
}

TEST_P(SatBvEncTest, add_gt1_4_4)
{
  check_add_gt1(4, 4);
}

TEST_P(SatBvEncTest, add_gt1_2_4)
{
  check_add_gt1(2, 4);
}

TEST_P(SatBvEncTest, add_gt1_4_2)
{
  check_add_gt1(4, 2);
}

TEST_P(SatBvEncTest, add_gt2_4_5)
{
  check_add_gt2(4, 5);
}

TEST_P(SatBvEncTest, add_gt2_4_15)
{
  check_add_gt2(4, 15);
}

TEST_P(SatBvEncTest, add_gt2_4_24)
{
  check_add_gt2(4, 24);
}

TEST_P(SatBvEncTest, add_ge1_4_4)
{
  check_add_ge1(4, 4);
}

TEST_P(SatBvEncTest, add_ge1_2_4)
{
  check_add_ge1(2, 4);
}

TEST_P(SatBvEncTest, add_ge1_4_2)
{
  check_add_ge1(4, 2);
}

TEST_P(SatBvEncTest, add_ge2_4_5)
{
  check_add_ge2(4, 5);
}

TEST_P(SatBvEncTest, add_ge2_4_15)
{
  check_add_ge2(4, 15);
}

TEST_P(SatBvEncTest, add_ge2_4_24)
{
  check_add_ge2(4, 24);
}

#if 0
INSTANTIATE_TEST_CASE_P(SatSolverTest,
			SatBvEncTest,
			::testing::Values("lingeling", "glueminisat2", "minisat2", "minisat",
					  "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));
#endif

INSTANTIATE_TEST_CASE_P(SatSolverTest,
			SatBvEncTest,
			::testing::Values("lingeling", "minisat2", "minisat",
					  "ymsat1", "ymsat2", "ymsat2old", "ymsat1_old"));

END_NAMESPACE_YM
