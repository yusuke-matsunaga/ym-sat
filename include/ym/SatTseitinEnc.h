#ifndef SATTSEITINENC_H
#define SATTSEITINENC_H

/// @file SatTseitinEnc.h
/// @brief SatTseitinEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatTseitinEnc SatTseitinEnc.h "SatTseitinEnc.h"
/// @brief Tseitin's encoding を行うクラス
//////////////////////////////////////////////////////////////////////
class SatTseitinEnc
{
public:

  /// @brief コンストラクタ
  /// @param[in] solver SATソルバ
  SatTseitinEnc(SatSolver& solver);

  /// @brief デストラクタ
  ~SatTseitinEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 2つのリテラルが等しいという条件を追加する．
  /// @param[in] lit1, lit2 対象のリテラル
  ///
  /// 具体的には (~lit1 + lit2)(lit1 + ~lit2) の２つの節を追加する．
  void
  add_buffgate(SatLiteral lit1,
	       SatLiteral lit2);

  /// @brief 2つのリテラルが等しくないという条件を追加する．
  /// @param[in] lit1, lit2 対象のリテラル
  ///
  /// 具体的には (~lit1 + ~lit2)(lit1 + lit2) の２つの節を追加する．
  void
  add_notgate(SatLiteral lit1,
	      SatLiteral lit2);

  /// @brief 2入力ANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_andgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2);

  /// @brief 3入力ANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_andgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3);

  /// @brief 4入力ANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_andgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3,
	      SatLiteral lit4);

  /// @brief n入力ANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_andgate(SatLiteral olit,
	      const vector<SatLiteral> lit_list);

  /// @brief 2入力NANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_nandgate(SatLiteral olit,
	       SatLiteral lit1,
	       SatLiteral lit2);

  /// @brief 3入力NANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_nandgate(SatLiteral olit,
	       SatLiteral lit1,
	       SatLiteral lit2,
	       SatLiteral lit3);

  /// @brief 4入力NANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_nandgate(SatLiteral olit,
	       SatLiteral lit1,
	       SatLiteral lit2,
	       SatLiteral lit3,
	       SatLiteral lit4);

  /// @brief n入力NANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_nandgate(SatLiteral olit,
	       const vector<SatLiteral> lit_list);

  /// @brief 2入力ORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_orgate(SatLiteral olit,
	     SatLiteral lit1,
	     SatLiteral lit2);

  /// @brief 3入力ORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_orgate(SatLiteral olit,
	     SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3);

  /// @brief 4入力ORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_orgate(SatLiteral olit,
	     SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4);

  /// @brief n入力ORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_orgate(SatLiteral olit,
	     const vector<SatLiteral> lit_list);

  /// @brief 2入力NORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_norgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2);

  /// @brief 3入力NORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_norgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3);

  /// @brief 4入力NORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_norgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3,
	      SatLiteral lit4);

  /// @brief n入力NORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_norgate(SatLiteral olit,
	      const vector<SatLiteral> lit_list);

  /// @brief 2入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_xorgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2);

  /// @brief 3入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_xorgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3);

  /// @brief 4入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_xorgate(SatLiteral olit,
	      SatLiteral lit1,
	      SatLiteral lit2,
	      SatLiteral lit3,
	      SatLiteral lit4);

  /// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_xorgate(SatLiteral olit,
	      const vector<SatLiteral> lit_list);

  /// @brief 2入力XNORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_xnorgate(SatLiteral olit,
	       SatLiteral lit1,
	       SatLiteral lit2);

  /// @brief 3入力XNORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_xnorgate(SatLiteral olit,
	       SatLiteral lit1,
	       SatLiteral lit2,
	       SatLiteral lit3);

  /// @brief 4入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_xnorgate(SatLiteral olit,
	       SatLiteral lit1,
	       SatLiteral lit2,
	       SatLiteral lit3,
	       SatLiteral lit4);

  /// @brief n入力XNORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_xnorgate(SatLiteral olit,
	       const vector<SatLiteral> lit_list);

  /// @brief half_adder の入出力の関係を表す条件を追加する．
  /// @param[in] alit, blit 入力のリテラル
  /// @param[in] slit 和の出力のリテラル
  /// @param[in] olit キャリー出力のリテラル
  void
  add_half_adder(SatLiteral alit,
		 SatLiteral blit,
		 SatLiteral slit,
		 SatLiteral olit);

  /// @brief full_adder の入出力の関係を表す条件を追加する．
  /// @param[in] alit, blit 入力のリテラル
  /// @param[in] ilit キャリー入力のリテラル
  /// @param[in] slit 和の出力のリテラル
  /// @param[in] olit キャリー出力のリテラル
  void
  add_full_adder(SatLiteral alit,
		 SatLiteral blit,
		 SatLiteral ilit,
		 SatLiteral slit,
		 SatLiteral olit);

  /// @brief 多ビットadderの入出力の関係を表す条件を追加する．
  /// @param[in] alits, blits 入力のリテラル
  /// @param[in] ilit キャリー入力のリテラル
  /// @param[in] slits 和の出力のリテラル
  /// @param[in] olit キャリー出力のリテラル
  ///
  /// * alits, blits のサイズ <= slits のサイズでなければならない．
  /// * 片方が短い場合には上位ビットに0を仮定する．
  void
  add_adder(const vector<SatLiteral>& alits,
	    const vector<SatLiteral>& blits,
	    SatLiteral ilit,
	    const vector<SatLiteral>& slits,
	    SatLiteral olit);

  /// @brief 1's counter の入出力の関係を表す条件を追加する．
  /// @param[in] ilits 入力のリテラル
  /// @param[in] olits 出力のリテラル
  ///
  /// ilits のサイズ < 2^(olits のサイズ) でなければならない．
  void
  add_counter(const vector<SatLiteral>& ilits,
	      const vector<SatLiteral>& olits);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  /// @param[in] start 開始位置
  /// @param[in] num 要素数
  void
  _add_xorgate_sub(SatLiteral olit,
		   const vector<SatLiteral> lit_list,
		   int start,
		   int num);

  /// @brief add_counter の本体
  /// @param[in] ilits 入力のリテラル
  /// @param[in] olits 出力のリテラル
  ///
  /// ベクタサイズのチェックを行わない．
  void
  _add_counter(const vector<SatLiteral>& ilits,
	       const vector<SatLiteral>& olits);


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

// @brief 2つのリテラルが等しいという条件を追加する．
// @param[in] lit1, lit2 対象のリテラル
//
// 具体的には (~lit1 + lit2)(lit1 + ~lit2) の２つの節を追加する．
inline
void
SatTseitinEnc::add_buffgate(SatLiteral lit1,
			    SatLiteral lit2)
{
  mSolver.add_clause(~lit1,  lit2);
  mSolver.add_clause( lit1, ~lit2);
}

// @brief 2つのリテラルが等しくないという条件を追加する．
// @param[in] lit1, lit2 対象のリテラル
//
// 具体的には (~lit1 + ~lit2)(lit1 + lit2) の２つの節を追加する．
inline
void
SatTseitinEnc::add_notgate(SatLiteral lit1,
			   SatLiteral lit2)
{
  add_buffgate(~lit1, lit2);
}

// @brief 2入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatTseitinEnc::add_andgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2)
{
  mSolver.add_clause(~lit1, ~lit2,  olit);
  mSolver.add_clause( lit1,        ~olit);
  mSolver.add_clause(        lit2, ~olit);
}

// @brief 3入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatTseitinEnc::add_andgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  mSolver.add_clause(~lit1, ~lit2, ~lit3,  olit);
  mSolver.add_clause( lit1,               ~olit);
  mSolver.add_clause(        lit2,        ~olit);
  mSolver.add_clause(               lit3, ~olit);
}

// @brief 4入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatTseitinEnc::add_andgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  mSolver.add_clause(~lit1, ~lit2, ~lit3, ~lit4,  olit);
  mSolver.add_clause( lit1,                      ~olit);
  mSolver.add_clause(        lit2,               ~olit);
  mSolver.add_clause(               lit3,        ~olit);
  mSolver.add_clause(                      lit4, ~olit);
}

// @brief 2入力NANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatTseitinEnc::add_nandgate(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2)
{
  add_andgate(~olit, lit1, lit2);
}

// @brief 3入力NANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatTseitinEnc::add_nandgate(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3)
{
  add_andgate(~olit, lit1, lit2, lit3);
}

// @brief 4入力NANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatTseitinEnc::add_nandgate(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4)
{
  add_andgate(~olit, lit1, lit2, lit3, lit4);
}

// @brief n入力NANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatTseitinEnc::add_nandgate(SatLiteral olit,
			    const vector<SatLiteral> lit_list)
{
  add_andgate(~olit, lit_list);
}

// @brief 2入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatTseitinEnc::add_orgate(SatLiteral olit,
			  SatLiteral lit1,
			  SatLiteral lit2)
{
  mSolver.add_clause( lit1,  lit2, ~olit);
  mSolver.add_clause(~lit1,         olit);
  mSolver.add_clause(       ~lit2,  olit);
}

// @brief 3入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatTseitinEnc::add_orgate(SatLiteral olit,
			  SatLiteral lit1,
			  SatLiteral lit2,
			  SatLiteral lit3)
{
  mSolver.add_clause( lit1,  lit2,  lit3, ~olit);
  mSolver.add_clause(~lit1,                olit);
  mSolver.add_clause(       ~lit2,         olit);
  mSolver.add_clause(              ~lit3,  olit);
}

// @brief 4入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatTseitinEnc::add_orgate(SatLiteral olit,
			  SatLiteral lit1,
			  SatLiteral lit2,
			  SatLiteral lit3,
			  SatLiteral lit4)
{
  mSolver.add_clause( lit1,  lit2,  lit3,  lit4, ~olit);
  mSolver.add_clause(~lit1,                       olit);
  mSolver.add_clause(       ~lit2,                olit);
  mSolver.add_clause(              ~lit3,         olit);
  mSolver.add_clause(                     ~lit4,  olit);
}

// @brief 2入力NORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatTseitinEnc::add_norgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2)
{
  add_orgate(~olit, lit1, lit2);
}

// @brief 3入力NORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatTseitinEnc::add_norgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  add_orgate(~olit, lit1, lit2, lit3);
}

// @brief 4入力NORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatTseitinEnc::add_norgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  add_orgate(~olit, lit1, lit2, lit3, lit4);
}

// @brief n入力NORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatTseitinEnc::add_norgate(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  add_orgate(~olit, lit_list);
}

// @brief 2入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatTseitinEnc::add_xorgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2)
{
  mSolver.add_clause( lit1,  lit2, ~olit);
  mSolver.add_clause( lit1, ~lit2,  olit);
  mSolver.add_clause(~lit1,  lit2,  olit);
  mSolver.add_clause(~lit1, ~lit2, ~olit);
}

// @brief 3入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatTseitinEnc::add_xorgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  mSolver.add_clause( lit1,  lit2,  lit3, ~olit);
  mSolver.add_clause( lit1,  lit2, ~lit3,  olit);
  mSolver.add_clause( lit1, ~lit2,  lit3,  olit);
  mSolver.add_clause( lit1, ~lit2, ~lit3, ~olit);
  mSolver.add_clause(~lit1,  lit2,  lit3,  olit);
  mSolver.add_clause(~lit1,  lit2, ~lit3, ~olit);
  mSolver.add_clause(~lit1, ~lit2,  lit3, ~olit);
  mSolver.add_clause(~lit1, ~lit2, ~lit3,  olit);
}

// @brief 4入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatTseitinEnc::add_xorgate(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  _add_xorgate_sub(olit, vector<SatLiteral>{lit1, lit2, lit3, lit4}, 0, 4);
}

// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatTseitinEnc::add_xorgate(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  _add_xorgate_sub(olit, lit_list, 0, n);
}

// @brief 2入力XNORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatTseitinEnc::add_xnorgate(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2)
{
  add_xorgate(~olit, lit1, lit2);
}

// @brief 3入力XNORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatTseitinEnc::add_xnorgate(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3)
{
  add_xorgate(~olit, lit1, lit2, lit3);
}

// @brief 4入力XNORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatTseitinEnc::add_xnorgate(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4)
{
  add_xorgate(~olit, lit1, lit2, lit3, lit4);
}

// @brief n入力XNORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatTseitinEnc::add_xnorgate(SatLiteral olit,
			    const vector<SatLiteral> lit_list)
{
  add_xorgate(~olit, lit_list);
}

END_NAMESPACE_YM_SAT

#endif // SATTSEITINENC_H
