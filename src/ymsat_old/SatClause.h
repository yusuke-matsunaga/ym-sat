#ifndef YMSAT1OLD_SATCLAUSE_H
#define YMSAT1OLD_SATCLAUSE_H

/// @file SatClause.h
/// @brief SatClause のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"
#include "ym/SatLiteral.h"
#include "conf.h"
#include "ymsat_old.h"


BEGIN_NAMESPACE_YM_YMSATOLD

//////////////////////////////////////////////////////////////////////
/// @class SatClause SatClause.h "SatClause.h"
/// @brief 節を表すクラス
///
/// 中身は本当に SatLiteral の配列．しかも，クラスのインスタンスと別に配列
/// 領域を確保するのはポインタ参照が一回増えて嫌なので，見かけはサイズ1
/// の配列 mLits[1] を定義しておいて，実際には要素数分の領域を確保した
/// メモリブロックを SatClause* として扱う．
/// このために「ポインタ付き new」演算子を用いている．
/// 詳しくは YmSat::new_clause() を参照
/// SatClause はそれ以外の情報として，制約節か学習節かを区別する1ビット
/// (サイズと合わせて1ワード)のフラグ，activity を表す double 変数を
/// 持つ．
//////////////////////////////////////////////////////////////////////
class SatClause
{
public:

  /// @brief コンストラクタ
  /// @param[in] lit_num リテラル数
  /// @param[in] lits リテラルの配列
  /// @param[in] learnt 学習節の場合 true
  /// @note 上に書いたように普通にこのコンストラクタを呼んではいけない．
  SatClause(int lit_num,
	    SatLiteral* lits,
	    bool learnt);

  /// @brief デストラクタ
  ~SatClause();


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を設定する外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．(2リテラル節用)
  /// @param[in] lit0, lit1 リテラル
  void
  set(SatLiteral lit0,
      SatLiteral lit1);

  /// @brief watch literal を入れ替える．
  void
  xchange_wl();

  /// @brief src_pos 番めのリテラルを wl1 に移動する．
  /// @param[in] src_pos もとの位置
  /// @note 互いに位置を交換する．
  /// @note 間のリテラルは不変
  void
  xchange_wl1(int src_pos);

  /// @brief literal block distance を設定する．
  void
  set_lbd(int lbd);

  /// @brief アクティビティを増加させる．
  void
  increase_activity(double delta);

  /// @brief アクティビティを定数倍する．
  void
  factor_activity(double factor);


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief リテラル数の取得
  int
  lit_num() const;

  /// @brief リテラルのアクセス
  /// @param[in] pos リテラルの位置 ( 0 <= pos < lit_num() )
  SatLiteral
  lit(int pos) const;

  /// @brief 0番めの watch literal を得る．
  SatLiteral
  wl0() const;

  /// @brief 1番めの watch literal を得る．
  SatLiteral
  wl1() const;

  /// @brief 学習節の場合 true を返す．
  bool
  is_learnt() const;

#if YMSAT_USE_LBD
  /// @brief SatLiteral Block Distance を返す．
  int
  lbd() const;
#endif

  /// @brief 学習節の場合にアクティビティを返す．
  double
  activity() const;


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
  SatLiteral mLits[1];

};

/// @relates SatClause
/// @brief SatClause の内容を出力する
ostream&
operator<<(ostream& s,
	   const SatClause& c);


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
// @param[in] learnt 学習節の場合 true
inline
SatClause::SatClause(int lit_num,
		     SatLiteral* lits,
		     bool learnt)
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

// @brief デストラクタ
inline
SatClause::~SatClause()
{
}

// @brief 内容を設定する．(2リテラル節用)
// @param[in] lit0, lit1 リテラル
inline
void
SatClause::set(SatLiteral lit0,
	       SatLiteral lit1)
{
  mLits[0] = lit0;
  mLits[1] = lit1;
}

// @brief watch literal を入れ替える．
inline
void
SatClause::xchange_wl()
{
  SatLiteral tmp = mLits[0];
  mLits[0] = mLits[1];
  mLits[1] = tmp;
}

// @brief src_pos 番めのリテラルを wl1 に移動する．
// @param[in] src_pos もとの位置
// @note 間のリテラルは不変
inline
void
SatClause::xchange_wl1(int src_pos)
{
  SatLiteral tmp = mLits[src_pos];
  mLits[src_pos] = mLits[1];
  mLits[1] = tmp;
}

#if YMSAT_USE_LBD
// @brief literal block distance を設定する．
inline
void
SatClause::set_lbd(int lbd)
{
  mLBD = lbd;
}
#endif

// @brief リテラル数の取得
inline
int
SatClause::lit_num() const
{
  return (mSizeLearnt >> 1);
}

// @brief リテラルのアクセス
inline
SatLiteral
SatClause::lit(int pos) const
{
  return mLits[pos];
}

// @brief 0番めの watch literal を得る．
inline
SatLiteral
SatClause::wl0() const
{
  return mLits[0];
}

// @brief 1番めの watch literal を得る．
inline
SatLiteral
SatClause::wl1() const
{
  return mLits[1];
}

// @brief 学習節の場合 true を返す．
inline
bool
SatClause::is_learnt() const
{
  return static_cast<bool>(mSizeLearnt & 1UL);
}

#if YMSAT_USE_LBD
// @brief SatLiteral Block Distance を返す．
inline
int
SatClause::lbd() const
{
  return mLBD;
}
#endif

// @brief 学習節の場合にアクティビティを返す．
inline
double
SatClause::activity() const
{
  return mActivity;
}

// @brief アクティビティを増加させる．
inline
void
SatClause::increase_activity(double delta)
{
  mActivity += delta;
}

// @brief アクティビティを定数倍する．
inline
void
SatClause::factor_activity(double factor)
{
  mActivity *= factor;
}

END_NAMESPACE_YM_YMSATOLD

#endif // YMSAT1OLD_SATCLAUSE_H
