
/// @file SatBvEnc.cc
/// @brief SatBvEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatBvEnc.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SAT

// @brief コンストラクタ
// @param[in] solver SATソルバ
SatBvEnc::SatBvEnc(SatSolver& solver) :
  mSolver{solver}
{
}

// @brief デストラクタ
SatBvEnc::~SatBvEnc()
{
}

// @brief A == B という条件を追加する．
// @param[in] a_vec, b_vec A,Bを表すビットベクタ
//
// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
// 0と仮定する．
void
SatBvEnc::add_eq(const vector<SatLiteral>& a_vec,
		 const vector<SatLiteral>& b_vec)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  SizeType nb{b_vec.size()};
  ASSERT_COND( nb > 0 );

  SizeType nmin = na;
  if ( nmin > nb ) {
    nmin = nb;
  }

  if ( na > nmin ) {
    // a_vec[na - 1:nmin] は 0 でなければ等しくない．
    for ( SizeType bit = nmin; bit < na; ++ bit ) {
      auto lit{a_vec[bit]};
      mSolver.add_clause(~lit);
    }
  }
  if ( nb > nmin ) {
    // b_vec[nb - 1:nmin] は 0 でなければ等しくない．
    for ( SizeType bit = nmin; bit < nb; ++ bit ) {
      auto lit{b_vec[bit]};
      mSolver.add_clause(~lit);
    }
  }
  // a_vec と b_vec の各ビットが等しい
  for ( SizeType bit = 0; bit < nmin; ++ bit ) {
    auto alit{a_vec[bit]};
    auto blit{b_vec[bit]};
    mSolver.add_clause(~alit,  blit);
    mSolver.add_clause( alit, ~blit);
  }
}

// @brief A == B という条件を追加する．
// @param[in] a_vec Aを表すビットベクタ
// @param[in] b_val Bの値
void
SatBvEnc::add_eq(const vector<SatLiteral>& a_vec,
		 int b_val)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成り立たない．
    mSolver.add_clause(vector<SatLiteral>());
    return;
  }

  for ( SizeType bit = 0; bit < na; ++ bit ) {
    auto alit{a_vec[bit]};
    if ( b_val & (1 << bit) ) {
      // bi == 1 の場合
      mSolver.add_clause(alit);
    }
    else {
      mSolver.add_clause(~alit);
    }
  }
}

// @brief A != B という条件を追加する．
// @param[in] a_vec, b_vec A,Bを表すビットベクタ
//
// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
// 0と仮定する．
void
SatBvEnc::add_ne(const vector<SatLiteral>& a_vec,
		 const vector<SatLiteral>& b_vec)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  SizeType nb{b_vec.size()};
  ASSERT_COND( nb > 0 );

  SizeType nmin = na;
  if ( nmin > nb ) {
    nmin = nb;
  }

  vector<SatLiteral> tmp_lits;
  if ( na > nmin ) {
    // a_vec[na - 1:nmin] が 0 でなければ等しくない．
    for ( SizeType bit = nmin; bit < na; ++ bit ) {
      auto alit{a_vec[bit]};
      tmp_lits.push_back(alit);
    }
  }
  if ( nb > nmin ) {
    // b_vec[nb - 1:nmim] が 0 でなければ等しくない．
    for ( SizeType bit = nmin; bit < nb; ++ bit ) {
      auto blit{b_vec[bit]};
      tmp_lits.push_back(blit);
    }
  }
  for ( SizeType bit = 0; bit < nmin; ++ bit ) {
    // a_vec[i] != b_vec[i] なら等しくない．
    SatLiteral nlit{mSolver.new_variable()};
    auto alit{a_vec[bit]};
    auto blit{b_vec[bit]};
    mSolver.add_clause(~nlit,  alit,  blit);
    mSolver.add_clause(~nlit, ~alit, ~blit);
    mSolver.add_clause( nlit, ~alit,  blit);
    mSolver.add_clause( nlit,  alit, ~blit);
    tmp_lits.push_back(nlit);
  }
  mSolver.add_clause(tmp_lits);
}

// @brief A != B という条件を追加する．
// @param[in] a_vec Aを表すビットベクタ
// @param[in] b_val Bの値
void
SatBvEnc::add_ne(const vector<SatLiteral>& a_vec,
		 int b_val)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成り立っている．
    return;
  }

  vector<SatLiteral> tmp_lits(na);
  for ( SizeType bit = 0; bit < na; ++ bit ) {
    auto alit{a_vec[bit]};
    if ( b_val & (1 << bit) ) {
      tmp_lits[bit] = ~alit;
    }
    else {
      tmp_lits[bit] =  alit;
    }
  }
  mSolver.add_clause(tmp_lits);
}

// @brief A < B という条件を追加する．
// @param[in] a_vec, b_vec A,Bを表すビットベクタ
//
// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
// 0と仮定する．
void
SatBvEnc::add_lt(const vector<SatLiteral>& a_vec,
		 const vector<SatLiteral>& b_vec)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  SizeType nb{b_vec.size()};
  ASSERT_COND( nb > 0 );

  if ( na == 1 && nb == 1 ) {
    // 両方とも1ビットのときは特殊
    auto alit{a_vec[0]};
    auto blit{b_vec[0]};
    mSolver.add_clause(~alit);
    mSolver.add_clause( blit);
    return;
  }

  vector<SatLiteral> tmp_lits;
  SatLiteral flit{kSatLiteralX};

  // 以降で elit: iビット目以上の全桁が等しい
  //       flit: i+1ビット目以上の全桁が等しい
  //       llit: iビット目で a < b が成り立つ．
  // という意味を持つ．

  SizeType n;
  // na < nb の時
  // b_vec の溢れた桁に 1 があれば成り立つ．
  if ( na < nb ) {
    flit = SatLiteral{mSolver.new_variable()};
    vector<SatLiteral> tmp_lits2;
    tmp_lits2.push_back( flit);
    for ( SizeType bit = na; bit < nb; ++ bit ) {
      auto blit{b_vec[bit]};
      tmp_lits.push_back(blit);
      mSolver.add_clause(~flit, ~blit);
      tmp_lits2.push_back(blit);
    }
    mSolver.add_clause(tmp_lits2);
    n = na - 1;
  }
  else if ( na > nb ) {
    flit = SatLiteral{mSolver.new_variable()};
    vector<SatLiteral> tmp_lits2;
    tmp_lits2.push_back( flit);
    for ( SizeType bit = nb; bit < na; ++ bit ) {
      auto alit{a_vec[bit]};
      mSolver.add_clause(~flit, ~alit);
      tmp_lits2.push_back(alit);
    }
    mSolver.add_clause(tmp_lits2);
    n = nb - 1;
  }
  else { // na == nb
    // 最上位桁は特別
    auto alit{a_vec[na - 1]};
    auto blit{b_vec[na - 1]};
    flit = SatLiteral{mSolver.new_variable()};
    mSolver.add_clause(~flit, ~alit,  blit);
    mSolver.add_clause(~flit,  alit, ~blit);
    mSolver.add_clause( flit, ~alit, ~blit);
    mSolver.add_clause( flit,  alit,  blit);
    SatLiteral llit{mSolver.new_variable()};
    mSolver.add_clause(~llit, ~alit       );
    mSolver.add_clause(~llit,         blit);
    mSolver.add_clause( llit,  alit, ~blit);
    tmp_lits.push_back(llit);
    n = na - 2;
  }
  for ( int bit = n; bit >= 0; -- bit ) {
    auto alit{a_vec[bit]};
    auto blit{b_vec[bit]};
    SatLiteral elit{mSolver.new_variable()};
    mSolver.add_clause(~elit,  flit              );
    mSolver.add_clause(~elit,        ~alit,  blit);
    mSolver.add_clause(~elit,         alit, ~blit);
    mSolver.add_clause( elit, ~flit, ~alit, ~blit);
    mSolver.add_clause( elit, ~flit,  alit,  blit);
    SatLiteral llit{mSolver.new_variable()};
    mSolver.add_clause(~llit,  flit              );
    mSolver.add_clause(~llit,        ~alit       );
    mSolver.add_clause(~llit,                blit);
    mSolver.add_clause( llit, ~flit,  alit, ~blit);
    tmp_lits.push_back(llit);
    flit = elit;
  }
  mSolver.add_clause(tmp_lits);
}

// @brief A < B という条件を追加する．
// @param[in] a_vec Aを表すビットベクタ
// @param[in] b_val Bの値
void
SatBvEnc::add_lt(const vector<SatLiteral>& a_vec,
		 int b_val)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成り立つ．
    return;
  }

  vector<SatLiteral> tmp_lits;
  SatLiteral flit;
  {
    auto alit{a_vec[na - 1]};
    if ( b_val & (1 << (na - 1)) ) {
      tmp_lits.push_back(~alit);
      flit =  alit;
    }
    else {
      flit = ~alit;
    }
  }
  for ( int bit = na - 2; bit >= 0; -- bit ) {
    auto alit{a_vec[bit]};
    SatLiteral elit{mSolver.new_variable()};
    if ( b_val & (1 << bit) ) {
      mSolver.add_clause(~elit,  flit       );
      mSolver.add_clause(~elit,         alit);
      mSolver.add_clause( elit, ~flit, ~alit);
      SatLiteral llit{mSolver.new_variable()};
      mSolver.add_clause(~llit,  flit       );
      mSolver.add_clause(~llit,        ~alit);
      mSolver.add_clause( llit, ~flit,  alit);
      tmp_lits.push_back(llit);
    }
    else {
      mSolver.add_clause(~elit,  flit       );
      mSolver.add_clause(~elit,        ~alit);
      mSolver.add_clause( elit, ~flit,  alit);
    }
    flit = elit;
  }
  mSolver.add_clause(tmp_lits);
}

// @brief A <= B という条件を追加する．
// @param[in] a_vec, b_vec A,Bを表すビットベクタ
//
// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
// 0と仮定する．
void
SatBvEnc::add_le(const vector<SatLiteral>& a_vec,
		 const vector<SatLiteral>& b_vec)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  SizeType nb{b_vec.size()};
  ASSERT_COND( nb > 0 );
  vector<SatLiteral> tmp_lits;
  SatLiteral flit{kSatLiteralX};

  // 以降で elit: iビット目以上の全桁が等しい
  //       flit: i+1ビット目以上の全桁が等しい
  //       llit: iビット目で a < b が成り立つ．
  // という意味を持つ．

  // 両方とも1ビットのときは特殊
  if ( na == 1 && nb == 1 ) {
    auto alit{a_vec[0]};
    auto blit{b_vec[0]};
    mSolver.add_clause(~alit,  blit);
    return;
  }

  SizeType n;
  // na < nb の時
  // b_vec の溢れた桁に 1 があれば成り立つ．
  if ( na < nb ) {
    flit = SatLiteral{mSolver.new_variable()};
    vector<SatLiteral> tmp_lits2;
    tmp_lits2.push_back( flit);
    for ( SizeType bit = na; bit < nb; ++ bit ) {
      auto blit{b_vec[bit]};
      tmp_lits.push_back(blit);
      mSolver.add_clause(~flit, ~blit);
      tmp_lits2.push_back(blit);
    }
    mSolver.add_clause(tmp_lits2);
    n = na - 1;
  }
  else if ( na > nb ) {
    flit = SatLiteral{mSolver.new_variable()};
    vector<SatLiteral> tmp_lits2;
    tmp_lits2.push_back( flit);
    for ( SizeType bit = nb; bit < na; ++ bit ) {
      auto alit{a_vec[bit]};
      mSolver.add_clause(~flit, ~alit);
      tmp_lits2.push_back(alit);
    }
    mSolver.add_clause(tmp_lits2);
    n = nb - 1;
  }
  else { // na == nb
    // 最上位桁は特別
    auto alit{a_vec[na - 1]};
    auto blit{b_vec[na - 1]};
    flit = SatLiteral{mSolver.new_variable()};
    mSolver.add_clause(~flit, ~alit,  blit);
    mSolver.add_clause(~flit,  alit, ~blit);
    mSolver.add_clause( flit, ~alit, ~blit);
    mSolver.add_clause( flit,  alit,  blit);
    SatLiteral llit{mSolver.new_variable()};
    mSolver.add_clause(~llit, ~alit       );
    mSolver.add_clause(~llit,         blit);
    mSolver.add_clause( llit,  alit, ~blit);
    tmp_lits.push_back(llit);
    n = na - 2;
  }
  for ( int bit = n; bit > 0; -- bit ) {
    auto alit{a_vec[bit]};
    auto blit{b_vec[bit]};
    SatLiteral elit{mSolver.new_variable()};
    mSolver.add_clause(~elit,  flit              );
    mSolver.add_clause(~elit,        ~alit,  blit);
    mSolver.add_clause(~elit,         alit, ~blit);
    mSolver.add_clause( elit, ~flit, ~alit, ~blit);
    mSolver.add_clause( elit, ~flit,  alit,  blit);
    SatLiteral llit{mSolver.new_variable()};
    mSolver.add_clause(~llit,  flit              );
    mSolver.add_clause(~llit,        ~alit       );
    mSolver.add_clause(~llit,                blit);
    mSolver.add_clause( llit, ~flit,  alit, ~blit);
    tmp_lits.push_back(llit);
    flit = elit;
  }
  { // 最下位の桁のみ a <= b で判定する．
    auto alit{a_vec[0]};
    auto blit{b_vec[0]};
    SatLiteral llit{mSolver.new_variable()};
    mSolver.add_clause(~llit,  flit              );
    mSolver.add_clause(~llit,        ~alit,  blit);
    mSolver.add_clause( llit, ~flit,  alit       );
    mSolver.add_clause( llit, ~flit,        ~blit);
    tmp_lits.push_back(llit);
  }
  mSolver.add_clause(tmp_lits);
}

// @brief A <= B という条件を追加する．
// @param[in] a_vec Aを表すビットベクタ
// @param[in] b_val Bの値
void
SatBvEnc::add_le(const vector<SatLiteral>& a_vec,
		 int b_val)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成り立っている．
    return;
  }

  vector<SatLiteral> tmp_lits;
  SatLiteral flit;
  {
    auto alit{a_vec[na - 1]};
    if ( b_val & (1 << (na - 1)) ) {
      tmp_lits.push_back(~alit);
      flit =  alit;
    }
    else {
      flit = ~alit;
    }
  }
  for ( int bit = na - 2; bit > 0; -- bit ) {
    auto alit{a_vec[bit]};
    SatLiteral elit{mSolver.new_variable()};
    if ( b_val & (1 << bit) ) {
      mSolver.add_clause(~elit,  flit       );
      mSolver.add_clause(~elit,         alit);
      mSolver.add_clause( elit, ~flit, ~alit);
      SatLiteral llit{mSolver.new_variable()};
      mSolver.add_clause(~llit,  flit       );
      mSolver.add_clause(~llit,        ~alit);
      mSolver.add_clause( llit, ~flit,  alit);
      tmp_lits.push_back(llit);
    }
    else {
      mSolver.add_clause(~elit,  flit       );
      mSolver.add_clause(~elit,        ~alit);
      mSolver.add_clause( elit, ~flit,  alit);
    }
    flit = elit;
  }
  { // 最下位のときは a <= b で判断する．
    if ( b_val & (1 << 0) ) {
      tmp_lits.push_back(flit);
    }
    else {
      auto alit{a_vec[0]};
      SatLiteral llit{mSolver.new_variable()};
      mSolver.add_clause(~llit,  flit       );
      mSolver.add_clause(~llit,        ~alit);
      mSolver.add_clause( llit, ~flit,  alit);
      tmp_lits.push_back(llit);
    }
  }
  mSolver.add_clause(tmp_lits);
}

// @brief A > B という条件を追加する．
// @param[in] a_vec Aを表すビットベクタ
// @param[in] b_val Bの値
void
SatBvEnc::add_gt(const vector<SatLiteral>& a_vec,
		 int b_val)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成りたたない．
    mSolver.add_clause(vector<SatLiteral>());
    return;
  }

  vector<SatLiteral> tmp_lits;
  SatLiteral flit;
  {
    auto alit{a_vec[na - 1]};
    if ( b_val & (1 << (na - 1)) ) {
      flit =  alit;
    }
    else {
      tmp_lits.push_back(alit);
      flit = ~alit;
    }
  }
  for ( int bit = na - 2; bit >= 0; -- bit ) {
    auto alit{a_vec[bit]};
    SatLiteral elit{mSolver.new_variable()};
    if ( b_val & (1 << bit) ) {
      mSolver.add_clause(~elit,  flit       );
      mSolver.add_clause(~elit,         alit);
      mSolver.add_clause( elit, ~flit, ~alit);
    }
    else {
      mSolver.add_clause(~elit,  flit       );
      mSolver.add_clause(~elit,        ~alit);
      mSolver.add_clause( elit, ~flit,  alit);
      SatLiteral glit{mSolver.new_variable()};
      mSolver.add_clause(~glit,  flit       );
      mSolver.add_clause(~glit,         alit);
      mSolver.add_clause( glit, ~flit, ~alit);
      tmp_lits.push_back(glit);
    }
    flit = elit;
  }
  mSolver.add_clause(tmp_lits);
}

// @brief A >= B という条件を追加する．
// @param[in] a_vec Aを表すビットベクタ
// @param[in] b_val Bの値
void
SatBvEnc::add_ge(const vector<SatLiteral>& a_vec,
		 int b_val)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成りたたない．
    mSolver.add_clause(vector<SatLiteral>());
    return;
  }

  vector<SatLiteral> tmp_lits;
  SatLiteral flit;
  {
    auto alit{a_vec[na - 1]};
    if ( b_val & (1 << (na - 1)) ) {
      flit =  alit;
    }
    else {
      tmp_lits.push_back(alit);
      flit = ~alit;
    }
  }
  for ( int bit = na - 2; bit > 0; -- bit ) {
    auto alit{a_vec[bit]};
    SatLiteral elit{mSolver.new_variable()};
    if ( b_val & (1 << bit) ) {
      mSolver.add_clause(~elit,  flit       );
      mSolver.add_clause(~elit,         alit);
      mSolver.add_clause( elit, ~flit, ~alit);
    }
    else {
      mSolver.add_clause(~elit,  flit       );
      mSolver.add_clause(~elit,        ~alit);
      mSolver.add_clause( elit, ~flit,  alit);
      SatLiteral glit{mSolver.new_variable()};
      mSolver.add_clause(~glit,  flit       );
      mSolver.add_clause(~glit,         alit);
      mSolver.add_clause( glit, ~flit, ~alit);
      tmp_lits.push_back(glit);
    }
    flit = elit;
  }
  { // 最下位の桁のみ a >= b で判断する．
    auto alit{a_vec[0]};
    if ( b_val & (1 << 0) ) {
      SatLiteral glit{mSolver.new_variable()};
      mSolver.add_clause(~glit,  flit       );
      mSolver.add_clause(~glit,         alit);
      mSolver.add_clause( glit, ~flit, ~alit);
      tmp_lits.push_back(glit);
    }
    else {
      tmp_lits.push_back(flit);
    }
  }
  mSolver.add_clause(tmp_lits);
}

END_NAMESPACE_YM_SAT
