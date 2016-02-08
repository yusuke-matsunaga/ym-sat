#ifndef YM_SATBOOL3_H
#define YM_SATBOOL3_H

/// @file ym/SatBool3.h
/// @brief SatBool3 の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym_config.h"


BEGIN_NAMESPACE_YM

/// @brief 不定値を含む論理値
/// @ingroup LogicGroup
enum SatBool3 {
  /// @brief false
  kB3False = -1,
  /// @brief true
  kB3True  = 1,
  /// @brief unknown
  kB3X     = 0
};

/// @brief SatBool3 の否定演算
/// @ingroup LogicGroup
inline
SatBool3
operator~(SatBool3 val)
{
  // 条件分岐を用いない dirty hack
  return static_cast<SatBool3>(-static_cast<int>(val));
}

/// @brief SatBool3 の inplace XOR 演算
inline
SatBool3
operator^=(SatBool3& left,
	   SatBool3 right)
{
  return left = static_cast<SatBool3>(static_cast<int>(left) * static_cast<int>(right) * -1);
}

/// @brief SatBool3 の XOR 演算
inline
SatBool3
operator^(SatBool3 left,
	  SatBool3 right)
{
  return static_cast<SatBool3>(static_cast<int>(left) * static_cast<int>(right) * -1);
}

/// @brief bool からのキャスト演算子
/// @ingroup LogicGroup
inline
SatBool3
b2B3(bool b)
{
  // 条件分岐を用いない dirty hack
  // true:1 false:0 と仮定している．
  return static_cast<SatBool3>((static_cast<int>(b) << 1) - 1);
}

/// @brief SatBool3 の内容を出力するストリーム演算子
/// @ingroup LogicGroup
inline
ostream&
operator<<(ostream& s,
	   SatBool3 val)
{
  switch ( val ) {
  case kB3False: s << "false"; break;
  case kB3True:  s << "true"; break;
  case kB3X:     s << "unknown"; break;
  }
  return s;
}

END_NAMESPACE_YM

#endif // YM_SATBOOL3_H
