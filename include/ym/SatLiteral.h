﻿#ifndef YM_SATLITERAL_H
#define YM_SATLITERAL_H

/// @file ym/SatLiteral.h
/// @brief SatLiteral のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2016, 2017, 2018, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @ingroup SatGroup
/// @class SatLiteral SatLiteral.h "ym/SatLiteral.h"
/// @brief リテラル(変数番号＋極性)を表すクラス
///
/// 変数番号は 0 から始まる非負の整数
/// 単純には最下位ビットで極性を表すだけだが，
/// 不正な値のときにはこのビットに1を立てない．
/// そのために neg_mask() というメンバ関数を用意している．
//////////////////////////////////////////////////////////////////////
class SatLiteral
{
public:

  /// @brief デフォルトコンストラクタ
  ///
  /// 内容は不定なのであまり使わない方が良い．
  SatLiteral();

  /// @brief コピーコンストラクタの変種
  /// @param[in] lit リテラル
  /// @param[in] inv 極性フラグ
  ///                - false: 反転なし (正極性)
  ///                - true:  反転あり (負極性)
  SatLiteral(const SatLiteral& lit,
	     bool inv = false);

  /// @brief 変数番号と極性からの変換関数
  /// @param[in] varid 変数番号
  /// @param[in] inv 極性フラグ
  ///                - false: 反転なし (正極性)
  ///                - true:  反転あり (負極性)
  static
  SatLiteral
  conv_from_varid(int varid,
		  bool inv);

  /// @brief index からの変換関数
  /// @param[in] index 変数番号と極性をエンコードしたもの
  static
  SatLiteral
  index2literal(int index);

  // コピーコンストラクタ,代入演算子,デストラクタはデフォルト
  // のものがそのまま使える．


public:
  //////////////////////////////////////////////////////////////////////
  // 内容をセットする関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  /// @param[in] varid 変数番号
  /// @param[in] inv 極性
  ///                - false: 反転なし (正極性)
  ///                - true:  反転あり (負極性)
  void
  set(int varid,
      bool inv = false);


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容が適正の時に true を返す．
  bool
  is_valid() const;

  /// @brief 変数番号を得る．
  /// @return 変数番号
  int
  varid() const;

  /// @brief 正極性のリテラルの時 true を返す．
  bool
  is_positive() const;

  /// @brief 負極性のリテラルの時 true を返す．
  bool
  is_negative() const;

  /// @brief 自身の極性を反転させる．
  /// @return 自身の参照を返す．
  ///
  /// 不正な値の場合は変化しない．
  const SatLiteral&
  invert();

  /// @brief 極性の反転
  /// @return 極性を反転させたリテラルを返す．
  ///
  /// 不正な値の場合は変化しない．
  SatLiteral
  operator~() const;

  /// @brief 同じ変数の正極性リテラルを返す．
  ///
  /// 不正な値の場合は変化しない．
  SatLiteral
  make_positive() const;

  /// @brief 同じ変数の負極性リテラルを返す．
  ///
  /// 不正な値の場合は変化しない．
  SatLiteral
  make_negative() const;

  /// @brief ハッシュ用の関数
  SizeType
  hash() const;

  /// @brief 配列のインデックスとして使用可能な数を返す．
  int
  index() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部でのみ用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 反転用のビットマスクを返す．
  ///
  /// 普通は 1U だが不正な値のときは 0U となる．
  int
  neg_mask() const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数番号と極性をパックしたもの
  int mIndex;

};

/// @relates SatLiteral
/// @brief 未定義リテラル
extern
const SatLiteral kSatLiteralX;

/// @relates SatLiteral
/// @brief 等価比較
/// @param[in] lit1, lit2 比較するリテラル
/// @return lit1 と lit2 が等しいリテラルの時 true を返す．
bool
operator==(SatLiteral lit1,
	   SatLiteral lit2);

/// @relates SatLiteral
/// @brief 非等価比較
/// @param[in] lit1, lit2 比較するリテラル
/// @return lit1 と lit2 が等しいリテラルでないとき true を返す．
bool
operator!=(SatLiteral lit1,
	   SatLiteral lit2);

/// @relates SatLiteral
/// @brief 小なり比較演算
/// @param[in] lit1, lit2 比較するリテラル
bool
operator<(SatLiteral lit1,
	  SatLiteral lit2);

/// @relates SatLiteral
/// @brief 小なりイコール比較演算
/// @param[in] lit1, lit2 比較するリテラル
bool
operator<=(SatLiteral lit1,
	   SatLiteral lit2);

/// @relates SatLiteral
/// @brief 大なり比較演算
/// @param[in] lit1, lit2 比較するリテラル
bool
operator>(SatLiteral lit1,
	  SatLiteral lit2);

/// @relates SatLiteral
/// @brief 大なりイコール比較演算
/// @param[in] lit1, lit2 比較するリテラル
bool
operator>=(SatLiteral lit1,
	   SatLiteral lit2);

/// @relates SatLiteral
/// @brief SatLiteral の内容を ostream に出力する関数
/// @param[in] s 出力ストリーム
/// @param[in] lit 出力対象のリテラル
/// @return s
ostream&
operator<<(ostream& s,
	   const SatLiteral& lit);


//////////////////////////////////////////////////////////////////////
// 上記のクラスを要素とするコンテナクラス
//////////////////////////////////////////////////////////////////////

/// @brief リテラルのベクタ
typedef vector<SatLiteral> SatLiteralVector;

/// @brief リテラルのリスト
typedef list<SatLiteral> SatLiteralList;


//////////////////////////////////////////////////////////////////////
// inline 関数の定義
//////////////////////////////////////////////////////////////////////

// 内容を設定する．
inline
void
SatLiteral::set(int varid,
		bool inv)
{
  if ( varid < 0 ) {
    mIndex = -1;
  }
  else {
    mIndex = (varid << 1) + static_cast<int>(inv);
  }
}

// デフォルトコンストラクタ
inline
SatLiteral::SatLiteral() :
  mIndex(-1)
{
}

// @brief コピーコンストラクタの変種
// @param[in] lit リテラル
// @param[in] inv 極性フラグ
//                - false: 反転なし (正極性)
//                - true:  反転あり (負極性)
inline
SatLiteral::SatLiteral(const SatLiteral& lit,
		       bool inv) :
  mIndex{lit.mIndex}
{
  if ( inv ) {
    mIndex ^= neg_mask();
  }
}

// @brief 変数番号と極性からの変換関数
// @param[in] varid 変数番号
// @param[in] inv 極性フラグ
//                - false: 反転なし (正極性)
//                - true:  反転あり (負極性)
inline
SatLiteral
SatLiteral::conv_from_varid(int varid,
			    bool inv)
{
  SatLiteral ans;
  ans.set(varid, inv);
  return ans;
}

// @brief index からの変換関数
inline
SatLiteral
SatLiteral::index2literal(int index)
{
  SatLiteral ans;
  ans.mIndex = index;
  return ans;
}

// @brief 内容が適正の時に true を返す．
inline
bool
SatLiteral::is_valid() const
{
  return mIndex >= 0;
}

// 変数番号を得る．
inline
int
SatLiteral::varid() const
{
  if ( is_valid() ) {
    return mIndex >> 1;
  }
  else {
    return -1;
  }
}

// @brief 正極性のリテラルの時 true を返す．
inline
bool
SatLiteral::is_positive() const
{
  return !is_negative();
}

// @brief 負極性のリテラルの時 true を返す．
inline
bool
SatLiteral::is_negative() const
{
  return static_cast<bool>(mIndex & 1);
}

// @brief 自身の極性を反転させる．
// @return 自身の参照を返す．
inline
const SatLiteral&
SatLiteral::invert()
{
  mIndex ^= neg_mask();
  return *this;
}

// 極性を反転させたリテラルを返す．
inline
SatLiteral
SatLiteral::operator~() const
{
  return index2literal(mIndex ^ neg_mask());
}

// @brief 同じ変数の正極性リテラルを返す．
inline
SatLiteral
SatLiteral::make_positive() const
{
  // 不正な値の場合でもこれはOK
  return index2literal(mIndex & ~1U);
}

// @brief 同じ変数の負極性リテラルを返す．
inline
SatLiteral
SatLiteral::make_negative() const
{
  return index2literal(mIndex | neg_mask());
}

// 等価比較
inline
bool
operator==(SatLiteral lit1,
	   SatLiteral lit2)
{
  return lit1.index() == lit2.index();
}
inline
bool
operator!=(SatLiteral lit1,
	   SatLiteral lit2)
{
  return !operator==(lit1, lit2);
}

// @relates SatLiteral
// @brief 小なり比較演算
// @param[in] lit1, lit2 比較するリテラル
inline
bool
operator<(SatLiteral lit1,
	  SatLiteral lit2)
{
  return lit1.index() < lit2.index();
}

// @relates SatLiteral
// @brief 小なりイコール比較演算
// @param[in] lit1, lit2 比較するリテラル
inline
bool
operator<=(SatLiteral lit1,
	   SatLiteral lit2)
{
  return !operator<(lit2, lit1);
}

// @relates SatLiteral
// @brief 大なり比較演算
// @param[in] lit1, lit2 比較するリテラル
inline
bool
operator>(SatLiteral lit1,
	  SatLiteral lit2)
{
  return operator<(lit2, lit1);
}

// @relates SatLiteral
// @brief 大なりイコール比較演算
// @param[in] lit1, lit2 比較するリテラル
inline
bool
operator>=(SatLiteral lit1,
	   SatLiteral lit2)
{
  return !operator<(lit1, lit2);
}

// ハッシュ用の関数
inline
SizeType
SatLiteral::hash() const
{
  return static_cast<SizeType>(mIndex);
}

// @brief 配列のインデックスとして使用可能な数を返す．
inline
int
SatLiteral::index() const
{
  return mIndex;
}

// @brief 反転用のビットマスクを返す．
//
// 普通は 1U だが不正な値のときは 0U となる．
inline
int
SatLiteral::neg_mask() const
{
  return is_valid() ? 1U : 0U;
}

// @relates SatLiteral
// @brief SatLiteral の内容を ostream に出力する関数
// @param[in] s 出力ストリーム
// @param[in] lit 出力対象のリテラル
// @return s
inline
ostream&
operator<<(ostream& s,
	   const SatLiteral& lit)
{
  if ( lit.is_valid() ) {
    s << lit.varid();
    if ( lit.is_negative() ) {
      s << "'";
    }
  }
  else {
    s << "-X-";
  }
  return s;
}

END_NAMESPACE_YM


BEGIN_NAMESPACE_STD

/// @brief SatLiteral の等価比較関数クラス
template <>
struct equal_to<YM_NAMESPACE::SatLiteral>
{
  bool
  operator()(YM_NAMESPACE::SatLiteral n1,
	     YM_NAMESPACE::SatLiteral n2) const
  {
    return n1 == n2;
  }
};

/// @brief SatLiteral をキーにしたハッシュ関数クラス
template <>
struct hash<YM_NAMESPACE::SatLiteral>
{
  SizeType
  operator()(YM_NAMESPACE::SatLiteral lit) const
  {
    return lit.hash();
  }
};

END_NAMESPACE_STD

#endif // YM_SATLITERAL_H
