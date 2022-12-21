#ifndef SATTSEITINENC_H
#define SATTSEITINENC_H

/// @file SatTseitinEnc.h
/// @brief SatTseitinEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
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
  SatTseitinEnc(
    SatSolver& solver ///< [in] SATソルバ
  );

  /// @brief デストラクタ
  ~SatTseitinEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
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
    mSolver.add_clause(~lit1,  lit2);
    mSolver.add_clause( lit1, ~lit2);
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
    mSolver.add_clause(~lit1, ~lit2,  olit);
    mSolver.add_clause( lit1,        ~olit);
    mSolver.add_clause(        lit2, ~olit);
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
    mSolver.add_clause(~lit1, ~lit2, ~lit3,  olit);
    mSolver.add_clause( lit1,               ~olit);
    mSolver.add_clause(        lit2,        ~olit);
    mSolver.add_clause(               lit3, ~olit);
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
    mSolver.add_clause(~lit1, ~lit2, ~lit3, ~lit4,  olit);
    mSolver.add_clause( lit1,                      ~olit);
    mSolver.add_clause(        lit2,               ~olit);
    mSolver.add_clause(               lit3,        ~olit);
    mSolver.add_clause(                      lit4, ~olit);
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
    mSolver.add_clause( lit1,  lit2, ~olit);
    mSolver.add_clause(~lit1,         olit);
    mSolver.add_clause(       ~lit2,  olit);
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
    mSolver.add_clause( lit1,  lit2,  lit3, ~olit);
    mSolver.add_clause(~lit1,                olit);
    mSolver.add_clause(       ~lit2,         olit);
    mSolver.add_clause(              ~lit3,  olit);
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
    mSolver.add_clause( lit1,  lit2,  lit3,  lit4, ~olit);
    mSolver.add_clause(~lit1,                       olit);
    mSolver.add_clause(       ~lit2,                olit);
    mSolver.add_clause(              ~lit3,         olit);
    mSolver.add_clause(                     ~lit4,  olit);
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
    mSolver.add_clause( lit1,  lit2, ~olit);
    mSolver.add_clause( lit1, ~lit2,  olit);
    mSolver.add_clause(~lit1,  lit2,  olit);
    mSolver.add_clause(~lit1, ~lit2, ~olit);
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
    mSolver.add_clause( lit1,  lit2,  lit3, ~olit);
    mSolver.add_clause( lit1,  lit2, ~lit3,  olit);
    mSolver.add_clause( lit1, ~lit2,  lit3,  olit);
    mSolver.add_clause( lit1, ~lit2, ~lit3, ~olit);
    mSolver.add_clause(~lit1,  lit2,  lit3,  olit);
    mSolver.add_clause(~lit1,  lit2, ~lit3, ~olit);
    mSolver.add_clause(~lit1, ~lit2,  lit3, ~olit);
    mSolver.add_clause(~lit1, ~lit2, ~lit3,  olit);
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


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
  void
  _add_xorgate_sub(
    SatLiteral olit,                    ///< [in] 出力のリテラル
    const vector<SatLiteral>& lit_list, ///< [in] 入力のリテラルのリスト
    SizeType start,                     ///< [in] 開始位置
    SizeType num                        ///< [in] 要素数
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

};

END_NAMESPACE_YM_SAT

#endif // SATTSEITINENC_H
