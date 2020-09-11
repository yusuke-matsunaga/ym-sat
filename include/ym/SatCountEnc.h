#ifndef SATCOUNTENC_H
#define SATCOUNTENC_H

/// @file SatCountEnc.h
/// @brief SatCountEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatCountEnc SatCountEnc.h "SatCountEnc.h"
/// @brief 1's count に基づく encoding を行うクラス
//////////////////////////////////////////////////////////////////////
class SatCountEnc
{
public:

  /// @brief コンストラクタ
  /// @param[in] solver SATソルバ
  SatCountEnc(SatSolver& solver);

  /// @brief デストラクタ
  ~SatCountEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_at_most_one(SatLiteral lit1,
		  SatLiteral lit2);

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_at_most_one(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3);

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_at_most_one(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4);

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
  void
  add_at_most_one(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4,
		  SatLiteral lit5);

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
  void
  add_at_most_one(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4,
		  SatLiteral lit5,
		  SatLiteral lit6);

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_at_most_one(const vector<SatLiteral>& lit_list);

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_at_most_two(SatLiteral lit1,
		  SatLiteral lit2);

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_at_most_two(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3);

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_at_most_two(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4);

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
  void
  add_at_most_two(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4,
		  SatLiteral lit5);

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
  void
  add_at_most_two(SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4,
		  SatLiteral lit5,
		  SatLiteral lit6);

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_at_most_two(const vector<SatLiteral>& lit_list);

  /// @brief 与えられたリテラルのうちk個しか true にならない条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  /// @param[in] k しきい値
  void
  add_at_most_k(const vector<SatLiteral>& lit_list,
		SizeType k);

  /// @brief 与えられたリテラルのうち1以上は true になる条件を追加する．
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_at_least_one(SatLiteral lit1,
		   SatLiteral lit2);

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_at_least_one(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3);

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_at_least_one(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4);

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
  void
  add_at_least_one(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4,
		   SatLiteral lit5);

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
  void
  add_at_least_one(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4,
		   SatLiteral lit5,
		   SatLiteral lit6);

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_at_least_one(const vector<SatLiteral>& lit_list);

  /// @brief 与えられたリテラルのうち2以上は true になる条件を追加する．
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_at_least_two(SatLiteral lit1,
		   SatLiteral lit2);

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_at_least_two(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3);

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_at_least_two(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4);

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
  void
  add_at_least_two(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4,
		   SatLiteral lit5);

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
  void
  add_at_least_two(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4,
		   SatLiteral lit5,
		   SatLiteral lit6);

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_at_least_two(const vector<SatLiteral>& lit_list);

  /// @brief 与えられたリテラルのうちk個以上は true になる条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  /// @param[in] k しきい値
  void
  add_at_least_k(const vector<SatLiteral>& lit_list,
		 SizeType k);

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_exact_one(SatLiteral lit1,
		SatLiteral lit2);

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_exact_one(SatLiteral lit1,
		SatLiteral lit2,
		SatLiteral lit3);

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_exact_one(SatLiteral lit1,
		SatLiteral lit2,
		SatLiteral lit3,
		SatLiteral lit4);

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
  void
  add_exact_one(SatLiteral lit1,
		SatLiteral lit2,
		SatLiteral lit3,
		SatLiteral lit4,
		SatLiteral lit5);

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
  void
  add_exact_one(SatLiteral lit1,
		SatLiteral lit2,
		SatLiteral lit3,
		SatLiteral lit4,
		SatLiteral lit5,
		SatLiteral lit6);

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_exact_one(const vector<SatLiteral>& lit_list);

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_exact_two(SatLiteral lit1,
		SatLiteral lit2);

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_exact_two(SatLiteral lit1,
		SatLiteral lit2,
		SatLiteral lit3);

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_exact_two(SatLiteral lit1,
		SatLiteral lit2,
		SatLiteral lit3,
		SatLiteral lit4);

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
  void
  add_exact_two(SatLiteral lit1,
		SatLiteral lit2,
		SatLiteral lit3,
		SatLiteral lit4,
		SatLiteral lit5);

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
  void
  add_exact_two(SatLiteral lit1,
		SatLiteral lit2,
		SatLiteral lit3,
		SatLiteral lit4,
		SatLiteral lit5,
		SatLiteral lit6);

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_exact_two(const vector<SatLiteral>& lit_list);

  /// @brief 与えられたリテラルのうち厳密にk個が true になる条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  /// @param[in] k しきい値
  void
  add_exact_k(const vector<SatLiteral>& lit_list,
	      SizeType k);

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_not_one(SatLiteral lit1,
	      SatLiteral lit2);

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_not_one(SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3);

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_not_one(SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3,
	      SatLiteral lit4);

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
  void
  add_not_one(SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3,
	      SatLiteral lit4,
	      SatLiteral lit5);

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
  void
  add_not_one(SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3,
	      SatLiteral lit4,
	      SatLiteral lit5,
	      SatLiteral lit6);

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_not_one(const vector<SatLiteral>& lit_list);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief add_at_most_one() の下請け関数
  void
  _add_at_most_one(const vector<SatLiteral>& lit_list,
		   SatLiteral olit);

  /// @brief add_at_most_two() の下請け関数
  void
  _add_at_most_two(const vector<SatLiteral>& lit_list,
		   SatLiteral olit1,
		   SatLiteral olit0);

  /// @brief add_at_least_two() の下請け関数
  void
  _add_at_least_two(const vector<SatLiteral>& lit_list,
		    SatLiteral olit1,
		    SatLiteral olit0);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatCountEnc::add_at_most_one(SatLiteral lit1,
			     SatLiteral lit2)
{
  mSolver.add_clause(~lit1, ~lit2);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatCountEnc::add_at_most_one(SatLiteral lit1,
			     SatLiteral lit2,
			     SatLiteral lit3)
{
  mSolver.add_clause(~lit1, ~lit2);
  mSolver.add_clause(~lit1, ~lit3);
  mSolver.add_clause(~lit2, ~lit3);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatCountEnc::add_at_most_one(SatLiteral lit1,
			     SatLiteral lit2,
			     SatLiteral lit3,
			     SatLiteral lit4)
{
  mSolver.add_clause(~lit1, ~lit2);
  mSolver.add_clause(~lit1, ~lit3);
  mSolver.add_clause(~lit1, ~lit4);
  mSolver.add_clause(~lit2, ~lit3);
  mSolver.add_clause(~lit2, ~lit4);
  mSolver.add_clause(~lit3, ~lit4);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatCountEnc::add_at_most_one(SatLiteral lit1,
			     SatLiteral lit2,
			     SatLiteral lit3,
			     SatLiteral lit4,
			     SatLiteral lit5)
{
  mSolver.add_clause(~lit1, ~lit2);
  mSolver.add_clause(~lit1, ~lit3);
  mSolver.add_clause(~lit1, ~lit4);
  mSolver.add_clause(~lit1, ~lit5);
  mSolver.add_clause(~lit2, ~lit3);
  mSolver.add_clause(~lit2, ~lit4);
  mSolver.add_clause(~lit2, ~lit5);
  mSolver.add_clause(~lit3, ~lit4);
  mSolver.add_clause(~lit3, ~lit5);
  mSolver.add_clause(~lit4, ~lit5);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatCountEnc::add_at_most_one(SatLiteral lit1,
			     SatLiteral lit2,
			     SatLiteral lit3,
			     SatLiteral lit4,
			     SatLiteral lit5,
			     SatLiteral lit6)
{
  mSolver.add_clause(~lit1, ~lit2);
  mSolver.add_clause(~lit1, ~lit3);
  mSolver.add_clause(~lit1, ~lit4);
  mSolver.add_clause(~lit1, ~lit5);
  mSolver.add_clause(~lit1, ~lit6);
  mSolver.add_clause(~lit2, ~lit3);
  mSolver.add_clause(~lit2, ~lit4);
  mSolver.add_clause(~lit2, ~lit5);
  mSolver.add_clause(~lit2, ~lit6);
  mSolver.add_clause(~lit3, ~lit4);
  mSolver.add_clause(~lit3, ~lit5);
  mSolver.add_clause(~lit3, ~lit6);
  mSolver.add_clause(~lit4, ~lit5);
  mSolver.add_clause(~lit4, ~lit6);
  mSolver.add_clause(~lit5, ~lit6);
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatCountEnc::add_at_most_two(SatLiteral lit1,
			     SatLiteral lit2)
{
  // 無条件でなりたつ．
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatCountEnc::add_at_most_two(SatLiteral lit1,
			     SatLiteral lit2,
			     SatLiteral lit3)
{
  mSolver.add_clause(~lit1, ~lit2, ~lit3);
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatCountEnc::add_at_most_two(SatLiteral lit1,
			     SatLiteral lit2,
			     SatLiteral lit3,
			     SatLiteral lit4)
{
  mSolver.add_clause(~lit1, ~lit2, ~lit3       );
  mSolver.add_clause(~lit1, ~lit2       , ~lit4);
  mSolver.add_clause(~lit1,        ~lit3, ~lit4);
  mSolver.add_clause(       ~lit2, ~lit3, ~lit4);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatCountEnc::add_at_most_two(SatLiteral lit1,
			     SatLiteral lit2,
			     SatLiteral lit3,
			     SatLiteral lit4,
			     SatLiteral lit5)
{
  mSolver.add_clause(~lit1, ~lit2, ~lit3              );
  mSolver.add_clause(~lit1, ~lit2,        ~lit4       );
  mSolver.add_clause(~lit1, ~lit2,               ~lit5);
  mSolver.add_clause(~lit1,        ~lit3, ~lit4       );
  mSolver.add_clause(~lit1,        ~lit3,        ~lit5);
  mSolver.add_clause(~lit1,               ~lit4, ~lit5);
  mSolver.add_clause(       ~lit2, ~lit3, ~lit4       );
  mSolver.add_clause(       ~lit2, ~lit3,        ~lit5);
  mSolver.add_clause(       ~lit2,        ~lit4, ~lit5);
  mSolver.add_clause(              ~lit3, ~lit4, ~lit5);
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatCountEnc::add_at_most_two(SatLiteral lit1,
			     SatLiteral lit2,
			     SatLiteral lit3,
			     SatLiteral lit4,
			     SatLiteral lit5,
			     SatLiteral lit6)
{
  mSolver.add_clause(~lit1, ~lit2, ~lit3                     );
  mSolver.add_clause(~lit1, ~lit2,        ~lit4              );
  mSolver.add_clause(~lit1, ~lit2,               ~lit5       );
  mSolver.add_clause(~lit1, ~lit2,                      ~lit6);
  mSolver.add_clause(~lit1,        ~lit3, ~lit4              );
  mSolver.add_clause(~lit1,        ~lit3,        ~lit5       );
  mSolver.add_clause(~lit1,        ~lit3,               ~lit6);
  mSolver.add_clause(~lit1,               ~lit4, ~lit5       );
  mSolver.add_clause(~lit1,               ~lit4,        ~lit6);
  mSolver.add_clause(~lit1,                      ~lit5, ~lit6);
  mSolver.add_clause(       ~lit2, ~lit3, ~lit4              );
  mSolver.add_clause(       ~lit2, ~lit3,        ~lit5       );
  mSolver.add_clause(       ~lit2, ~lit3,               ~lit6);
  mSolver.add_clause(       ~lit2,        ~lit4, ~lit5       );
  mSolver.add_clause(       ~lit2,        ~lit4,        ~lit6);
  mSolver.add_clause(       ~lit2,               ~lit5, ~lit6);
  mSolver.add_clause(              ~lit3, ~lit4, ~lit5       );
  mSolver.add_clause(              ~lit3, ~lit4,        ~lit6);
  mSolver.add_clause(              ~lit3,        ~lit5, ~lit6);
  mSolver.add_clause(                     ~lit4, ~lit5, ~lit6);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatCountEnc::add_at_least_one(SatLiteral lit1,
			      SatLiteral lit2)
{
  mSolver.add_clause( lit1,  lit2);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatCountEnc::add_at_least_one(SatLiteral lit1,
			      SatLiteral lit2,
			      SatLiteral lit3)
{
  mSolver.add_clause( lit1,  lit2,  lit3);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatCountEnc::add_at_least_one(SatLiteral lit1,
			      SatLiteral lit2,
			      SatLiteral lit3,
			      SatLiteral lit4)
{
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatCountEnc::add_at_least_one(SatLiteral lit1,
			      SatLiteral lit2,
			      SatLiteral lit3,
			      SatLiteral lit4,
			      SatLiteral lit5)
{
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4,  lit5);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatCountEnc::add_at_least_one(SatLiteral lit1,
			      SatLiteral lit2,
			      SatLiteral lit3,
			      SatLiteral lit4,
			      SatLiteral lit5,
			      SatLiteral lit6)
{
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4,  lit5,  lit6);
}

// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatCountEnc::add_at_least_one(const vector<SatLiteral>& lit_list)
{
  mSolver.add_clause(lit_list);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatCountEnc::add_at_least_two(SatLiteral lit1,
			      SatLiteral lit2)
{
  mSolver.add_clause(lit1);
  mSolver.add_clause(lit2);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatCountEnc::add_at_least_two(SatLiteral lit1,
			      SatLiteral lit2,
			      SatLiteral lit3)
{
  // 2つ以上が False にならない．
  mSolver.add_clause( lit1,  lit2       );
  mSolver.add_clause( lit1,         lit3);
  mSolver.add_clause(        lit2,  lit3);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatCountEnc::add_at_least_two(SatLiteral lit1,
			      SatLiteral lit2,
			      SatLiteral lit3,
			      SatLiteral lit4)
{
  // 3つ以上が False にならない．
  mSolver.add_clause( lit1,  lit2,  lit3       );
  mSolver.add_clause( lit1,  lit2,         lit4);
  mSolver.add_clause( lit1,         lit3,  lit4);
  mSolver.add_clause(        lit2,  lit3,  lit4);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatCountEnc::add_at_least_two(SatLiteral lit1,
			      SatLiteral lit2,
			      SatLiteral lit3,
			      SatLiteral lit4,
			      SatLiteral lit5)
{
  // 4つ以上が False にならない．
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4       );
  mSolver.add_clause( lit1,  lit2,  lit3,         lit5);
  mSolver.add_clause( lit1,  lit2,         lit4,  lit5);
  mSolver.add_clause( lit1,         lit3,  lit4,  lit5);
  mSolver.add_clause(        lit2,  lit3,  lit4,  lit5);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatCountEnc::add_at_least_two(SatLiteral lit1,
			      SatLiteral lit2,
			      SatLiteral lit3,
			      SatLiteral lit4,
			      SatLiteral lit5,
			      SatLiteral lit6)
{
  // 5つ以上が False にならない．
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4,  lit5       );
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4,         lit6);
  mSolver.add_clause( lit1,  lit2,  lit3,         lit5,  lit6);
  mSolver.add_clause( lit1,  lit2,         lit4,  lit5,  lit6);
  mSolver.add_clause( lit1,         lit3,  lit4,  lit5,  lit6);
  mSolver.add_clause(        lit2,  lit3,  lit4,  lit5,  lit6);
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatCountEnc::add_exact_one(SatLiteral lit1,
			   SatLiteral lit2)
{
  add_at_most_one(lit1, lit2);
  add_at_least_one(lit1, lit2);
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatCountEnc::add_exact_one(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  add_at_most_one(lit1, lit2, lit3);
  add_at_least_one(lit1, lit2, lit3);
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatCountEnc::add_exact_one(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  add_at_most_one(lit1, lit2, lit3, lit4);
  add_at_least_one(lit1, lit2, lit3, lit4);
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatCountEnc::add_exact_one(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4,
			   SatLiteral lit5)
{
  add_at_most_one(lit1, lit2, lit3, lit4, lit5);
  add_at_least_one(lit1, lit2, lit3, lit4, lit5);
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatCountEnc::add_exact_one(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4,
			   SatLiteral lit5,
			   SatLiteral lit6)
{
  add_at_most_one(lit1, lit2, lit3, lit4, lit5, lit6);
  add_at_least_one(lit1, lit2, lit3, lit4, lit5, lit6);
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatCountEnc::add_exact_two(SatLiteral lit1,
			   SatLiteral lit2)
{
  add_at_most_two(lit1, lit2);
  add_at_least_two(lit1, lit2);
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatCountEnc::add_exact_two(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  add_at_most_two(lit1, lit2, lit3);
  add_at_least_two(lit1, lit2, lit3);
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatCountEnc::add_exact_two(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  add_at_most_two(lit1, lit2, lit3, lit4);
  add_at_least_two(lit1, lit2, lit3, lit4);
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatCountEnc::add_exact_two(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4,
			   SatLiteral lit5)
{
  add_at_most_two(lit1, lit2, lit3, lit4, lit5);
  add_at_least_two(lit1, lit2, lit3, lit4, lit5);
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatCountEnc::add_exact_two(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4,
			   SatLiteral lit5,
			   SatLiteral lit6)
{
  add_at_most_two(lit1, lit2, lit3, lit4, lit5, lit6);
  add_at_least_two(lit1, lit2, lit3, lit4, lit5, lit6);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatCountEnc::add_not_one(SatLiteral lit1,
			 SatLiteral lit2)
{
  mSolver.add_clause( lit1, ~lit2);
  mSolver.add_clause(~lit1,  lit2);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatCountEnc::add_not_one(SatLiteral lit1,
			 SatLiteral lit2,
			 SatLiteral lit3)
{
  mSolver.add_clause( lit1,  lit2, ~lit3);
  mSolver.add_clause( lit1, ~lit2,  lit3);
  mSolver.add_clause(~lit1,  lit2,  lit3);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatCountEnc::add_not_one(SatLiteral lit1,
			 SatLiteral lit2,
			 SatLiteral lit3,
			 SatLiteral lit4)
{
  mSolver.add_clause( lit1,  lit2,  lit3, ~lit4);
  mSolver.add_clause( lit1,  lit2, ~lit3,  lit4);
  mSolver.add_clause( lit1, ~lit2,  lit3,  lit4);
  mSolver.add_clause(~lit1,  lit2,  lit3,  lit4);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatCountEnc::add_not_one(SatLiteral lit1,
			 SatLiteral lit2,
			 SatLiteral lit3,
			 SatLiteral lit4,
			 SatLiteral lit5)
{
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4, ~lit5);
  mSolver.add_clause( lit1,  lit2,  lit3, ~lit4,  lit5);
  mSolver.add_clause( lit1,  lit2, ~lit3,  lit4,  lit5);
  mSolver.add_clause( lit1, ~lit2,  lit3,  lit4,  lit5);
  mSolver.add_clause(~lit1,  lit2,  lit3,  lit4,  lit5);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatCountEnc::add_not_one(SatLiteral lit1,
			 SatLiteral lit2,
			 SatLiteral lit3,
			 SatLiteral lit4,
			 SatLiteral lit5,
			 SatLiteral lit6)
{
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4,  lit5, ~lit6);
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4, ~lit5,  lit6);
  mSolver.add_clause( lit1,  lit2,  lit3, ~lit4,  lit5,  lit6);
  mSolver.add_clause( lit1,  lit2, ~lit3,  lit4,  lit5,  lit6);
  mSolver.add_clause( lit1, ~lit2,  lit3,  lit4,  lit5,  lit6);
  mSolver.add_clause(~lit1,  lit2,  lit3,  lit4,  lit5,  lit6);
}

END_NAMESPACE_YM_SAT

#endif // SATCOUNTENC_H
