#ifndef YMSAT_CLAUSE_H
#define YMSAT_CLAUSE_H

/// @file Clause.h
/// @brief Clause のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "Literal.h"
#include "Watcher.h"
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

  /// @brief コンストラクタもどき
  static
  Clause*
  new_clause(
    const vector<Literal>& lit_list, ///< [in] リテラルのリスト
    bool learnt = false              ///< [in] 学習節の場合 true
  )
  {
    auto lit_num = static_cast<std::uint32_t>(lit_list.size());
    auto size = sizeof(Clause) + sizeof(Literal) * (lit_num - 1);
    auto p = new char[size];
    auto clause = new (p) Clause{lit_num, lit_list, learnt};
    return clause;
  }

  /// @brief デストラクタもどき
  static
  void
  delete_clause(
    Clause* clause
  )
  {
    delete clause->mWatcher[0];
    delete clause->mWatcher[1];
    auto p = reinterpret_cast<char*>(clause);
    delete [] p;
  }


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
    std::swap(mWatcher[0], mWatcher[1]);
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

  /// @brief 0番目の watcher を得る．
  Watcher*
  watcher0() const
  {
    return mWatcher[0];
  }

  /// @brief 1番目の watcher を得る．
  Watcher*
  watcher1() const
  {
    return mWatcher[1];
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
  // 内部で用いる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief コンストラクタ
  ///
  /// 上に書いたように普通にこのコンストラクタを呼んではいけない．
  Clause(
    std::uint32_t lit_num,           ///< [in] リテラル数
    const vector<Literal>& lit_list, ///< [in] リテラルのリスト
    bool learnt                      ///< [in] 学習節の場合 true
  ) : mSizeLearnt{(lit_num << 1) | static_cast<std::uint32_t>(learnt)}
  {
#if YMSAT_USE_LBD
    mLBD = lit_num;
#endif
    mWatcher[0] = new Watcher(Reason(this));
    mWatcher[1] = new Watcher(Reason(this));
    for ( int i = 0; i < lit_num; ++ i ) {
      mLits[i] = lit_list[i];
    }
  }

  /// @brief デストラクタ
  ~Clause()
  {
    // このデストラクタを呼んではいけない．
    abort();
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // サイズと learnt フラグをパックしたもの
  std::uint32_t mSizeLearnt;

#if YMSAT_USE_LBD
  // リテラルブロック距離
  int mLBD;
#endif

  // activity
  double mActivity{0.0};

  // Watcher
  Watcher* mWatcher[2];

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
