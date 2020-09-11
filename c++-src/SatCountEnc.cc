
/// @file SatCountEnc.cc
/// @brief SatCountEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatCountEnc.h"
#include "ym/SatTseitinEnc.h"
#include "ym/CombiGen.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatCountEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] solver SATソルバ
SatCountEnc::SatCountEnc(SatSolver& solver) :
  mSolver{solver}
{
}

// @brief デストラクタ
SatCountEnc::~SatCountEnc()
{
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::add_at_most_one(const vector<SatLiteral>& lit_list)
{
  SizeType n{lit_list.size()};
  if ( n <= 1 ) {
    // はじめから条件は満たされている．
    return;
  }
  else if ( n == 2 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    add_at_most_one(lit1, lit2);
  }
  else if ( n == 3 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    add_at_most_one(lit1, lit2, lit3);
  }
  else if ( n == 4 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    auto lit4{lit_list[3]};
    add_at_most_one(lit1, lit2, lit3, lit4);
  }
  else {
    SizeType n1{(n + 1) / 2};
    vector<SatLiteral> lit_list1(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      lit_list1[i] = lit_list[i];
    }
    auto olit1{mSolver.new_variable(false)};
    _add_at_most_one(lit_list1, olit1);

    SizeType n2{n - n1};
    vector<SatLiteral> lit_list2(n2);
    for ( SizeType i = 0; i < n2; ++ i ) {
      lit_list2[i] = lit_list[i + n1];
    }
    auto olit2{mSolver.new_variable(false)};
    _add_at_most_one(lit_list2, olit2);

    mSolver.add_clause(~olit1, ~olit2);
  }
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::add_exact_one(const vector<SatLiteral>& lit_list)
{
  SizeType n{lit_list.size()};
  if ( n == 0 ) {
    // 成り立たない．
    mSolver.add_clause(vector<SatLiteral>());
  }
  else if ( n == 1 ) {
    auto lit{lit_list[0]};
    mSolver.add_clause(lit);
  }
  else if ( n == 2 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    add_exact_one(lit1, lit2);
  }
  else if ( n == 3 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    add_exact_one(lit1, lit2, lit3);
  }
  else if ( n == 4 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    auto lit4{lit_list[3]};
    add_exact_one(lit1, lit2, lit3, lit4);
  }
  else {
    SizeType n1{(n + 1) / 2};
    vector<SatLiteral> lit_list1(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      lit_list1[i] = lit_list[i];
    }
    auto olit1{mSolver.new_variable(false)};
    _add_at_most_one(lit_list1, olit1);

    SizeType n2{n - n1};
    vector<SatLiteral> lit_list2(n2);
    for ( SizeType i = 0; i < n2; ++ i ) {
      lit_list2[i] = lit_list[i + n1];
    }
    auto olit2{mSolver.new_variable(false)};
    _add_at_most_one(lit_list2, olit2);

    mSolver.add_clause( olit1,  olit2);
    mSolver.add_clause(~olit1, ~olit2);
  }
}

// @brief add_at_most_one() の下請け関数
void
SatCountEnc::_add_at_most_one(const vector<SatLiteral>& lit_list,
			      SatLiteral olit)
{
  SizeType n{lit_list.size()};
  ASSERT_COND( n >= 2 );

  SatTseitinEnc enc(mSolver);
  if ( n == 2 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    add_at_most_one(lit1, lit2);
    enc.add_orgate(olit, lit1, lit2);
  }
  else if ( n == 3 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    add_at_most_one(lit1, lit2, lit3);
    enc.add_orgate(olit, lit1, lit2, lit3);
  }
  else if ( n == 4 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    auto lit4{lit_list[3]};
    add_at_most_one(lit1, lit2, lit3, lit4);
    enc.add_orgate(olit, lit1, lit2, lit3, lit4);
  }
  else {
    SizeType n1{(n + 1) / 2};
    vector<SatLiteral> lit_list1(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      lit_list1[i] = lit_list[i];
    }
    auto olit1{mSolver.new_variable(false)};
    _add_at_most_one(lit_list1, olit1);

    SizeType n2{n - n1};
    vector<SatLiteral> lit_list2(n2);
    for ( SizeType i = 0; i < n2; ++ i ) {
      lit_list2[i] = lit_list[i + n1];
    }
    auto olit2{mSolver.new_variable(false)};
    _add_at_most_one(lit_list2, olit2);

    mSolver.add_clause(~olit1, ~olit2);
    enc.add_orgate(olit, olit1, olit2);
  }
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::add_at_most_two(const vector<SatLiteral>& lit_list)
{
  SizeType n{lit_list.size()};
  if ( n <= 2 ) {
    // はじめから条件は満たされている．
    return;
  }
  else if ( n == 3 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    add_at_most_two(lit1, lit2, lit3);
  }
  else if ( n == 4 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    auto lit4{lit_list[3]};
    add_at_most_two(lit1, lit2, lit3, lit4);
  }
  else {
    SizeType n1{(n + 1) / 2};
    vector<SatLiteral> lit_list1(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      lit_list1[i] = lit_list[i];
    }
    auto olit1_1{mSolver.new_variable(false)};
    auto olit1_0{mSolver.new_variable(false)};
    _add_at_most_two(lit_list1, olit1_1, olit1_0);

    SizeType n2{n - n1};
    vector<SatLiteral> lit_list2(n2);
    for ( SizeType i = 0; i < n2; ++ i ) {
      lit_list2[i] = lit_list[i + n1];
    }
    auto olit2_1{mSolver.new_variable(false)};
    auto olit2_0{mSolver.new_variable(false)};
    _add_at_most_two(lit_list2, olit2_1, olit2_0);

    // だめなパタン
    // 1 | 2
    // 2 | 1
    // 2 | 2
    mSolver.add_clause(           ~olit1_0, ~olit2_1          );
    mSolver.add_clause( ~olit1_1,                     ~olit2_0);
    mSolver.add_clause( ~olit1_1,           ~olit2_1          );
  }
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::add_exact_two(const vector<SatLiteral>& lit_list)
{
  SizeType n{lit_list.size()};
  if ( n < 2 ) {
    // 満たせない．
    mSolver.add_clause(vector<SatLiteral>());
    return;
  }
  else if ( n == 2 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    mSolver.add_clause(lit1);
    mSolver.add_clause(lit2);
    return;
  }
  else if ( n == 3 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    add_exact_two(lit1, lit2, lit3);
  }
  else if ( n == 4 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    auto lit4{lit_list[3]};
    add_exact_two(lit1, lit2, lit3, lit4);
  }
  else {
    SizeType n1{(n + 1) / 2};
    vector<SatLiteral> lit_list1(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      lit_list1[i] = lit_list[i];
    }
    auto olit1_1{mSolver.new_variable(false)};
    auto olit1_0{mSolver.new_variable(false)};
    _add_at_most_two(lit_list1, olit1_1, olit1_0);

    SizeType n2{n - n1};
    vector<SatLiteral> lit_list2(n2);
    for ( SizeType i = 0; i < n2; ++ i ) {
      lit_list2[i] = lit_list[i + n1];
    }
    auto olit2_1{mSolver.new_variable(false)};
    auto olit2_0{mSolver.new_variable(false)};
    _add_at_most_two(lit_list2, olit2_1, olit2_0);

    // だめなパタン
    // 0 | 0
    // 0 | 1
    // 1 | 0
    // 1 | 2
    // 2 | 1
    // 2 | 2
    mSolver.add_clause(  olit1_1,  olit1_0,  olit2_1,  olit2_0);
    mSolver.add_clause(  olit1_1,  olit1_0,  olit2_1, ~olit2_0);
    mSolver.add_clause(  olit1_1, ~olit1_0,  olit2_1,  olit2_0);
    mSolver.add_clause(  olit1_1, ~olit1_0, ~olit2_1,  olit2_0);
    mSolver.add_clause( ~olit1_1,  olit1_0,  olit2_1, ~olit2_0);
    mSolver.add_clause( ~olit1_1,  olit1_0, ~olit2_1,  olit2_0);
  }
}

// @brief add_at_most_two() の下請け関数
void
SatCountEnc::_add_at_most_two(const vector<SatLiteral>& lit_list,
			      SatLiteral olit1,
			      SatLiteral olit0)
{
  // lit_list に at_most_two() 制約をつける．
  // lit_list のカウント結果を olit1, olit0 に入れる．

  SizeType n{lit_list.size()};
  ASSERT_COND( n >= 2 );

  SatTseitinEnc enc(mSolver);
  if ( n == 2 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    add_at_most_two(lit1, lit2);
    enc.add_half_adder(lit1, lit2, olit0, olit1);
  }
  else if ( n == 3 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    add_at_most_two(lit1, lit2, lit3);
    enc.add_full_adder(lit1, lit2, lit3, olit0, olit1);
  }
  else if ( n == 4 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    auto lit4{lit_list[3]};
    add_at_most_two(lit1, lit2, lit3, lit4);
    // 本当は足して2以上になる可能性はあるが，
    // at_most_two() 制約があるのでオーバーフローしない．
    auto s1{mSolver.new_variable(false)};
    auto c1{mSolver.new_variable(false)};
    enc.add_half_adder(lit1, lit2, s1, c1);
    auto s2{mSolver.new_variable(false)};
    auto c2{mSolver.new_variable(false)};
    enc.add_half_adder(lit3, lit4, s2, c2);
    auto c3{mSolver.new_variable(false)};
    enc.add_half_adder(s1, s2, olit0, c3);
    enc.add_orgate(olit1, c1, c2, c3);
  }
  else {
    SizeType n1{(n + 1) / 2};
    vector<SatLiteral> lit_list1(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      lit_list1[i] = lit_list[i];
    }
    auto olit1_1{mSolver.new_variable(false)};
    auto olit1_0{mSolver.new_variable(false)};
    _add_at_most_two(lit_list1, olit1_1, olit1_0);

    SizeType n2{n - n1};
    vector<SatLiteral> lit_list2(n2);
    for ( SizeType i = 0; i < n2; ++ i ) {
      lit_list2[i] = lit_list[i + n1];
    }
    auto olit2_1{mSolver.new_variable(false)};
    auto olit2_0{mSolver.new_variable(false)};
    _add_at_most_two(lit_list2, olit2_1, olit2_0);

    mSolver.add_clause(           ~olit1_0, ~olit2_1          );
    mSolver.add_clause( ~olit1_1,                     ~olit2_0);
    mSolver.add_clause( ~olit1_1,           ~olit2_1          );

    auto colit{mSolver.new_variable(false)};
    enc.add_half_adder(olit1_0, olit2_0, olit0, colit);
    enc.add_xorgate(olit1, olit1_1, olit2_1, colit);
  }
}

// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::add_at_least_two(const vector<SatLiteral>& lit_list)
{
  SizeType n{lit_list.size()};
  if ( n < 2 ) {
    // 無理
    mSolver.add_clause(vector<SatLiteral>());
    return;
  }
  else if ( n == 2 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    add_at_least_two(lit1, lit2);
  }
  else if ( n == 3 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    add_at_least_two(lit1, lit2, lit3);
  }
  else if ( n == 4 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    auto lit4{lit_list[3]};
    add_at_least_two(lit1, lit2, lit3, lit4);
  }
  else {
    SizeType n1{(n + 1) / 2};
    vector<SatLiteral> lit_list1(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      lit_list1[i] = lit_list[i];
    }
    auto olit1_1{mSolver.new_variable(false)};
    auto olit1_0{mSolver.new_variable(false)};
    _add_at_least_two(lit_list1, olit1_1, olit1_0);

    SizeType n2{n - n1};
    vector<SatLiteral> lit_list2(n2);
    for ( SizeType i = 0; i < n2; ++ i ) {
      lit_list2[i] = lit_list[i + n1];
    }
    auto olit2_1{mSolver.new_variable(false)};
    auto olit2_0{mSolver.new_variable(false)};
    _add_at_least_two(lit_list2, olit2_1, olit2_0);

    // だめなパタン
    // 0 | 0
    // 0 | 1
    // 1 | 0
    mSolver.add_clause( olit1_1,  olit1_0,  olit2_1          );
    mSolver.add_clause( olit1_1,            olit2_1,  olit2_0);
  }
}

// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatCountEnc::_add_at_least_two(const vector<SatLiteral>& lit_list,
			       SatLiteral olit1,
			       SatLiteral olit0)
{
  SatTseitinEnc enc(mSolver);

  SizeType n{lit_list.size()};
  ASSERT_COND( n >= 2 );
  if ( n == 2 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    enc.add_half_adder(lit1, lit2, olit0, olit1);
  }
  else if ( n == 3 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    enc.add_full_adder(lit1, lit2, lit3, olit0, olit1);
  }
  else if ( n == 4 ) {
    auto lit1{lit_list[0]};
    auto lit2{lit_list[1]};
    auto lit3{lit_list[2]};
    auto lit4{lit_list[3]};
    auto s1{mSolver.new_variable(false)};
    auto c1{mSolver.new_variable(false)};
    enc.add_half_adder(lit1, lit2, s1, c1);
    auto s2{mSolver.new_variable(false)};
    auto c2{mSolver.new_variable(false)};
    enc.add_half_adder(lit3, lit4, s2, c2);
    auto c3{mSolver.new_variable(false)};
    enc.add_half_adder(s1, s2, olit0, c3);
    enc.add_orgate(olit1, c1, c2, c3);
  }
  else {
    SizeType n1{(n + 1) / 2};
    vector<SatLiteral> lit_list1(n1);
    for ( SizeType i = 0; i < n1; ++ i ) {
      lit_list1[i] = lit_list[i];
    }
    auto olit1_1{mSolver.new_variable(false)};
    auto olit1_0{mSolver.new_variable(false)};
    _add_at_least_two(lit_list1, olit1_1, olit1_0);

    SizeType n2 = n - n1;
    vector<SatLiteral> lit_list2(n2);
    for ( SizeType i = 0; i < n2; ++ i ) {
      lit_list2[i] = lit_list[i + n1];
    }
    auto olit2_1{mSolver.new_variable(false)};
    auto olit2_0{mSolver.new_variable(false)};
    _add_at_least_two(lit_list2, olit2_1, olit2_0);

    auto c1{mSolver.new_variable(false)};
    enc.add_half_adder(olit1_0, olit2_0, olit0, c1);
    enc.add_orgate(olit1, olit1_1, olit2_1, c1);
  }
}

// @brief 与えられたリテラルのうちk個しか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatCountEnc::add_at_most_k(const vector<SatLiteral>& lit_list,
			   SizeType k)
{
  SizeType n{lit_list.size()};
  if ( n <= k) {
    // はじめから条件は満たされている．
    return;
  }

  vector<SatLiteral> tmp_lits(k + 1);
  for ( CombiGen cg(n, k + 1) ; !cg.is_end(); ++ cg ) {
    for ( SizeType i: Range(k + 1) ) {
      tmp_lits[i] = ~lit_list[cg(i)];
    }
    mSolver.add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうちk個以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatCountEnc::add_at_least_k(const vector<SatLiteral>& lit_list,
			    SizeType k)
{
  SizeType n = lit_list.size();
  if ( k == 0 ) {
    // はじめから条件は満たされている．
    return;
  }

  SizeType nk = n - k;
  vector<SatLiteral> tmp_lits(nk + 1);
  for ( CombiGen cg(n, nk + 1); !cg.is_end(); ++ cg ) {
    for ( SizeType i: Range(nk + 1) ) {
      tmp_lits[i] = lit_list[cg(i)];
    }
    mSolver.add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうち厳密にk個が true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatCountEnc::add_exact_k(const vector<SatLiteral>& lit_list,
			 SizeType k)
{
  add_at_most_k(lit_list, k);
  add_at_least_k(lit_list, k);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit_lit 入力のリテラルのリスト
void
SatCountEnc::add_not_one(const vector<SatLiteral>& lit_list)
{
  SizeType n = lit_list.size();
  vector<SatLiteral> tmp_lits(n);
  for ( SizeType i: Range(n) ) {
    for ( SizeType j: Range(n) ) {
      auto lit{lit_list[j]};
      if ( j == i ) {
	tmp_lits[j] = ~lit;
      }
      else {
	tmp_lits[j] =  lit;
      }
    }
    mSolver.add_clause(tmp_lits);
  }
}

END_NAMESPACE_YM_SAT
