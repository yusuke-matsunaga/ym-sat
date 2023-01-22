#ifndef YM_SATSOLVER_H
#define YM_SATSOLVER_H

/// @file ym/SatSolver.h
/// @brief SatSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2017, 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"
#include "ym/SatModel.h"
#include "ym/SatSolverType.h"
#include "ym/SatStats.h"


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
  ///
  /// @sa SatSolverType
  SatSolver(
    const SatSolverType& solver_type = SatSolverType{} ///< [in] 実装タイプ
  );

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
  /// @return 新しい変数を表すリテラルを返す．
  ///
  /// * 変数番号は 0 から始まる．
  /// * lingeling においては decision = true のリテラルは freeze される．
  SatLiteral
  new_variable(
    bool decision = true ///< [in] 決定変数の時に true とする．
  );

  /// @brief 条件リテラルを設定する．
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(
    const vector<SatLiteral>& lit_list ///< [in] 条件リテラルのリスト
  )
  {
    mConditionalLits = lit_list;
  }

  /// @brief 条件リテラルを設定する．
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(
    SatLiteral lit1 ///< [in] 条件リテラル
  )
  {
    _set_conditional_literals(1, &lit1);
  }

  /// @brief 条件リテラルを設定する．
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(
    SatLiteral lit1, ///< [in] 条件リテラル1
    SatLiteral lit2  ///< [in] 条件リテラル2
  )
  {
    SatLiteral tmp_lits[] = {lit1, lit2};

    _set_conditional_literals(2, tmp_lits);
  }

  /// @brief 条件リテラルを設定する．
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(
    SatLiteral lit1, ///< [in] 条件リテラル1
    SatLiteral lit2, ///< [in] 条件リテラル2
    SatLiteral lit3  ///< [in] 条件リテラル3  );
  )
  {
    SatLiteral tmp_lits[] = {lit1, lit2, lit3};

    _set_conditional_literals(3, tmp_lits);
  }

  /// @brief 条件リテラルを設定する．
  ///
  /// 以降の add_clause() にはこのリテラルの否定が追加される．
  /// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
  void
  set_conditional_literals(
    SatLiteral lit1, ///< [in] 条件リテラル1
    SatLiteral lit2, ///< [in] 条件リテラル2
    SatLiteral lit3, ///< [in] 条件リテラル3
    SatLiteral lit4  ///< [in] 条件リテラル4
  )
  {
    SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4};

    _set_conditional_literals(4, tmp_lits);
  }

  /// @brief 条件リテラルをクリアする．
  void
  clear_conditional_literals()
  {
    mConditionalLits.clear();
  }

  /// @brief 節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] リテラルのリスト
  )
  {
    _add_clause(lits);
  }

  /// @brief 1項の節(リテラル)を追加する．
  void
  add_clause(
    SatLiteral lit1
  )
  {
    _add_clause(1, &lit1);
  }

  /// @brief 2項の節を追加する．
  void
  add_clause(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    SatLiteral tmp_lits[] = {lit1, lit2};

    _add_clause(2, tmp_lits);
  }

  /// @brief 3項の節を追加する．
  void
  add_clause(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3
  )
  {
    SatLiteral tmp_lits[] = {lit1, lit2, lit3};

    _add_clause(3, tmp_lits);
  }

  /// @brief 4項の節を追加する．
  void
  add_clause(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4
  )
  {
    SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4};

    _add_clause(4, tmp_lits);
  }

  /// @brief 5項の節を追加する．
  void
  add_clause(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5
  )
  {
    SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4, lit5};

    _add_clause(5, tmp_lits);
  }

  /// @brief 6項の節を追加する．
  void
  add_clause(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5,
    SatLiteral lit6
  )
  {
    SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4, lit5, lit6};

    _add_clause(6, tmp_lits);
  }

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name Tseitin's encoding を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief 2つのリテラルが等しいという条件を追加する．
  ///
  /// 具体的には (~lit1 + lit2)(lit1 + ~lit2) の２つの節を追加する．
  void
  add_buffgate(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    add_clause(~lit1,  lit2);
    add_clause( lit1, ~lit2);
  }

  /// @brief 2つのリテラルが等しくないという条件を追加する．
  ///
  /// 具体的には (~lit1 + ~lit2)(lit1 + lit2) の２つの節を追加する．
  void
  add_notgate(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    add_buffgate(~lit1, lit2);
  }

  /// @brief 2入力ANDゲートの入出力の関係を表す条件を追加する．
  void
  add_andgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2  ///< [in] 入力のリテラル2
  )
  {
    add_clause(~lit1, ~lit2,  olit);
    add_clause( lit1,        ~olit);
    add_clause(        lit2, ~olit);
  }

  /// @brief 3入力ANDゲートの入出力の関係を表す条件を追加する．
  void
  add_andgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3  ///< [in] 入力のリテラル3
  )
  {
    add_clause(~lit1, ~lit2, ~lit3,  olit);
    add_clause( lit1,               ~olit);
    add_clause(        lit2,        ~olit);
    add_clause(               lit3, ~olit);
  }

  /// @brief 4入力ANDゲートの入出力の関係を表す条件を追加する．
  void
  add_andgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3, ///< [in] 入力のリテラル3
    SatLiteral lit4  ///< [in] 入力のリテラル4
  )
  {
    add_clause(~lit1, ~lit2, ~lit3, ~lit4,  olit);
    add_clause( lit1,                      ~olit);
    add_clause(        lit2,               ~olit);
    add_clause(               lit3,        ~olit);
    add_clause(                      lit4, ~olit);
  }

  /// @brief n入力ANDゲートの入出力の関係を表す条件を追加する．
  void
  add_andgate(
    SatLiteral olit,                   ///< [in] 出力のリテラル
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  );

  /// @brief 2入力NANDゲートの入出力の関係を表す条件を追加する．
  void
  add_nandgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2  ///< [in] 入力のリテラル2
  )
  {
    add_andgate(~olit, lit1, lit2);
  }

  /// @brief 3入力NANDゲートの入出力の関係を表す条件を追加する．
  void
  add_nandgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3  ///< [in] 入力のリテラル3
  )
  {
    add_andgate(~olit, lit1, lit2, lit3);
  }

  /// @brief 4入力NANDゲートの入出力の関係を表す条件を追加する．
  void
  add_nandgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3, ///< [in] 入力のリテラル3
    SatLiteral lit4  ///< [in] 入力のリテラル4
  )
  {
    add_andgate(~olit, lit1, lit2, lit3, lit4);
  }

  /// @brief n入力NANDゲートの入出力の関係を表す条件を追加する．
  void
  add_nandgate(
    SatLiteral olit,                    ///< [in] 出力のリテラル
    const vector<SatLiteral>& lit_list  ///< [in] 入力のリテラルのリスト
  )
  {
    add_andgate(~olit, lit_list);
  }

  /// @brief 2入力ORゲートの入出力の関係を表す条件を追加する．
  void
  add_orgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2  ///< [in] 入力のリテラル2
  )
  {
    add_clause( lit1,  lit2, ~olit);
    add_clause(~lit1,         olit);
    add_clause(       ~lit2,  olit);
  }

  /// @brief 3入力ORゲートの入出力の関係を表す条件を追加する．
  void
  add_orgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3  ///< [in] 入力のリテラル3
  )
  {
    add_clause( lit1,  lit2,  lit3, ~olit);
    add_clause(~lit1,                olit);
    add_clause(       ~lit2,         olit);
    add_clause(              ~lit3,  olit);
  }

  /// @brief 4入力ORゲートの入出力の関係を表す条件を追加する．
  void
  add_orgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3, ///< [in] 入力のリテラル3
    SatLiteral lit4  ///< [in] 入力のリテラル4
  )
  {
    add_clause( lit1,  lit2,  lit3,  lit4, ~olit);
    add_clause(~lit1,                       olit);
    add_clause(       ~lit2,                olit);
    add_clause(              ~lit3,         olit);
    add_clause(                     ~lit4,  olit);
  }

  /// @brief n入力ORゲートの入出力の関係を表す条件を追加する．
  void
  add_orgate(
    SatLiteral olit,                   ///< [in] 出力のリテラル
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  );

  /// @brief 2入力NORゲートの入出力の関係を表す条件を追加する．
  void
  add_norgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2  ///< [in] 入力のリテラル2
  )
  {
    add_orgate(~olit, lit1, lit2);
  }

  /// @brief 3入力NORゲートの入出力の関係を表す条件を追加する．
  void
  add_norgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3  ///< [in] 入力のリテラル3
  )
  {
    add_orgate(~olit, lit1, lit2, lit3);
  }

  /// @brief 4入力NORゲートの入出力の関係を表す条件を追加する．
  void
  add_norgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3, ///< [in] 入力のリテラル3
    SatLiteral lit4  ///< [in] 入力のリテラル4
  )
  {
    add_orgate(~olit, lit1, lit2, lit3, lit4);
  }

  /// @brief n入力NORゲートの入出力の関係を表す条件を追加する．
  void
  add_norgate(
    SatLiteral olit,                   ///< [in] 出力のリテラル
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  )
  {
    add_orgate(~olit, lit_list);
  }

  /// @brief 2入力XORゲートの入出力の関係を表す条件を追加する．
  void
  add_xorgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2  ///< [in] 入力のリテラル2
  )
  {
    add_clause( lit1,  lit2, ~olit);
    add_clause( lit1, ~lit2,  olit);
    add_clause(~lit1,  lit2,  olit);
    add_clause(~lit1, ~lit2, ~olit);
  }

  /// @brief 3入力XORゲートの入出力の関係を表す条件を追加する．
  void
  add_xorgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3  ///< [in] 入力のリテラル3
  )
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

  /// @brief 4入力XORゲートの入出力の関係を表す条件を追加する．
  void
  add_xorgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3, ///< [in] 入力のリテラル3
    SatLiteral lit4  ///< [in] 入力のリテラル4
  )
  {
    _add_xorgate_sub(olit, vector<SatLiteral>{lit1, lit2, lit3, lit4}, 0, 4);
  }

  /// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
  void
  add_xorgate(
    SatLiteral olit,                   ///< [in] 出力のリテラル
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  )
  {
    SizeType n = lit_list.size();
    _add_xorgate_sub(olit, lit_list, 0, n);
  }

  /// @brief 2入力XNORゲートの入出力の関係を表す条件を追加する．
  void
  add_xnorgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2  ///< [in] 入力のリテラル2
  )
  {
    add_xorgate(~olit, lit1, lit2);
  }

  /// @brief 3入力XNORゲートの入出力の関係を表す条件を追加する．
  void
  add_xnorgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3  ///< [in] 入力のリテラル3
  )
  {
    add_xorgate(~olit, lit1, lit2, lit3);
  }

  /// @brief 4入力XORゲートの入出力の関係を表す条件を追加する．
  void
  add_xnorgate(
    SatLiteral olit, ///< [in] 出力のリテラル
    SatLiteral lit1, ///< [in] 入力のリテラル1
    SatLiteral lit2, ///< [in] 入力のリテラル2
    SatLiteral lit3, ///< [in] 入力のリテラル3
    SatLiteral lit4  ///< [in] 入力のリテラル4
  )
  {
    add_xorgate(~olit, lit1, lit2, lit3, lit4);
  }

  /// @brief n入力XNORゲートの入出力の関係を表す条件を追加する．
  void
  add_xnorgate(
    SatLiteral olit,                   ///< [in] 出力のリテラル
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  )
  {
    add_xorgate(~olit, lit_list);
  }

  /// @brief half_adder の入出力の関係を表す条件を追加する．
  void
  add_half_adder(
    SatLiteral alit, ///< [in] 入力Aのリテラル
    SatLiteral blit, ///< [in] 入力Bのリテラル
    SatLiteral slit, ///< [in] 和の出力のリテラル
    SatLiteral olit  ///< [in] キャリー出力のリテラル
  );

  /// @brief full_adder の入出力の関係を表す条件を追加する．
  void
  add_full_adder(
    SatLiteral alit, ///< [in] 入力Aのリテラル
    SatLiteral blit, ///< [in] 入力Bのリテラル
    SatLiteral ilit, ///< [in] キャリー入力のリテラル
    SatLiteral slit, ///< [in] 和の出力のリテラル
    SatLiteral olit  ///< [in] キャリー出力のリテラル
  );

  /// @brief 多ビットadderの入出力の関係を表す条件を追加する．
  ///
  /// * alits, blits のサイズ <= slits のサイズでなければならない．
  /// * 片方が短い場合には上位ビットに0を仮定する．
  void
  add_adder(
    const vector<SatLiteral>& alits, ///< [in] 入力Aのリテラルのリスト
    const vector<SatLiteral>& blits, ///< [in] 入力Bのリテラルのリスト
    SatLiteral ilit,                 ///< [in] キャリー入力のリテラル
    const vector<SatLiteral>& slits, ///< [in] 出力のリテラルのリスト
    SatLiteral olit                  ///< [in] キャリー出力のリテラル
  );

  /// @brief 1's counter の入出力の関係を表す条件を追加する．
  /// @return 個数を表す2進数を表すリテラルのリストを返す．
  vector<SatLiteral>
  add_counter(
    const vector<SatLiteral>& ilits ///< [in] 入力のリテラルのリスト
  );

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  void
  add_at_most_one(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    add_clause(~lit1, ~lit2);
  }

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  void
  add_at_most_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3
  )
  {
    add_clause(~lit1, ~lit2);
    add_clause(~lit1, ~lit3);
    add_clause(~lit2, ~lit3);
  }

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  void
  add_at_most_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4
  )
  {
    add_clause(~lit1, ~lit2);
    add_clause(~lit1, ~lit3);
    add_clause(~lit1, ~lit4);
    add_clause(~lit2, ~lit3);
    add_clause(~lit2, ~lit4);
    add_clause(~lit3, ~lit4);
  }

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  void
  add_at_most_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5
  )
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

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  void
  add_at_most_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5,
    SatLiteral lit6
  )
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

  /// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
  void
  add_at_most_one(
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  );

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  void
  add_at_most_two(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    // 無条件でなりたつ．
  }

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  void
  add_at_most_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3
  )
  {
    add_clause(~lit1, ~lit2, ~lit3);
  }

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  void
  add_at_most_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4
  )
  {
    add_clause(~lit1, ~lit2, ~lit3       );
    add_clause(~lit1, ~lit2       , ~lit4);
    add_clause(~lit1,        ~lit3, ~lit4);
    add_clause(       ~lit2, ~lit3, ~lit4);
  }

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  void
  add_at_most_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5
  )
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

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  void
  add_at_most_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5,
    SatLiteral lit6
  )
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

  /// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
  void
  add_at_most_two(
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  );

  /// @brief 与えられたリテラルのうちk個しか true にならない条件を追加する．
  void
  add_at_most_k(
    const vector<SatLiteral>& lit_list, ///< [in] 入力のリテラルのリスト
    SizeType k                          ///< [in] しきい値
  );

  /// @brief 与えられたリテラルのうち1以上は true になる条件を追加する．
  void
  add_at_least_one(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    add_clause( lit1,  lit2);
  }

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  void
  add_at_least_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3
  )
  {
    add_clause( lit1,  lit2,  lit3);
  }

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  void
  add_at_least_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4
  )
  {
    add_clause( lit1,  lit2,  lit3,  lit4);
  }

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  void
  add_at_least_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5
  )
  {
    add_clause( lit1,  lit2,  lit3,  lit4,  lit5);
  }

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  void
  add_at_least_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5,
    SatLiteral lit6
  )
  {
    add_clause( lit1,  lit2,  lit3,  lit4,  lit5,  lit6);
  }

  /// @brief 与えられたリテラルのうち1つ以上は true になる条件を追加する．
  void
  add_at_least_one(
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  )
  {
    add_clause(lit_list);
  }

  /// @brief 与えられたリテラルのうち2以上は true になる条件を追加する．
  void
  add_at_least_two(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    add_clause(lit1);
    add_clause(lit2);
  }

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  void
  add_at_least_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3
  )
  {
    // 2つ以上が False にならない．
    add_clause( lit1,  lit2       );
    add_clause( lit1,         lit3);
    add_clause(        lit2,  lit3);
  }

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  void
  add_at_least_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4
  )
  {
    // 3つ以上が False にならない．
    add_clause( lit1,  lit2,  lit3       );
    add_clause( lit1,  lit2,         lit4);
    add_clause( lit1,         lit3,  lit4);
    add_clause(        lit2,  lit3,  lit4);
  }

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  void
  add_at_least_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5
  )
  {
    // 4つ以上が False にならない．
    add_clause( lit1,  lit2,  lit3,  lit4       );
    add_clause( lit1,  lit2,  lit3,         lit5);
    add_clause( lit1,  lit2,         lit4,  lit5);
    add_clause( lit1,         lit3,  lit4,  lit5);
    add_clause(        lit2,  lit3,  lit4,  lit5);
  }

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  void
  add_at_least_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5,
    SatLiteral lit6
  )
  {
    // 5つ以上が False にならない．
    add_clause( lit1,  lit2,  lit3,  lit4,  lit5       );
    add_clause( lit1,  lit2,  lit3,  lit4,         lit6);
    add_clause( lit1,  lit2,  lit3,         lit5,  lit6);
    add_clause( lit1,  lit2,         lit4,  lit5,  lit6);
    add_clause( lit1,         lit3,  lit4,  lit5,  lit6);
    add_clause(        lit2,  lit3,  lit4,  lit5,  lit6);
  }

  /// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
  void
  add_at_least_two(
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  );

  /// @brief 与えられたリテラルのうちk個以上は true になる条件を追加する．
  void
  add_at_least_k(
    const vector<SatLiteral>& lit_list, ///< [in] 入力のリテラルのリスト
    SizeType k                          ///< [in] しきい値
  );

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  void
  add_exact_one(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    add_at_most_one(lit1, lit2);
    add_at_least_one(lit1, lit2);
  }

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  void
  add_exact_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3
  )
  {
    add_at_most_one(lit1, lit2, lit3);
    add_at_least_one(lit1, lit2, lit3);
  }

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  void
  add_exact_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4
  )
  {
    add_at_most_one(lit1, lit2, lit3, lit4);
    add_at_least_one(lit1, lit2, lit3, lit4);
  }

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  void
  add_exact_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5
  )
  {
    add_at_most_one(lit1, lit2, lit3, lit4, lit5);
    add_at_least_one(lit1, lit2, lit3, lit4, lit5);
  }

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  void
  add_exact_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5,
    SatLiteral lit6
  )
  {
    add_at_most_one(lit1, lit2, lit3, lit4, lit5, lit6);
    add_at_least_one(lit1, lit2, lit3, lit4, lit5, lit6);
  }

  /// @brief 与えられたリテラルのうち厳密に1つが true になる条件を追加する．
  void
  add_exact_one(
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  );

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  void
  add_exact_two(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    add_at_most_two(lit1, lit2);
    add_at_least_two(lit1, lit2);
  }

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  void
  add_exact_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3
  )
  {
    add_at_most_two(lit1, lit2, lit3);
    add_at_least_two(lit1, lit2, lit3);
  }

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  void
  add_exact_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4
  )
  {
    add_at_most_two(lit1, lit2, lit3, lit4);
    add_at_least_two(lit1, lit2, lit3, lit4);
  }

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  void
  add_exact_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5
  )
  {
    add_at_most_two(lit1, lit2, lit3, lit4, lit5);
    add_at_least_two(lit1, lit2, lit3, lit4, lit5);
  }

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  void
  add_exact_two(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5,
    SatLiteral lit6
  )
  {
    add_at_most_two(lit1, lit2, lit3, lit4, lit5, lit6);
    add_at_least_two(lit1, lit2, lit3, lit4, lit5, lit6);
  }

  /// @brief 与えられたリテラルのうち厳密に2つが true になる条件を追加する．
  void
  add_exact_two(
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  );

  /// @brief 与えられたリテラルのうち厳密にk個が true になる条件を追加する．
  void
  add_exact_k(
    const vector<SatLiteral>& lit_list, ///< [in] 入力のリテラルのリスト
    SizeType k                          ///< [in] しきい値
  )
  {
    add_at_most_k(lit_list, k);
    add_at_least_k(lit_list, k);
  }

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  void
  add_not_one(
    SatLiteral lit1,
    SatLiteral lit2
  )
  {
    add_clause( lit1, ~lit2);
    add_clause(~lit1,  lit2);
  }

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  void
  add_not_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3
  )
  {
    add_clause( lit1,  lit2, ~lit3);
    add_clause( lit1, ~lit2,  lit3);
    add_clause(~lit1,  lit2,  lit3);
  }

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  void
  add_not_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4
  )
  {
    add_clause( lit1,  lit2,  lit3, ~lit4);
    add_clause( lit1,  lit2, ~lit3,  lit4);
    add_clause( lit1, ~lit2,  lit3,  lit4);
    add_clause(~lit1,  lit2,  lit3,  lit4);
  }

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  void
  add_not_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5
  )
  {
    add_clause( lit1,  lit2,  lit3,  lit4, ~lit5);
    add_clause( lit1,  lit2,  lit3, ~lit4,  lit5);
    add_clause( lit1,  lit2, ~lit3,  lit4,  lit5);
    add_clause( lit1, ~lit2,  lit3,  lit4,  lit5);
    add_clause(~lit1,  lit2,  lit3,  lit4,  lit5);
  }

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  void
  add_not_one(
    SatLiteral lit1,
    SatLiteral lit2,
    SatLiteral lit3,
    SatLiteral lit4,
    SatLiteral lit5,
    SatLiteral lit6
  )
  {
    add_clause( lit1,  lit2,  lit3,  lit4,  lit5, ~lit6);
    add_clause( lit1,  lit2,  lit3,  lit4, ~lit5,  lit6);
    add_clause( lit1,  lit2,  lit3, ~lit4,  lit5,  lit6);
    add_clause( lit1,  lit2, ~lit3,  lit4,  lit5,  lit6);
    add_clause( lit1, ~lit2,  lit3,  lit4,  lit5,  lit6);
    add_clause(~lit1,  lit2,  lit3,  lit4,  lit5,  lit6);
  }

  /// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
  void
  add_not_one(
    const vector<SatLiteral>& lit_list ///< [in] 入力のリテラルのリスト
  );

  /// @brief A == B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_eq(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  );

  /// @brief A == B という条件を追加する．
  void
  add_eq(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val                        ///< [in] Bの値
  );

  /// @brief A != B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_ne(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  );

  /// @brief A != B という条件を追加する．
  void
  add_ne(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  /// @brief A < B という条件を追加する．
  /// @param[in] a_vec, b_vec A,Bを表すビットベクタ
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_lt(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  );

  /// @brief A < B という条件を追加する．
  void
  add_lt(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  /// @brief A <= B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_le(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  );

  /// @brief A <= B という条件を追加する．
  void
  add_le(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  /// @brief A > B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_gt(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  )
  {
    add_lt(b_vec, a_vec);
  }

  /// @brief A > B という条件を追加する．
  void
  add_gt(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  /// @brief A >= B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_ge(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  )
  {
    add_le(b_vec, a_vec);
  }

  /// @brief A >= B という条件を追加する．
  void
  add_ge(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 動作制御を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief SAT 問題を解く．
  /// @return 結果(SatBool3)を返す．
  ///
  /// 結果の意味は以下の通り
  /// * kB3True  充足した．
  /// * kB3False 充足不能が判明した．
  /// * kB3X     わからなかった．
  SatBool3
  solve(
    int time_limit = 0 ///< [in] 時間制約(秒) 0 で制約なし
  )
  {
    // 空の assumptions を付けて solve() を呼ぶだけ
    return solve({}, time_limit);
  }

  /// @brief assumption 付きの SAT 問題を解く．
  /// @return 結果(SatBool3)を返す．
  ///
  /// 結果の意味は以下の通り
  /// * SatBool3::True  充足した．
  /// * SatBool3::False 充足不能が判明した．
  /// * SatBool3::X     わからなかった．
  SatBool3
  solve(
    const vector<SatLiteral>& assumptions, ///< [in] あらかじめ仮定する変数の値割り当てリスト
    int time_limit = 0                     ///< [in] 時間制約(秒) 0 で制約なし
  );

  /// @brief 直前に解いた問題のモデルを返す．
  const SatModel&
  model()
  {
    return mModel;
  }

  /// @brief 直前に解いた問題のサイズを返す．
  SizeType
  model_size()
  {
    return mModel.size();
  }

  /// @brief 直前に解いた問題の値を返す．
  SatBool3
  read_model(
    SatLiteral lit ///< [in] リテラル
  )
  {
    return mModel.get(lit);
  }

  /// @brief 直前の問題の矛盾の原因のリテラルを返す．
  const vector<SatLiteral>&
  conflict_literals()
  {
    return mConflictLiterals;
  }

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  void
  stop();

  /// @brief 時間計測機能を制御する
  void
  timer_on(
    bool enable
  );

  /// @brief conflict_limit の最大値
  /// @return 以前の設定値を返す．
  SizeType
  set_max_conflict(
    SizeType val ///< [in] 設定する値
  );

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  void
  reg_msg_handler(
    SatMsgHandler* msg_handler ///< [in] 登録するメッセージハンドラ
  );

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  /// @name 内部状態の取得を行う関数
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief SolverType を取り出す．
  SatSolverType
  type() const
  {
    return mType;
  }

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const;

  /// @brief 現在の内部状態を得る．
  SatStats
  get_stats() const;

  /// @brief 変数の数を得る．
  SizeType
  variable_num() const
  {
    return mVariableNum;
  }

  /// @brief 制約節の数を得る．
  SizeType
  clause_num() const
  {
    return mClauseList.size();
  }

  /// @brief 制約節のリテラルの総数を得る．
  SizeType
  literal_num() const
  {
    return mLiteralNum;
  }

  /// @brief DIMACS 形式で制約節を出力する．
  void
  write_DIMACS(
    ostream& s ///< [in] 出力先のストリーム
  ) const;

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////

private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief set_conditional_literals() の下請け関数
  void
  _set_conditional_literals(
    SizeType n,       ///< [in] conditional_literal の数
    SatLiteral lits[] ///< [in] conditional_literal の配列
  );

  /// @brief add_clause() の下請け関数
  void
  _add_clause(
    const vector<SatLiteral>& lits ///< [in] リテラルのリスト
  );

  /// @brief add_clause() の下請け関数
  void
  _add_clause(
    SizeType n,       ///< [in] リテラル数
    SatLiteral lits[] ///< [in] リテラルの配列
  );

  /// @brief _add_clause() の下請け関数
  void
  _add_clause_sub(
    const vector<SatLiteral>& lits ///< [in] リテラルのリスト
  );

  /// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
  void
  _add_xorgate_sub(
    SatLiteral olit,                    ///< [in] 出力のリテラル
    const vector<SatLiteral>& lit_list, ///< [in] 入力のリテラルのリスト
    SizeType start,                     ///< [in] 開始位置
    SizeType num                        ///< [in] 要素数
  );

  /// @brief add_at_most_one() の下請け関数
  void
  _add_at_most_one(
    const vector<SatLiteral>& lit_list,
    SatLiteral olit
  );

  /// @brief add_at_most_two() の下請け関数
  void
  _add_at_most_two(
    const vector<SatLiteral>& lit_list,
    SatLiteral olit1,
    SatLiteral olit0
  );

  /// @brief add_at_least_two() の下請け関数
  void
  _add_at_least_two(
    const vector<SatLiteral>& lit_list,
    SatLiteral olit1,
    SatLiteral olit0
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ソルバの型
  SatSolverType mType;

  // 実際の機能を実装しているクラス
  unique_ptr<SatSolverImpl> mImpl;

  // ロガー
  unique_ptr<SatLogger> mLogger;

  // 条件リテラル
  vector<SatLiteral> mConditionalLits;

  // 直前の問題のモデル
  SatModel mModel;

  // 直前の矛盾の原因
  vector<SatLiteral> mConflictLiterals;

  // 変数の数(リポート用)
  SizeType mVariableNum{0};

  // 節のリスト(リポート用)
  vector<vector<SatLiteral>> mClauseList;

  // リテラル数(リポート用)
  SizeType mLiteralNum{0};

};

END_NAMESPACE_YM_SAT

#endif // YM_SATSOLVER_H
