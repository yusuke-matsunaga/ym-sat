#ifndef YMSAT_CLAUSE_H
#define YMSAT_CLAUSE_H

/// @file Clause.h
/// @brief Clause のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "Literal.h"
#include "conf.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class Clause Clause.h "Clause.h"
/// @brief 節を表すクラス
///
/// 中身は本当に Literal の配列．しかも，クラスのインスタンスと別に配列
/// 領域を確保するのはポインタ参照が一回増えて嫌なので，見かけはサイズ1
/// の配列 mLits[1] を定義しておいて，実際には要素数分の領域を確保した
/// メモリブロックを Clause* として扱う．
/// このために「ポインタ付き new」演算子を用いている．
/// 詳しくは YmSat::new_clause() を参照
/// Clause はそれ以外の情報として，制約節か学習節かを区別する1ビット
/// (サイズと合わせて1ワード)のフラグ，activity を表す double 変数を
/// 持つ．
//////////////////////////////////////////////////////////////////////
class Clause
{
public:

  /// @brief コンストラクタ
  ///
  /// 上に書いたように普通にこのコンストラクタを呼んではいけない．
  Clause(
    SizeType lit_num, ///< [in] リテラル数
    Literal* lits,    ///< [in] リテラルの配列
    bool learnt       ///< [in] 学習節の場合 true
  )
  {
    mSizeLearnt = (lit_num << 1) | static_cast<ymuint>(learnt);
#if YMSAT_USE_LBD
    mLBD = lit_num;
#endif
    mActivity = 0.0;
    for ( int i = 0; i < lit_num; ++ i ) {
      mLits[i] = lits[i];
    }
  }

  /// @brief デストラクタ
  ~Clause() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．(2リテラル節用)
  void
  set(
    Literal lit0, ///< [in] リテラル1
    Literal lit1  ///< [in] リテラル2
  )
  {
    mLits[0] = lit0;
    mLits[1] = lit1;
  }

  /// @brief watch literal を入れ替える．
  void
  xchange_wl()
  {
    std::swap(mLits[0], mLits[1]);
  }

  /// @brief src_pos 番めのリテラルを wl1 に移動する．
  ///
  /// 互いに位置を交換する．
  /// 間のリテラルは不変
  void
  xchange_wl1(
    int src_pos ///< [in] もとの位置
  )
  {
    std::swap(mLits[1], mLits[src_pos]);
  }

#if YMSAT_USE_LBD
  /// @brief literal block distance を設定する．
  void
  set_lbd(
    int lbd
  )
  {
    mLBD = lbd;
  }
#endif

  /// @brief アクティビティを増加させる．
  void
  increase_activity(
    double delta
  )
  {
    mActivity += delta;
  }

  /// @brief アクティビティを定数倍する．
  void
  factor_activity(
    double factor
  )
  {
    mActivity *= factor;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief リテラル数の取得
  SizeType
  lit_num() const
  {
    return (mSizeLearnt >> 1);
  }

  /// @brief リテラルのアクセス
  Literal
  lit(
    SizeType pos ///< [in] リテラルの位置 ( 0 <= pos < lit_num() )
  ) const
  {
    return mLits[pos];
  }

  /// @brief 0番めの watch literal を得る．
  Literal
  wl0() const
  {
    return mLits[0];
  }

  /// @brief 1番めの watch literal を得る．
  Literal
  wl1() const
  {
    return mLits[1];
  }

  /// @brief 学習節の場合 true を返す．
  bool
  is_learnt() const
  {
    return static_cast<bool>(mSizeLearnt & 1UL);
  }

#if YMSAT_USE_LBD
  /// @brief Literal Block Distance を返す．
  int
  lbd() const;
  {
    return mLBD;
  }
#endif

  /// @brief 学習節の場合にアクティビティを返す．
  double
  activity() const
  {
    return mActivity;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // サイズと learnt フラグをパックしたもの
  ymuint32 mSizeLearnt;

#if YMSAT_USE_LBD
  // リテラルブロック距離
  int mLBD;
#endif

  // activity
  double mActivity;

  // リテラルの配列
  // 実際にはこの後にリテラル数分の領域を確保する．
  Literal mLits[1];

};

// reduceDB で用いる Clause の比較関数
class ClauseLess
{
public:
  bool
  operator()(
    const Clause* a,
    const Clause* b
  )
  {
    return a->lit_num() > 2 && (b->lit_num() == 2 || a->activity() < b->activity() );
  }
};

/// @relates Clause
/// @brief Clause の内容を出力する
ostream&
operator<<(
  ostream& s,
  const Clause& c
);

END_NAMESPACE_YM_SAT

#endif // YMSAT_CLAUSE_H
