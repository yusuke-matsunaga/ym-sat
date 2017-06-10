#ifndef YM_SATVARID_H
#define YM_SATVARID_H

/// @file ym/SatVarId.h
/// @brief SatVarId の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym_config.h"
#include "ym/HashFunc.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class SatVarId SatVarId.h "ym/SatVarId.h"
/// @ingroup SatGroup
/// @brief 変数番号を表す型
///
/// 基本的にはただの符号なし整数だが，意味のある演算がほとんどないので
/// あえてクラスの形にしている．
/// 例えば変数どうしの四則演算に直接的な意味はない．
/// また，適正でない値(kSatVarIdIllegal)という特別な定数を持つ．
//////////////////////////////////////////////////////////////////////
class SatVarId
{
public:

  /// @brief 空のコンストラクタ
  /// @note 不正な値(kSatVarIdIllegal)になる．
  SatVarId();

  /// @brief 値を指定したコンストラクタ
  explicit
  SatVarId(ymuint val);

  /// @brief デストラクタ
  ~SatVarId();


public:
  //////////////////////////////////////////////////////////////////////
  // 外から使われる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 適正な値を持っている時に true を返す．
  bool
  is_valid() const;

  /// @brief 値を取り出す．
  ///
  /// is_valid() == false の時の値は不定
  ymuint
  val() const;

  friend
  bool
  operator==(const SatVarId& left,
	     const SatVarId& right);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実際の値
  ymuint mVal;

};

/// @brief 等価比較
/// @param[in] left, right 比較対象の変数
bool
operator==(const SatVarId& left,
	   const SatVarId& right);

/// @brief 非等価比較
/// @param[in] left, right 比較対象の変数
bool
operator!=(const SatVarId& left,
	   const SatVarId& right);

/// @relates SatVarId
/// @brief ストリームに対する出力
/// @param[in] s 出力先のストリーム
/// @param[in] varid 対象の変数
ostream&
operator<<(ostream& s,
	   const SatVarId& varid);


//////////////////////////////////////////////////////////////////////
// SatVarId を要素とするコンテナクラスの定義
//////////////////////////////////////////////////////////////////////

/// @ingroup LogicGroup
/// @brief 変数番号のベクタ
typedef vector<SatVarId> SatVarVector;

/// @ingroup LogicGroup
/// @brief 変数番号のリスト
typedef list<SatVarId> SatVarList;


//////////////////////////////////////////////////////////////////////
// HashBase<SatVarId> 用のハッシュ関数
//////////////////////////////////////////////////////////////////////
template<>
struct
HashFunc<SatVarId>
{
  ymuint
  operator()(SatVarId key) const {
    return key.val();
  }
};


//////////////////////////////////////////////////////////////////////
// 定数
//////////////////////////////////////////////////////////////////////

/// @relates SatVarId
/// @brief 不正な値
extern
const SatVarId kSatVarIdIllegal;


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 空のコンストラクタ
inline
SatVarId::SatVarId() :
  mVal(-1)
{
}

// @brief 値を指定したコンストラクタ
inline
SatVarId::SatVarId(ymuint val) :
  mVal(val)
{
}

// @brief デストラクタ
inline
SatVarId::~SatVarId()
{
}

// @brief 適正な値を持っている時に true を返す．
inline
bool
SatVarId::is_valid() const
{
  return mVal != -1;
}

// @brief 値を取り出す．
inline
ymuint
SatVarId::val() const
{
  return mVal;
}

// @brief 等価比較
// @param[in] left, right 比較対象の変数
inline
bool
operator==(const SatVarId& left,
	   const SatVarId& right)
{
  return left.mVal ==  right.mVal;
}

// @brief 非等価比較
// @param[in] left, right 比較対象の変数
inline
bool
operator!=(const SatVarId& left,
	   const SatVarId& right)
{
  return !operator==(left, right);
}

// @relates SatVarId
// @brief ストリームに対する出力
// @param[in] s 出力先のストリーム
// @param[in] varid 対象の変数
inline
ostream&
operator<<(ostream& s,
	   const SatVarId& varid)
{
  return s << varid.val();
}

END_NAMESPACE_YM

#endif // YM_SATVARID_H
