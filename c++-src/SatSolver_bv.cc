
/// @file SatSolver_bv.cc
/// @brief SatSolver の実装ファイル(bitvector 関係)
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SAT

// @brief A == B という条件を追加する．
void
SatSolver::add_eq(
  const vector<SatLiteral>& a_vec,
  const vector<SatLiteral>& b_vec
)
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
      auto lit = a_vec[bit];
      add_clause(~lit);
    }
  }
  if ( nb > nmin ) {
    // b_vec[nb - 1:nmin] は 0 でなければ等しくない．
    for ( SizeType bit = nmin; bit < nb; ++ bit ) {
      auto lit = b_vec[bit];
      add_clause(~lit);
    }
  }
  // a_vec と b_vec の各ビットが等しい
  for ( SizeType bit = 0; bit < nmin; ++ bit ) {
    auto alit = a_vec[bit];
    auto blit = b_vec[bit];
    add_clause(~alit,  blit);
    add_clause( alit, ~blit);
  }
}

// @brief A == B という条件を追加する．
void
SatSolver::add_eq(
  const vector<SatLiteral>& a_vec,
  int b_val
)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成り立たない．
    add_clause(vector<SatLiteral>{});
    return;
  }

  for ( SizeType bit = 0; bit < na; ++ bit ) {
    auto alit = a_vec[bit];
    if ( b_val & (1 << bit) ) {
      // bi == 1 の場合
      add_clause(alit);
    }
    else {
      add_clause(~alit);
    }
  }
}

// @brief A != B という条件を追加する．
void
SatSolver::add_ne(
  const vector<SatLiteral>& a_vec,
  const vector<SatLiteral>& b_vec
)
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
      auto alit = a_vec[bit];
      tmp_lits.push_back(alit);
    }
  }
  if ( nb > nmin ) {
    // b_vec[nb - 1:nmim] が 0 でなければ等しくない．
    for ( SizeType bit = nmin; bit < nb; ++ bit ) {
      auto blit = b_vec[bit];
      tmp_lits.push_back(blit);
    }
  }
  for ( SizeType bit = 0; bit < nmin; ++ bit ) {
    // a_vec[i] != b_vec[i] なら等しくない．
    auto nlit = new_variable();
    auto alit = a_vec[bit];
    auto blit = b_vec[bit];
    add_clause(~nlit,  alit,  blit);
    add_clause(~nlit, ~alit, ~blit);
    add_clause( nlit, ~alit,  blit);
    add_clause( nlit,  alit, ~blit);
    tmp_lits.push_back(nlit);
  }
  add_clause(tmp_lits);
}

// @brief A != B という条件を追加する．
void
SatSolver::add_ne(
  const vector<SatLiteral>& a_vec,
  int b_val
)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成り立っている．
    return;
  }

  vector<SatLiteral> tmp_lits(na);
  for ( SizeType bit = 0; bit < na; ++ bit ) {
    auto alit = a_vec[bit];
    if ( b_val & (1 << bit) ) {
      tmp_lits[bit] = ~alit;
    }
    else {
      tmp_lits[bit] =  alit;
    }
  }
  add_clause(tmp_lits);
}

// @brief A < B という条件を追加する．
void
SatSolver::add_lt(
  const vector<SatLiteral>& a_vec,
  const vector<SatLiteral>& b_vec
)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  SizeType nb{b_vec.size()};
  ASSERT_COND( nb > 0 );

  if ( na == 1 && nb == 1 ) {
    // 両方とも1ビットのときは特殊
    auto alit = a_vec[0];
    auto blit = b_vec[0];
    add_clause(~alit);
    add_clause( blit);
    return;
  }

  vector<SatLiteral> tmp_lits;
  auto flit = kSatLiteralX;

  // 以降で elit: iビット目以上の全桁が等しい
  //        flit: i+1ビット目以上の全桁が等しい
  //        llit: iビット目で a < b が成り立つ．
  // という意味を持つ．

  SizeType n;
  // na < nb の時
  // b_vec の溢れた桁に 1 があれば成り立つ．
  if ( na < nb ) {
    flit = new_variable();
    vector<SatLiteral> tmp_lits2;
    tmp_lits2.push_back( flit);
    for ( SizeType bit = na; bit < nb; ++ bit ) {
      auto blit = b_vec[bit];
      tmp_lits.push_back(blit);
      add_clause(~flit, ~blit);
      tmp_lits2.push_back(blit);
    }
    add_clause(tmp_lits2);
    n = na - 1;
  }
  else if ( na > nb ) {
    flit = new_variable();
    vector<SatLiteral> tmp_lits2;
    tmp_lits2.push_back( flit);
    for ( SizeType bit = nb; bit < na; ++ bit ) {
      auto alit = a_vec[bit];
      add_clause(~flit, ~alit);
      tmp_lits2.push_back(alit);
    }
    add_clause(tmp_lits2);
    n = nb - 1;
  }
  else { // na == nb
    // 最上位桁は特別
    auto alit = a_vec[na - 1];
    auto blit = b_vec[na - 1];
    flit = new_variable();
    add_clause(~flit, ~alit,  blit);
    add_clause(~flit,  alit, ~blit);
    add_clause( flit, ~alit, ~blit);
    add_clause( flit,  alit,  blit);
    auto llit = new_variable();
    add_clause(~llit, ~alit       );
    add_clause(~llit,         blit);
    add_clause( llit,  alit, ~blit);
    tmp_lits.push_back(llit);
    n = na - 2;
  }
  for ( int bit = n; bit >= 0; -- bit ) {
    auto alit = a_vec[bit];
    auto blit = b_vec[bit];
    auto elit = new_variable();
    add_clause(~elit,  flit              );
    add_clause(~elit,        ~alit,  blit);
    add_clause(~elit,         alit, ~blit);
    add_clause( elit, ~flit, ~alit, ~blit);
    add_clause( elit, ~flit,  alit,  blit);
    auto llit = new_variable();
    add_clause(~llit,  flit              );
    add_clause(~llit,        ~alit       );
    add_clause(~llit,                blit);
    add_clause( llit, ~flit,  alit, ~blit);
    tmp_lits.push_back(llit);
    flit = elit;
  }
  add_clause(tmp_lits);
}

// @brief A < B という条件を追加する．
void
SatSolver::add_lt(
  const vector<SatLiteral>& a_vec,
  int b_val
)
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
    auto alit = a_vec[na - 1];
    if ( b_val & (1 << (na - 1)) ) {
      tmp_lits.push_back(~alit);
      flit =  alit;
    }
    else {
      flit = ~alit;
    }
  }
  for ( int bit = na - 2; bit >= 0; -- bit ) {
    auto alit = a_vec[bit];
    auto elit = new_variable();
    if ( b_val & (1 << bit) ) {
      add_clause(~elit,  flit       );
      add_clause(~elit,         alit);
      add_clause( elit, ~flit, ~alit);
      auto llit = new_variable();
      add_clause(~llit,  flit       );
      add_clause(~llit,        ~alit);
      add_clause( llit, ~flit,  alit);
      tmp_lits.push_back(llit);
    }
    else {
      add_clause(~elit,  flit       );
      add_clause(~elit,        ~alit);
      add_clause( elit, ~flit,  alit);
    }
    flit = elit;
  }
  add_clause(tmp_lits);
}

// @brief A <= B という条件を追加する．
void
SatSolver::add_le(
  const vector<SatLiteral>& a_vec,
  const vector<SatLiteral>& b_vec
)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  SizeType nb{b_vec.size()};
  ASSERT_COND( nb > 0 );
  vector<SatLiteral> tmp_lits;
  auto flit = kSatLiteralX;

  // 以降で elit: iビット目以上の全桁が等しい
  //       flit: i+1ビット目以上の全桁が等しい
  //       llit: iビット目で a < b が成り立つ．
  // という意味を持つ．

  // 両方とも1ビットのときは特殊
  if ( na == 1 && nb == 1 ) {
    auto alit = a_vec[0];
    auto blit = b_vec[0];
    add_clause(~alit,  blit);
    return;
  }

  SizeType n;
  // na < nb の時
  // b_vec の溢れた桁に 1 があれば成り立つ．
  if ( na < nb ) {
    flit = new_variable();
    vector<SatLiteral> tmp_lits2;
    tmp_lits2.push_back( flit);
    for ( SizeType bit = na; bit < nb; ++ bit ) {
      auto blit = b_vec[bit];
      tmp_lits.push_back(blit);
      add_clause(~flit, ~blit);
      tmp_lits2.push_back(blit);
    }
    add_clause(tmp_lits2);
    n = na - 1;
  }
  else if ( na > nb ) {
    flit = new_variable();
    vector<SatLiteral> tmp_lits2;
    tmp_lits2.push_back( flit);
    for ( SizeType bit = nb; bit < na; ++ bit ) {
      auto alit = a_vec[bit];
      add_clause(~flit, ~alit);
      tmp_lits2.push_back(alit);
    }
    add_clause(tmp_lits2);
    n = nb - 1;
  }
  else { // na == nb
    // 最上位桁は特別
    auto alit = a_vec[na - 1];
    auto blit = b_vec[na - 1];
    flit = new_variable();
    add_clause(~flit, ~alit,  blit);
    add_clause(~flit,  alit, ~blit);
    add_clause( flit, ~alit, ~blit);
    add_clause( flit,  alit,  blit);
    auto llit = new_variable();
    add_clause(~llit, ~alit       );
    add_clause(~llit,         blit);
    add_clause( llit,  alit, ~blit);
    tmp_lits.push_back(llit);
    n = na - 2;
  }
  for ( int bit = n; bit > 0; -- bit ) {
    auto alit = a_vec[bit];
    auto blit = b_vec[bit];
    auto elit = new_variable();
    add_clause(~elit,  flit              );
    add_clause(~elit,        ~alit,  blit);
    add_clause(~elit,         alit, ~blit);
    add_clause( elit, ~flit, ~alit, ~blit);
    add_clause( elit, ~flit,  alit,  blit);
    auto llit = new_variable();
    add_clause(~llit,  flit              );
    add_clause(~llit,        ~alit       );
    add_clause(~llit,                blit);
    add_clause( llit, ~flit,  alit, ~blit);
    tmp_lits.push_back(llit);
    flit = elit;
  }
  { // 最下位の桁のみ a <= b で判定する．
    auto alit = a_vec[0];
    auto blit = b_vec[0];
    auto llit = new_variable();
    add_clause(~llit,  flit              );
    add_clause(~llit,        ~alit,  blit);
    add_clause( llit, ~flit,  alit       );
    add_clause( llit, ~flit,        ~blit);
    tmp_lits.push_back(llit);
  }
  add_clause(tmp_lits);
}

// @brief A <= B という条件を追加する．
void
SatSolver::add_le(
  const vector<SatLiteral>& a_vec,
  int b_val
)
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
    auto alit = a_vec[na - 1];
    if ( b_val & (1 << (na - 1)) ) {
      tmp_lits.push_back(~alit);
      flit =  alit;
    }
    else {
      flit = ~alit;
    }
  }
  for ( int bit = na - 2; bit > 0; -- bit ) {
    auto alit = a_vec[bit];
    auto elit = new_variable();
    if ( b_val & (1 << bit) ) {
      add_clause(~elit,  flit       );
      add_clause(~elit,         alit);
      add_clause( elit, ~flit, ~alit);
      auto llit = new_variable();
      add_clause(~llit,  flit       );
      add_clause(~llit,        ~alit);
      add_clause( llit, ~flit,  alit);
      tmp_lits.push_back(llit);
    }
    else {
      add_clause(~elit,  flit       );
      add_clause(~elit,        ~alit);
      add_clause( elit, ~flit,  alit);
    }
    flit = elit;
  }
  { // 最下位のときは a <= b で判断する．
    if ( b_val & (1 << 0) ) {
      tmp_lits.push_back(flit);
    }
    else {
      auto alit = a_vec[0];
      auto llit = new_variable();
      add_clause(~llit,  flit       );
      add_clause(~llit,        ~alit);
      add_clause( llit, ~flit,  alit);
      tmp_lits.push_back(llit);
    }
  }
  add_clause(tmp_lits);
}

// @brief A > B という条件を追加する．
void
SatSolver::add_gt(
  const vector<SatLiteral>& a_vec,
  int b_val
)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成りたたない．
    add_clause(vector<SatLiteral>{});
    return;
  }

  vector<SatLiteral> tmp_lits;
  SatLiteral flit;
  {
    auto alit = a_vec[na - 1];
    if ( b_val & (1 << (na - 1)) ) {
      flit =  alit;
    }
    else {
      tmp_lits.push_back(alit);
      flit = ~alit;
    }
  }
  for ( int bit = na - 2; bit >= 0; -- bit ) {
    auto alit = a_vec[bit];
    auto elit = new_variable();
    if ( b_val & (1 << bit) ) {
      add_clause(~elit,  flit       );
      add_clause(~elit,         alit);
      add_clause( elit, ~flit, ~alit);
    }
    else {
      add_clause(~elit,  flit       );
      add_clause(~elit,        ~alit);
      add_clause( elit, ~flit,  alit);
      auto glit = new_variable();
      add_clause(~glit,  flit       );
      add_clause(~glit,         alit);
      add_clause( glit, ~flit, ~alit);
      tmp_lits.push_back(glit);
    }
    flit = elit;
  }
  add_clause(tmp_lits);
}

// @brief A >= B という条件を追加する．
void
SatSolver::add_ge(
  const vector<SatLiteral>& a_vec,
  int b_val
)
{
  SizeType na{a_vec.size()};
  ASSERT_COND( na > 0 );
  if ( (1 << na) <= b_val ) {
    // 常に成りたたない．
    add_clause(vector<SatLiteral>{});
    return;
  }

  vector<SatLiteral> tmp_lits;
  SatLiteral flit;
  {
    auto alit = a_vec[na - 1];
    if ( b_val & (1 << (na - 1)) ) {
      flit =  alit;
    }
    else {
      tmp_lits.push_back(alit);
      flit = ~alit;
    }
  }
  for ( int bit = na - 2; bit > 0; -- bit ) {
    auto alit = a_vec[bit];
    auto elit = new_variable();
    if ( b_val & (1 << bit) ) {
      add_clause(~elit,  flit       );
      add_clause(~elit,         alit);
      add_clause( elit, ~flit, ~alit);
    }
    else {
      add_clause(~elit,  flit       );
      add_clause(~elit,        ~alit);
      add_clause( elit, ~flit,  alit);
      auto glit = new_variable();
      add_clause(~glit,  flit       );
      add_clause(~glit,         alit);
      add_clause( glit, ~flit, ~alit);
      tmp_lits.push_back(glit);
    }
    flit = elit;
  }
  { // 最下位の桁のみ a >= b で判断する．
    auto alit = a_vec[0];
    if ( b_val & (1 << 0) ) {
      auto glit = new_variable();
      add_clause(~glit,  flit       );
      add_clause(~glit,         alit);
      add_clause( glit, ~flit, ~alit);
      tmp_lits.push_back(glit);
    }
    else {
      tmp_lits.push_back(flit);
    }
  }
  add_clause(tmp_lits);
}

END_NAMESPACE_YM_SAT
