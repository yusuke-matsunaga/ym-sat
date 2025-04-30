#ifndef SABASE_H
#define SABASE_H

/// @file SaBase.h
/// @brief SaBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Analyzer.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SaBase SaBase.h "SaBase.h"
/// @brief Analyzer のベース実装クラス
///
/// 継承クラスに共通な実装を提供するためのクラス
//////////////////////////////////////////////////////////////////////
class SaBase :
  public Analyzer
{
public:

  /// @brief コンストラクタ
  SaBase(
    SatCore& core ///< [in] コアマネージャ
  );

  /// @brief デストラクタ
  ~SaBase();


public:
  //////////////////////////////////////////////////////////////////////
  // SatAnalyer の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 新しい変数が追加されたときに呼ばれる仮想関数
  void
  alloc_var(
    SizeType size
  ) override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスのための実装関数
  //////////////////////////////////////////////////////////////////////

  /// @brief lit_list を極小セパレータにする．
  ///
  /// lit_list から lit_list の他のリテラルの割り当て結果によって
  /// 割り当てられたリテラルを取り除く．
  void
  make_minimal(
    vector<Literal>& lit_list ///< [inout] リテラルのリスト
  );

  /// @brief リテラルの並び替えを行う．
  /// @return 2番めのリテラル (lit_list[1]) のレベルを返す．
  ///
  /// lit_list[0] は不変で，lit_list[1] 以降のリテラルで
  /// 最も decision level の高いリテラルを lit_list[1]
  /// に持ってくる．
  int
  reorder(
    vector<Literal>& lit_list ///< [inout] リテラルのリスト
  );

  /// @brief 変数のマークにアクセスする．
  bool
  get_mark(
    SatVarId var ///< [in] 対象の変数
  )
  {
    return mMark[var];
  }

  /// @brief 変数にマークをつけてキューに積む
  void
  set_mark(
    SatVarId var ///< [in] 対象の変数
  );

  /// @brief mClearQueue につまれた変数のマークを消す．
  void
  clear_marks(
    SizeType top = 0 ///< [in] マークを消す変数の先頭の位置
  );

  /// @brief 変数のマークを消す．
  void
  clear_mark(
    SatVarId var ///< [in] 対象の変数
  )
  {
    mMark[var] = false;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief make_minimal のサブルーティン
  bool
  check_recur(
    SatVarId var,  ///< [in] 対象の変数
    std::uint64_t lmask ///< [in] lit_list に含まれる変数の決定レベルのハッシュ値
  );

  /// @brief check_recur のサブルーティン
  ///
  /// lit の変数が未処理なら var_stack に積む．
  void
  put_var(
    Literal lit ///< [in] 対象のリテラル
  )
  {
    auto var = lit.varid();
    if ( !get_mark(var) && decision_level(var) > 0 ) {
      set_mark(var);
      mVarStack.push_back(var);
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数のマークを消すためのキュー
  vector<SatVarId> mClearQueue;

  // 変数のマーク
  vector<bool> mMark;

  // check_recur() で用いられるスタック
  vector<SatVarId> mVarStack;

};

END_NAMESPACE_YM_SAT

#endif // SABASE_H
