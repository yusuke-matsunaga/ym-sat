
/// @file SatOrderedSet.cc
/// @brief SatOrderedSet の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatOrderedSet.h"
#include "ym/SatSolver.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_YM_SAT

// @brief コンストラクタ
SatOrderedSet::SatOrderedSet(
  SatSolver& solver,
  int min,
  int max
) : mSolver{solver},
    mMin{min},
    mMax{max}
{
  ASSERT_COND( min <= max );

  mPriVarArray.clear();
  mVarArray.clear();

  SizeType n = mMax - mMin + 1;
  SizeType n1 = n - 1;

  // 2種類の変数ベクタを作る．
  // mPriVarArray は順序符号化
  if ( n1 > 0 ) {
    mPriVarArray.resize(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      mPriVarArray[i] = mSolver.new_variable();
      // 順序制約を作る．
      if ( i > 0 ) {
	auto lit1 = mPriVarArray[i - 1];
	auto lit2 = mPriVarArray[i + 0];
	mSolver.add_clause( lit1, ~lit2);
      }
    }
  }

  // mVarArray は one-hot 符号化を行う．
  mVarArray.resize(n);

  if ( n1 > 0 ) {
    mVarArray[0] = ~mPriVarArray[0];
    for ( int i = 1; i < n1; ++ i ) {
      mVarArray[i] = mSolver.new_variable();
    }
    mVarArray[n1] = mPriVarArray[n1 - 1];
  }
  else {
    auto lit = mSolver.new_variable();
    mVarArray[0] = lit;
    solver.add_clause(lit);
  }

  // 順序制約から one-hot へ符号化を行う．
  for ( SizeType i = 1; i < n1; ++ i ) {
    auto lit1 = mPriVarArray[i - 1];
    auto lit2 = mPriVarArray[i + 0];
    auto lit = mVarArray[i];
    mSolver.add_clause(~lit,  lit1       );
    mSolver.add_clause(~lit,        ~lit2);
    mSolver.add_clause( lit, ~lit1,  lit2);
  }
}

// @brief SATの解から値を得る．
int
SatOrderedSet::val(
  const SatModel& model
) const
{
  SizeType n = mVarArray.size();
  if ( n > 1 ) {
    for ( SizeType i = 0; i < n; ++ i ) {
      auto lit = mVarArray[i];
      if ( model[lit] == SatBool3::True ) {
	return i + mMin;
      }
    }
    ASSERT_NOT_REACHED;
    return -1;
  }
  else {
    return mMin;
  }
}

// @brief この変数の値が lval 以上になるという制約を作る．
void
SatOrderedSet::add_ge_constraint(
  int lval
)
{
  ASSERT_COND( mMin <= lval && lval <= mMax );

  if ( lval == mMin ) {
    // 常に成り立つ．
    return;
  }

  auto lit = mPriVarArray[lval - mMin - 1];
  mSolver.add_clause(lit);
}

// @brief この変数の値が uval 以下になるという制約を作る．
void
SatOrderedSet::add_le_constraint(
  int uval
)
{
  ASSERT_COND( mMin <= uval && uval <= mMax );

  if ( uval == mMax ) {
    // 常に成り立つ．
    return;
  }

  auto lit = mPriVarArray[uval - mMin];
  mSolver.add_clause(~lit);
}

// @brief この変数の値が uval 以下か lval 以上という制約を作る．
void
SatOrderedSet::add_dropoff_constraint(
  int uval,
  int lval
)
{
  ASSERT_COND( mMin <= uval && uval <= mMax );
  ASSERT_COND( mMin <= lval && lval <= mMax );

  if ( uval >= lval ) {
    // 常に成り立つ．
    return;
  }
  // この時点で uval == mMax, lval == mMin はない．

  auto lit1 = mPriVarArray[uval - mMin];
  auto lit2 = mPriVarArray[lval - mMin - 1];
  mSolver.add_clause(~lit1, lit2);
}

END_NAMESPACE_YM_SAT
