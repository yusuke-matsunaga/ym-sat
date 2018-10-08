﻿#ifndef YM_SATSOLVER_H
#define YM_SATSOLVER_H

/// @file ym/SatSolver.h
/// @brief SatSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2017, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_YM_SAT

class SatSolverImpl;
class SatLogger;

//////////////////////////////////////////////////////////////////////
/// @class SatSolver SatSolver.h "ym/SatSolver.h"
/// @ingroup SatGroup
/// @brief CNF-SAT エンジンのインターフェイスを表すクラス
///
/// 実際の処理は SatSolverImpl (の継承クラス)が行う．
//////////////////////////////////////////////////////////////////////
class SatSolver
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  /// @param[in] solver_type 実装タイプ
  SatSolver(const SatSolverType& solver_type = SatSolverType());

  /// @brief デストラクタ
  ~SatSolver();

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 変数と節の生成を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を追加する．
  /// @param[in] decision 決定変数の時に true とする．
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  SatVarId
  new_variable(bool decision = true);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit_list 条件リテラルのリスト
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(const vector<SatLiteral>& lit_list);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit1 条件リテラル
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(SatLiteral lit1);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit1, lit2 条件リテラル
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(SatLiteral lit1,
			   SatLiteral lit2);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit1, lit2, lit3 条件リテラル
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3);

  /// @brief 条件リテラルを設定する．
  /// @param[in] lit1, lit2, lit3, lit4 条件リテラル
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4);

  /// @brief 条件リテラルをクリアする．
  void
  clear_conditional_literals();

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 1項の節(リテラル)を追加する．
  /// @param[in] lit1 リテラル
  void
  add_clause(SatLiteral lit1);

  /// @brief 2項の節を追加する．
  /// @param[in] lit1, lit2 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2);

  /// @brief 3項の節を追加する．
  /// @param[in] lit1, lit2, lit3 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3);

  /// @brief 4項の節を追加する．
  /// @param[in] lit1, lit2, lit3, lit4 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4);

  /// @brief 5項の節を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4,
	     SatLiteral lit5);

  /// @brief 6項の節を追加する．
  /// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 リテラル
  void
  add_clause(SatLiteral lit1,
	     SatLiteral lit2,
	     SatLiteral lit3,
	     SatLiteral lit4,
	     SatLiteral lit5,
	     SatLiteral lit6);

  /// @brief 2つのリテラルが等しいという条件を追加する．
  /// @param[in] lit1, lit2 対象のリテラル
  ///
  /// 具体的には (~lit1 + lit2)(lit1 + ~lit2) の２つの節を追加する．
  void
  add_eq_rel(SatLiteral lit1,
	     SatLiteral lit2);

  /// @brief 2つのリテラルが等しくないという条件を追加する．
  /// @param[in] lit1, lit2 対象のリテラル
  ///
  /// 具体的には (~lit1 + ~lit2)(lit1 + lit2) の２つの節を追加する．
  void
  add_neq_rel(SatLiteral lit1,
	      SatLiteral lit2);

  /// @brief 2入力ANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_andgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2);

  /// @brief 3入力ANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_andgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3);

  /// @brief 4入力ANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_andgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4);

  /// @brief n入力ANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_andgate_rel(SatLiteral olit,
		  const vector<SatLiteral> lit_list);

  /// @brief 2入力NANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_nandgate_rel(SatLiteral olit,
		   SatLiteral lit1,
		   SatLiteral lit2);

  /// @brief 3入力NANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_nandgate_rel(SatLiteral olit,
		   SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3);

  /// @brief 4入力NANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_nandgate_rel(SatLiteral olit,
		   SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4);

  /// @brief n入力NANDゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_nandgate_rel(SatLiteral olit,
		   const vector<SatLiteral> lit_list);

  /// @brief 2入力ORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_orgate_rel(SatLiteral olit,
		 SatLiteral lit1,
		 SatLiteral lit2);

  /// @brief 3入力ORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_orgate_rel(SatLiteral olit,
		 SatLiteral lit1,
		 SatLiteral lit2,
		 SatLiteral lit3);

  /// @brief 4入力ORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_orgate_rel(SatLiteral olit,
		 SatLiteral lit1,
		 SatLiteral lit2,
		 SatLiteral lit3,
		 SatLiteral lit4);

  /// @brief n入力ORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_orgate_rel(SatLiteral olit,
		 const vector<SatLiteral> lit_list);

  /// @brief 2入力NORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_norgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2);

  /// @brief 3入力NORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_norgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3);

  /// @brief 4入力NORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_norgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4);

  /// @brief n入力NORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_norgate_rel(SatLiteral olit,
		  const vector<SatLiteral> lit_list);

  /// @brief 2入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_xorgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2);

  /// @brief 3入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_xorgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3);

  /// @brief 4入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_xorgate_rel(SatLiteral olit,
		  SatLiteral lit1,
		  SatLiteral lit2,
		  SatLiteral lit3,
		  SatLiteral lit4);

  /// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_xorgate_rel(SatLiteral olit,
		  const vector<SatLiteral> lit_list);

  /// @brief 2入力XNORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2 入力のリテラル
  void
  add_xnorgate_rel(SatLiteral olit,
		   SatLiteral lit1,
		   SatLiteral lit2);

  /// @brief 3入力XNORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3 入力のリテラル
  void
  add_xnorgate_rel(SatLiteral olit,
		   SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3);

  /// @brief 4入力XORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
  void
  add_xnorgate_rel(SatLiteral olit,
		   SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4);

  /// @brief n入力XNORゲートの入出力の関係を表す条件を追加する．
  /// @param[in] olit 出力のリテラル
  /// @param[in] lit_list 入力のリテラルのリスト
  void
  add_xnorgate_rel(SatLiteral olit,
		   const vector<SatLiteral> lit_list);

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
		int k);

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
		 int k);

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
	      int k);

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

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 動作制御を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief SAT 問題を解く．
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @retval kB3True  充足した．
  /// @retval kB3False 充足不能が判明した．
  /// @retval kB3X     わからなかった．
  ///
  /// i 番めの変数の割り当て結果は model[i] に入る．
  /// 通常は complete な割り当てなので値は true/false だけになるはず
  SatBool3
  solve(vector<SatBool3>& model);

  /// @brief assumption 付きの SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @retval kSat 充足した．
  /// @retval kUnsat 充足不能が判明した．
  /// @retval kUndet わからなかった．
  ///
  /// i 番めの変数の割り当て結果は model[i] に入る．
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	vector<SatBool3>& model);

  /// @brief assumption 付きの SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
  /// @retval kSat 充足した．
  /// @retval kUnsat 充足不能が判明した．
  /// @retval kUndet わからなかった．
  ///
  /// i 番めの変数の割り当て結果は model[i] に入る．
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	vector<SatBool3>& model,
	vector<SatLiteral>& conflicts);

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  void
  stop();

  /// @brief 時間計測機能を制御する
  void
  timer_on(bool enable);

  /// @brief conflict_limit の最大値
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  int
  set_max_conflict(int val);

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  /// @param[in] msg_handler 登録するメッセージハンドラ
  void
  reg_msg_handler(SatMsgHandler* msg_handler);

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内部状態の取得を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const;

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  void
  get_stats(SatStats& stats) const;

  /// @brief 変数の数を得る．
  int
  variable_num() const;

  /// @brief 制約節の数を得る．
  int
  clause_num() const;

  /// @brief 制約節のリテラルの総数を得る．
  int
  literal_num() const;

  /// @brief DIMACS 形式で制約節を出力する．
  /// @param[in] s 出力先のストリーム
  void
  write_DIMACS(ostream& s) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実際の機能を実装しているクラス
  SatSolverImpl* mImpl;

  // ロガー
  SatLogger* mLogger;

  // 条件リテラル
  vector<SatLiteral> mConditionalLits;

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
SatSolver::add_eq_rel(SatLiteral lit1,
		      SatLiteral lit2)
{
  add_clause(~lit1,  lit2);
  add_clause( lit1, ~lit2);
}

// @brief 2つのリテラルが等しくないという条件を追加する．
// @param[in] lit1, lit2 対象のリテラル
//
// 具体的には (~lit1 + ~lit2)(lit1 + lit2) の２つの節を追加する．
inline
void
SatSolver::add_neq_rel(SatLiteral lit1,
		       SatLiteral lit2)
{
  add_eq_rel(~lit1, lit2);
}

// @brief 2入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_andgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2)
{
  add_clause(~lit1, ~lit2,  olit);
  add_clause( lit1,        ~olit);
  add_clause(        lit2, ~olit);
}

// @brief 3入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_andgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  add_clause(~lit1, ~lit2, ~lit3,  olit);
  add_clause( lit1,               ~olit);
  add_clause(        lit2,        ~olit);
  add_clause(               lit3, ~olit);
}

// @brief 4入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_andgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  add_clause(~lit1, ~lit2, ~lit3, ~lit4,  olit);
  add_clause( lit1,                      ~olit);
  add_clause(        lit2,               ~olit);
  add_clause(               lit3,        ~olit);
  add_clause(                      lit4, ~olit);
}

// @brief 2入力NANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_nandgate_rel(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2)
{
  add_andgate_rel(~olit, lit1, lit2);
}

// @brief 3入力NANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_nandgate_rel(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3)
{
  add_andgate_rel(~olit, lit1, lit2, lit3);
}

// @brief 4入力NANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_nandgate_rel(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4)
{
  add_andgate_rel(~olit, lit1, lit2, lit3, lit4);
}

// @brief n入力NANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatSolver::add_nandgate_rel(SatLiteral olit,
			    const vector<SatLiteral> lit_list)
{
  add_andgate_rel(~olit, lit_list);
}

// @brief 2入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_orgate_rel(SatLiteral olit,
			  SatLiteral lit1,
			  SatLiteral lit2)
{
  add_clause( lit1,  lit2, ~olit);
  add_clause(~lit1,         olit);
  add_clause(       ~lit2,  olit);
}

// @brief 3入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_orgate_rel(SatLiteral olit,
			  SatLiteral lit1,
			  SatLiteral lit2,
			  SatLiteral lit3)
{
  add_clause( lit1,  lit2,  lit3, ~olit);
  add_clause(~lit1,                olit);
  add_clause(       ~lit2,         olit);
  add_clause(              ~lit3,  olit);
}

// @brief 4入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_orgate_rel(SatLiteral olit,
			  SatLiteral lit1,
			  SatLiteral lit2,
			  SatLiteral lit3,
			  SatLiteral lit4)
{
  add_clause( lit1,  lit2,  lit3,  lit4, ~olit);
  add_clause(~lit1,                       olit);
  add_clause(       ~lit2,                olit);
  add_clause(              ~lit3,         olit);
  add_clause(                     ~lit4,  olit);
}

// @brief 2入力NORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_norgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2)
{
  add_orgate_rel(~olit, lit1, lit2);
}

// @brief 3入力NORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_norgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  add_orgate_rel(~olit, lit1, lit2, lit3);
}

// @brief 4入力NORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_norgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  add_orgate_rel(~olit, lit1, lit2, lit3, lit4);
}

// @brief n入力NORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatSolver::add_norgate_rel(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  add_orgate_rel(~olit, lit_list);
}

// @brief 2入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_xorgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2)
{
  add_clause( lit1,  lit2, ~olit);
  add_clause( lit1, ~lit2,  olit);
  add_clause(~lit1,  lit2,  olit);
  add_clause(~lit1, ~lit2, ~olit);
}

// @brief 3入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_xorgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  add_clause( lit1,  lit2,  lit3, ~olit);
  add_clause( lit1,  lit2, ~lit3,  olit);
  add_clause( lit1, ~lit2,  lit3,  olit);
  add_clause( lit1, ~lit2, ~lit3, ~olit);
  add_clause(~lit1,  lit2,  lit3,  olit);
  add_clause(~lit1,  lit2, ~lit3, ~olit);
  add_clause(~lit1, ~lit2,  lit3, ~olit);
  add_clause(~lit1, ~lit2, ~lit3,  olit);
}

// @brief 4入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_xorgate_rel(SatLiteral olit,
			   SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  add_clause( lit1,  lit2,  lit3,  lit4, ~olit);
  add_clause( lit1,  lit2,  lit3, ~lit4,  olit);
  add_clause( lit1,  lit2, ~lit3,  lit4,  olit);
  add_clause( lit1,  lit2, ~lit3, ~lit4, ~olit);
  add_clause( lit1, ~lit2,  lit3,  lit4,  olit);
  add_clause( lit1, ~lit2,  lit3, ~lit4, ~olit);
  add_clause( lit1, ~lit2, ~lit3,  lit4, ~olit);
  add_clause( lit1, ~lit2, ~lit3, ~lit4,  olit);
  add_clause(~lit1,  lit2,  lit3,  lit4,  olit);
  add_clause(~lit1,  lit2,  lit3, ~lit4, ~olit);
  add_clause(~lit1,  lit2, ~lit3,  lit4, ~olit);
  add_clause(~lit1,  lit2, ~lit3, ~lit4,  olit);
  add_clause(~lit1, ~lit2,  lit3,  lit4, ~olit);
  add_clause(~lit1, ~lit2,  lit3, ~lit4,  olit);
  add_clause(~lit1, ~lit2, ~lit3,  lit4,  olit);
  add_clause(~lit1, ~lit2, ~lit3, ~lit4, ~olit);
}

// @brief 2入力XNORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_xnorgate_rel(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2)
{
  add_xorgate_rel(~olit, lit1, lit2);
}

// @brief 3入力XNORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_xnorgate_rel(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3)
{
  add_xorgate_rel(~olit, lit1, lit2, lit3);
}

// @brief 4入力XNORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_xnorgate_rel(SatLiteral olit,
			    SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4)
{
  add_xorgate_rel(~olit, lit1, lit2, lit3, lit4);
}

// @brief n入力XNORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatSolver::add_xnorgate_rel(SatLiteral olit,
			    const vector<SatLiteral> lit_list)
{
  add_xorgate_rel(~olit, lit_list);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_at_most_one(SatLiteral lit1,
			   SatLiteral lit2)
{
  add_clause(~lit1, ~lit2);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_at_most_one(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  add_clause(~lit1, ~lit2);
  add_clause(~lit1, ~lit3);
  add_clause(~lit2, ~lit3);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_at_most_one(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  add_clause(~lit1, ~lit2);
  add_clause(~lit1, ~lit3);
  add_clause(~lit1, ~lit4);
  add_clause(~lit2, ~lit3);
  add_clause(~lit2, ~lit4);
  add_clause(~lit3, ~lit4);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatSolver::add_at_most_one(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4,
			   SatLiteral lit5)
{
  add_clause(~lit1, ~lit2);
  add_clause(~lit1, ~lit3);
  add_clause(~lit1, ~lit4);
  add_clause(~lit1, ~lit5);
  add_clause(~lit2, ~lit3);
  add_clause(~lit2, ~lit4);
  add_clause(~lit2, ~lit5);
  add_clause(~lit3, ~lit4);
  add_clause(~lit3, ~lit5);
  add_clause(~lit4, ~lit5);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatSolver::add_at_most_one(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4,
			   SatLiteral lit5,
			   SatLiteral lit6)
{
  add_clause(~lit1, ~lit2);
  add_clause(~lit1, ~lit3);
  add_clause(~lit1, ~lit4);
  add_clause(~lit1, ~lit5);
  add_clause(~lit1, ~lit6);
  add_clause(~lit2, ~lit3);
  add_clause(~lit2, ~lit4);
  add_clause(~lit2, ~lit5);
  add_clause(~lit2, ~lit6);
  add_clause(~lit3, ~lit4);
  add_clause(~lit3, ~lit5);
  add_clause(~lit3, ~lit6);
  add_clause(~lit4, ~lit5);
  add_clause(~lit4, ~lit6);
  add_clause(~lit5, ~lit6);
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_at_most_two(SatLiteral lit1,
			   SatLiteral lit2)
{
  // 無条件でなりたつ．
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_at_most_two(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3)
{
  add_clause(~lit1, ~lit2, ~lit3);
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_at_most_two(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4)
{
  add_clause(~lit1, ~lit2, ~lit3       );
  add_clause(~lit1, ~lit2       , ~lit4);
  add_clause(~lit1,        ~lit3, ~lit4);
  add_clause(       ~lit2, ~lit3, ~lit4);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatSolver::add_at_most_two(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4,
			   SatLiteral lit5)
{
  add_clause(~lit1, ~lit2, ~lit3              );
  add_clause(~lit1, ~lit2,        ~lit4       );
  add_clause(~lit1, ~lit2,               ~lit5);
  add_clause(~lit1,        ~lit3, ~lit4       );
  add_clause(~lit1,        ~lit3,        ~lit5);
  add_clause(~lit1,               ~lit4, ~lit5);
  add_clause(       ~lit2, ~lit3, ~lit4       );
  add_clause(       ~lit2, ~lit3,        ~lit5);
  add_clause(       ~lit2,        ~lit4, ~lit5);
  add_clause(              ~lit3, ~lit4, ~lit5);
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatSolver::add_at_most_two(SatLiteral lit1,
			   SatLiteral lit2,
			   SatLiteral lit3,
			   SatLiteral lit4,
			   SatLiteral lit5,
			   SatLiteral lit6)
{
  add_clause(~lit1, ~lit2, ~lit3                     );
  add_clause(~lit1, ~lit2,        ~lit4              );
  add_clause(~lit1, ~lit2,               ~lit5       );
  add_clause(~lit1, ~lit2,                      ~lit6);
  add_clause(~lit1,        ~lit3, ~lit4              );
  add_clause(~lit1,        ~lit3,        ~lit5       );
  add_clause(~lit1,        ~lit3,               ~lit6);
  add_clause(~lit1,               ~lit4, ~lit5       );
  add_clause(~lit1,               ~lit4,        ~lit6);
  add_clause(~lit1,                      ~lit5, ~lit6);
  add_clause(       ~lit2, ~lit3, ~lit4              );
  add_clause(       ~lit2, ~lit3,        ~lit5       );
  add_clause(       ~lit2, ~lit3,               ~lit6);
  add_clause(       ~lit2,        ~lit4, ~lit5       );
  add_clause(       ~lit2,        ~lit4,        ~lit6);
  add_clause(       ~lit2,               ~lit5, ~lit6);
  add_clause(              ~lit3, ~lit4, ~lit5       );
  add_clause(              ~lit3, ~lit4,        ~lit6);
  add_clause(              ~lit3,        ~lit5, ~lit6);
  add_clause(                     ~lit4, ~lit5, ~lit6);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_at_least_one(SatLiteral lit1,
			    SatLiteral lit2)
{
  add_clause( lit1,  lit2);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_at_least_one(SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3)
{
  add_clause( lit1,  lit2,  lit3);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_at_least_one(SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4)
{
  add_clause( lit1,  lit2,  lit3,  lit4);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatSolver::add_at_least_one(SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4,
			    SatLiteral lit5)
{
  add_clause( lit1,  lit2,  lit3,  lit4,  lit5);
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatSolver::add_at_least_one(SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4,
			    SatLiteral lit5,
			    SatLiteral lit6)
{
  add_clause( lit1,  lit2,  lit3,  lit4,  lit5,  lit6);
}

// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatSolver::add_at_least_one(const vector<SatLiteral>& lit_list)
{
  add_clause(lit_list);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_at_least_two(SatLiteral lit1,
			    SatLiteral lit2)
{
  add_clause(lit1);
  add_clause(lit2);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_at_least_two(SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3)
{
  // 2つ以上が False にならない．
  add_clause( lit1,  lit2       );
  add_clause( lit1,         lit3);
  add_clause(        lit2,  lit3);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_at_least_two(SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4)
{
  // 3つ以上が False にならない．
  add_clause( lit1,  lit2,  lit3       );
  add_clause( lit1,  lit2,         lit4);
  add_clause( lit1,         lit3,  lit4);
  add_clause(        lit2,  lit3,  lit4);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatSolver::add_at_least_two(SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4,
			    SatLiteral lit5)
{
  // 4つ以上が False にならない．
  add_clause( lit1,  lit2,  lit3,  lit4       );
  add_clause( lit1,  lit2,  lit3,         lit5);
  add_clause( lit1,  lit2,         lit4,  lit5);
  add_clause( lit1,         lit3,  lit4,  lit5);
  add_clause(        lit2,  lit3,  lit4,  lit5);
}

// @brief 与えられたリテラルのうち2つ以上が true になる条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatSolver::add_at_least_two(SatLiteral lit1,
			    SatLiteral lit2,
			    SatLiteral lit3,
			    SatLiteral lit4,
			    SatLiteral lit5,
			    SatLiteral lit6)
{
  // 5つ以上が False にならない．
  add_clause( lit1,  lit2,  lit3,  lit4,  lit5       );
  add_clause( lit1,  lit2,  lit3,  lit4,         lit6);
  add_clause( lit1,  lit2,  lit3,         lit5,  lit6);
  add_clause( lit1,  lit2,         lit4,  lit5,  lit6);
  add_clause( lit1,         lit3,  lit4,  lit5,  lit6);
  add_clause(        lit2,  lit3,  lit4,  lit5,  lit6);
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_exact_one(SatLiteral lit1,
			 SatLiteral lit2)
{
  add_at_most_one(lit1, lit2);
  add_at_least_one(lit1, lit2);
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_exact_one(SatLiteral lit1,
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
SatSolver::add_exact_one(SatLiteral lit1,
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
SatSolver::add_exact_one(SatLiteral lit1,
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
SatSolver::add_exact_one(SatLiteral lit1,
			 SatLiteral lit2,
			 SatLiteral lit3,
			 SatLiteral lit4,
			 SatLiteral lit5,
			 SatLiteral lit6)
{
  add_at_most_one(lit1, lit2, lit3, lit4, lit5, lit6);
  add_at_least_one(lit1, lit2, lit3, lit4, lit5, lit6);
}

// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatSolver::add_exact_one(const vector<SatLiteral>& lit_list)
{
  add_at_most_one(lit_list);
  add_at_least_one(lit_list);
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_exact_two(SatLiteral lit1,
			 SatLiteral lit2)
{
  add_at_most_two(lit1, lit2);
  add_at_least_two(lit1, lit2);
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_exact_two(SatLiteral lit1,
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
SatSolver::add_exact_two(SatLiteral lit1,
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
SatSolver::add_exact_two(SatLiteral lit1,
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
SatSolver::add_exact_two(SatLiteral lit1,
			 SatLiteral lit2,
			 SatLiteral lit3,
			 SatLiteral lit4,
			 SatLiteral lit5,
			 SatLiteral lit6)
{
  add_at_most_two(lit1, lit2, lit3, lit4, lit5, lit6);
  add_at_least_two(lit1, lit2, lit3, lit4, lit5, lit6);
}

// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
inline
void
SatSolver::add_exact_two(const vector<SatLiteral>& lit_list)
{
  add_at_most_two(lit_list);
  add_at_least_two(lit_list);
}

// @brief 与えられたリテラルのうち厳密にk個が true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
inline
void
SatSolver::add_exact_k(const vector<SatLiteral>& lit_list,
		       int k)
{
  add_at_most_k(lit_list, k);
  add_at_least_k(lit_list, k);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2 入力のリテラル
inline
void
SatSolver::add_not_one(SatLiteral lit1,
		       SatLiteral lit2)
{
  add_clause( lit1, ~lit2);
  add_clause(~lit1,  lit2);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2, lit3 入力のリテラル
inline
void
SatSolver::add_not_one(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3)
{
  add_clause( lit1,  lit2, ~lit3);
  add_clause( lit1, ~lit2,  lit3);
  add_clause(~lit1,  lit2,  lit3);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4 入力のリテラル
inline
void
SatSolver::add_not_one(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3,
		       SatLiteral lit4)
{
  add_clause( lit1,  lit2,  lit3, ~lit4);
  add_clause( lit1,  lit2, ~lit3,  lit4);
  add_clause( lit1, ~lit2,  lit3,  lit4);
  add_clause(~lit1,  lit2,  lit3,  lit4);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5 入力のリテラル
inline
void
SatSolver::add_not_one(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3,
		       SatLiteral lit4,
		       SatLiteral lit5)
{
  add_clause( lit1,  lit2,  lit3,  lit4, ~lit5);
  add_clause( lit1,  lit2,  lit3, ~lit4,  lit5);
  add_clause( lit1,  lit2, ~lit3,  lit4,  lit5);
  add_clause( lit1, ~lit2,  lit3,  lit4,  lit5);
  add_clause(~lit1,  lit2,  lit3,  lit4,  lit5);
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit1, lit2, lit3, lit4, lit5, lit6 入力のリテラル
inline
void
SatSolver::add_not_one(SatLiteral lit1,
		       SatLiteral lit2,
		       SatLiteral lit3,
		       SatLiteral lit4,
		       SatLiteral lit5,
		       SatLiteral lit6)
{
  add_clause( lit1,  lit2,  lit3,  lit4,  lit5, ~lit6);
  add_clause( lit1,  lit2,  lit3,  lit4, ~lit5,  lit6);
  add_clause( lit1,  lit2,  lit3, ~lit4,  lit5,  lit6);
  add_clause( lit1,  lit2, ~lit3,  lit4,  lit5,  lit6);
  add_clause( lit1, ~lit2,  lit3,  lit4,  lit5,  lit6);
  add_clause(~lit1,  lit2,  lit3,  lit4,  lit5,  lit6);
}

// @brief SAT 問題を解く．
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @retval kB3True 充足した．
// @retval kB3False 充足不能が判明した．
// @retval kB3X わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
inline
SatBool3
SatSolver::solve(vector<SatBool3>& model)
{
  // 空の assumptions を付けて solve() を呼ぶだけ
  return solve(vector<SatLiteral>(), model);
}

// @brief assumption 付きの SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @retval kSat 充足した．
// @retval kUnsat 充足不能が判明した．
// @retval kUndet わからなかった．
//
// i 番めの変数の割り当て結果は model[i] に入る．
inline
SatBool3
SatSolver::solve(const vector<SatLiteral>& assumptions,
		 vector<SatBool3>& model)
{
  // conflicts 用のダミー配列
  vector<SatLiteral> dummy;
  return solve(assumptions, model, dummy);
}

END_NAMESPACE_YM_SAT

#endif // YM_SATSOLVER_H
