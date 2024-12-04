#ifndef CNFSIZE_H
#define CNFSIZE_H

/// @file CnfSize.h
/// @brief CnfSize のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2024 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class CnfSize CnfSize.h "CnfSize.h"
/// @brief CNF のサイズを表す構造体
//////////////////////////////////////////////////////////////////////
struct CnfSize
{
  SizeType clause_num{0};  ///< 項数
  SizeType literal_num{0}; ///< リテラル数

  /// @brief {0, 0} の値を作るクラスメソッド
  static
  CnfSize
  zero()
  {
    return CnfSize{0, 0};
  }

  /// @brief 加算付き代入文
  CnfSize&
  operator+=(
    const CnfSize& right ///< [in] オペランド
  )
  {
    clause_num += right.clause_num;
    literal_num += right.literal_num;
    return *this;
  }

  /// @brief 減算付き代入文
  CnfSize&
  operator-=(
    const CnfSize& right ///< [in] オペランド
  )
  {
    clause_num -= right.clause_num;
    literal_num -= right.literal_num;
    return *this;
  }

  /// @brief 加算
  CnfSize
  operator+(
    const CnfSize& right ///< [in] オペランド
  ) const
  {
    return CnfSize{*this}.operator+=(right);
  }

  /// @brief 減算
  CnfSize
  operator-(
    const CnfSize& right ///< [in] オペランド
  ) const
  {
    return CnfSize{*this}.operator-=(right);
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const CnfSize& right ///< [in] オペランド
  ) const
  {
    return clause_num == right.clause_num
      && literal_num == right.literal_num;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const CnfSize& right ///< [in] オペランド
  ) const
  {
    return !operator==(right);
  }

};

/// @brief ストリーム出力
inline
ostream&
operator<<(
  ostream& s,         ///< [in] 出力先のストリーム
  const CnfSize& size ///< [in] 対象のオブジェクト
)
{
  s << "#C: " << size.clause_num
    << " #L: " << size.literal_num;
  return s;
}

END_NAMESPACE_YM_SAT

#endif // CNFSIZE_H
