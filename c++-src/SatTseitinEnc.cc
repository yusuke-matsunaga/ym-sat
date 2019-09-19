
/// @file SatTseitinEnc.cc
/// @brief SatTseitinEnc の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatTseitinEnc.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatTseitinEnc
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] solver SATソルバ
SatTseitinEnc::SatTseitinEnc(SatSolver& solver) :
  mSolver{solver}
{
}

// @brief デストラクタ
SatTseitinEnc::~SatTseitinEnc()
{
}

// @brief n入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatTseitinEnc::add_andgate(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits(n + 1);
  for ( int i = 0; i < n; ++ i ) {
    SatLiteral ilit{lit_list[i]};
    tmp_lits[i] = ~ilit;
    mSolver.add_clause(ilit, ~olit);
  }
  tmp_lits[n] = olit;
  mSolver.add_clause(tmp_lits);
}

// @brief n入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatTseitinEnc::add_orgate(SatLiteral olit,
			  const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits(n + 1);
  for ( int i = 0; i < n; ++ i ) {
    SatLiteral ilit{lit_list[i]};
    mSolver.add_clause(~ilit, olit);
    tmp_lits[i] = ilit;
  }
  tmp_lits[n] = ~olit;
  mSolver.add_clause(tmp_lits);
}

// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] start 開始位置
// @param[in] num 要素数
void
SatTseitinEnc::_add_xorgate_sub(SatLiteral olit,
				const vector<SatLiteral> lit_list,
				int start,
				int num)
{
  ASSERT_COND( num >= 2 );

  if ( num == 2 ) {
    auto lit0{lit_list[start + 0]};
    auto lit1{lit_list[start + 1]};
    add_xorgate(olit, lit0, lit1);
  }
  else if ( num == 3 ) {
    auto lit0{lit_list[start + 0]};
    auto lit1{lit_list[start + 1]};
    auto lit2{lit_list[start + 2]};
    add_xorgate(olit, lit0, lit1, lit2);
  }
  else {
    int nl = num / 2;
    int nr = num - nl;
    auto llit{mSolver.new_variable(false)};
    _add_xorgate_sub(llit, lit_list, start, nl);
    auto rlit{mSolver.new_variable(false)};
    _add_xorgate_sub(rlit, lit_list, start + nl, nr);
    add_xorgate(olit, llit, rlit);
  }
}

// @brief half_adder の入出力の関係を表す条件を追加する．
// @param[in] alit, blit 入力のリテラル
// @param[in] slit 和の出力のリテラル
// @param[in] olit キャリー出力のリテラル
void
SatTseitinEnc::add_half_adder(SatLiteral alit,
			      SatLiteral blit,
			      SatLiteral slit,
			      SatLiteral olit)
{
  mSolver.add_clause(~slit,  alit,  blit);
  mSolver.add_clause( slit,  alit, ~blit);
  mSolver.add_clause( slit, ~alit,  blit);
  mSolver.add_clause(~slit, ~alit, ~blit);
  mSolver.add_clause(~olit,  alit       );
  mSolver.add_clause(~olit,         blit);
  mSolver.add_clause( olit, ~alit, ~blit);
}

// @brief full_adder の入出力の関係を表す条件を追加する．
// @param[in] alit, blit 入力のリテラル
// @param[in] ilit キャリー入力のリテラル
// @param[in] slit 和の出力のリテラル
// @param[in] olit キャリー出力のリテラル
void
SatTseitinEnc::add_full_adder(SatLiteral alit,
			     SatLiteral blit,
			     SatLiteral ilit,
			     SatLiteral slit,
			     SatLiteral olit)
{
  mSolver.add_clause(~slit,  alit,  blit,  ilit);
  mSolver.add_clause( slit,  alit,  blit, ~ilit);
  mSolver.add_clause( slit,  alit, ~blit,  ilit);
  mSolver.add_clause(~slit,  alit, ~blit, ~ilit);
  mSolver.add_clause( slit, ~alit,  blit,  ilit);
  mSolver.add_clause(~slit, ~alit,  blit, ~ilit);
  mSolver.add_clause(~slit, ~alit, ~blit,  ilit);
  mSolver.add_clause( slit, ~alit, ~blit, ~ilit);
  mSolver.add_clause(~olit,  alit,  blit       );
  mSolver.add_clause(~olit,  alit,         ilit);
  mSolver.add_clause(~olit,         blit,  ilit);
  mSolver.add_clause( olit, ~alit, ~blit       );
  mSolver.add_clause( olit, ~alit,        ~ilit);
  mSolver.add_clause( olit,        ~blit, ~ilit);
}

// @brief 多ビットadderの入出力の関係を表す条件を追加する．
// @param[in] alits, blits 入力のリテラル
// @param[in] ilit キャリー入力のリテラル
// @param[in] slits 和の出力のリテラル
// @param[in] olit キャリー出力のリテラル
void
SatTseitinEnc::add_adder(const vector<SatLiteral>& alits,
			 const vector<SatLiteral>& blits,
			 SatLiteral ilit,
			 const vector<SatLiteral>& slits,
			 SatLiteral olit)
{
  int na = alits.size();
  int nb = blits.size();
  int ns = slits.size();
  ASSERT_COND( na <= ns );
  ASSERT_COND( nb <= ns );

  bool a_zero = false;
  bool b_zero = false;
  bool c_zero = false;
  for ( int i = 0; i < ns; ++ i ) {
    auto slit{slits[i]};
    auto alit{kSatLiteralX};
    auto blit{kSatLiteralX};
    if ( i < na ) {
      alit = alits[i];
    }
    else {
      a_zero = true;
    }
    if ( i < nb ) {
      blit = blits[i];
    }
    else {
      b_zero = true;
    }
    if ( a_zero && b_zero ) {
      if ( c_zero ) {
	mSolver.add_clause(~slit);
      }
      else {
	add_buffgate(ilit, slit);
	c_zero = true;
      }
      if ( i == (ns - 1) ) {
	mSolver.add_clause(~olit);
      }
    }
    else if ( a_zero ) { // !b_zero
      auto olit1{i < (ns - 1) ? mSolver.new_variable(false) : olit};
      add_half_adder(blit, ilit, slit, olit1);
      ilit = olit1;
    }
    else if ( b_zero ) { // !a_zero
      auto olit1{i < (ns - 1) ? mSolver.new_variable(false) : olit};
      add_half_adder(alit, ilit, slit, olit1);
      ilit = olit1;
    }
    else { // !a_zero && !b_zero
      auto olit1{i < (ns - 1) ? mSolver.new_variable(false) : olit};
      add_full_adder(alit, blit, ilit, slit, olit1);
      ilit = olit1;
    }
  }
}

BEGIN_NONAMESPACE

int
get_ln(int n)
{
  int n_ln = 0;
  while ( (1 << n_ln) <= n ) {
    ++ n_ln;
  }
  return n_ln;
}

END_NONAMESPACE

// @brief 1's counter の入出力の関係を表す条件を追加する．
// @param[in] ilits 入力のリテラル
// @param[in] olits 出力のリテラル
//
// ilits のサイズ < 2^(olits のサイズ) でなければならない．
void
SatTseitinEnc::add_counter(const vector<SatLiteral>& ilits,
			   const vector<SatLiteral>& olits)
{
  int ni = ilits.size();
  int no = olits.size();

  int no_exp = 1 << no;

  ASSERT_COND( ni < no_exp );

  int ni_ln = get_ln(ni);
  if ( ni_ln < no ) {
    // 出力の桁が多すぎる場合
    vector<SatLiteral> olits1(ni_ln);
    for ( int i = 0; i < ni_ln; ++ i ) {
      olits1[i] = olits[i];
    }
    _add_counter(ilits, olits1);
    for ( int i = ni_ln; i < no; ++ i ) {
      mSolver.add_clause(~olits[i]);
    }
  }
  else {
    _add_counter(ilits, olits);
  }
}

// @brief add_counter の本体
// @param[in] ilits 入力のリテラル
// @param[in] olits 出力のリテラル
//
// ベクタサイズのチェックを行わない．
void
SatTseitinEnc::_add_counter(const vector<SatLiteral>& ilits,
			    const vector<SatLiteral>& olits)
{
  int ni = ilits.size();
  int no = olits.size();

  if ( ni == 1 ) {
    ASSERT_COND( no == 1 );
    add_buffgate(ilits[0], olits[0]);
  }
  else if ( ni == 2 ) {
    ASSERT_COND( no == 2 );
    add_half_adder(ilits[0], ilits[1], olits[0], olits[1]);
  }
  else if ( ni == 3 ) {
    ASSERT_COND( no == 2 );
    add_full_adder(ilits[0], ilits[1], ilits[2], olits[0], olits[1]);
  }
  else if ( ni == 4 ) {
    ASSERT_COND( no == 3 );
    auto c0{mSolver.new_variable(false)};
    auto c1{mSolver.new_variable(false)};
    auto d0{mSolver.new_variable(false)};
    auto d1{mSolver.new_variable(false)};
    auto e1{mSolver.new_variable(false)};
    add_half_adder(ilits[0], ilits[1], c0, c1);
    add_half_adder(ilits[2], ilits[3], d0, d1);
    add_half_adder(c0, d0, olits[0], e1);
    add_full_adder(c1, d1, e1, olits[1], olits[2]);
  }
  else if ( ni == 5 ) {
    ASSERT_COND( no == 3 );
    auto c0{mSolver.new_variable(false)};
    auto c1{mSolver.new_variable(false)};
    auto d0{mSolver.new_variable(false)};
    auto d1{mSolver.new_variable(false)};
    auto e1{mSolver.new_variable(false)};
    add_half_adder(ilits[0], ilits[1], c0, c1);
    add_half_adder(ilits[2], ilits[3], d0, d1);
    vector<SatLiteral> clits{c0, c1};
    vector<SatLiteral> dlits{d0, d1};
    vector<SatLiteral> tmp_olits{olits[0], olits[1]};
    add_adder(clits, dlits, ilits[4], tmp_olits, olits[2]);
  }
  else {
    int ni1 = (ni - 1) / 2;
    vector<SatLiteral> tmp_ilits1(ni1);
    for ( int i = 0; i < ni1; ++ i ) {
      tmp_ilits1[i] = ilits[i];
    }
    int no1 = get_ln(ni1);
    vector<SatLiteral> tmp_olits1(no1);
    for ( int i = 0; i < no1; ++ i ) {
      tmp_olits1[i] = mSolver.new_variable(false);
    }
    _add_counter(tmp_ilits1, tmp_olits1);

    int ni2 = ni - ni1 - 1;
    vector<SatLiteral> tmp_ilits2(ni2);
    for ( int i = 0; i < ni2; ++ i ) {
      tmp_ilits2[i] = ilits[i + ni1];
    }
    int no2 = get_ln(ni2);
    vector<SatLiteral> tmp_olits2(no2);
    for ( int i = 0; i < no2; ++ i ) {
      tmp_olits2[i] = mSolver.new_variable(false);
    }
    add_counter(tmp_ilits2, tmp_olits2);

    vector<SatLiteral> tmp_olits3(no - 1);
    for ( int i = 0; i < (no - 1); ++ i ) {
      tmp_olits3[i] = olits[i];
    }
    add_adder(tmp_olits1, tmp_olits2, ilits[ni - 1], tmp_olits3, olits[no - 1]);
  }
}

END_NAMESPACE_YM_SAT
