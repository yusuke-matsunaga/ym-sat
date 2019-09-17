#ifndef SATMODEL_H
#define SATMODEL_H

/// @file SatModel.h
/// @brief SatModel のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatBool3.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatModel SatModel.h "SatModel.h"
/// @brief SATの解を表すクラス
//////////////////////////////////////////////////////////////////////
class SatModel
{
public:

  /// @brief コンストラクタ
  SatModel();

  /// @brief デストラクタ
  ~SatModel();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief サイズを返す．
  int
  size() const;

  /// @brief 対象の値を返す．
  /// @param[in] lit リテラル
  SatBool3
  operator[](SatLiteral lit) const;

  /// @brief 対象の値を返す(operator[] の別名)．
  /// @param[in] lit リテラル
  SatBool3
  get(SatLiteral lit) const;

  /// @brief 配列のサイズを設定する．
  void
  resize(int size);

  /// @brief 値をセットする．
  /// @param[in] pos 位置
  /// @param[in] val 値
  void
  set(int pos,
      SatBool3 val);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 各変数の値の配列
  vector<SatBool3> mValArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
SatModel::SatModel()
{
}

// @brief デストラクタ
inline
SatModel::~SatModel()
{
}

// @brief サイズを返す．
inline
int
SatModel::size() const
{
  return mValArray.size();
}

// @brief 対象の値を返す．
// @param[in] lit リテラル
inline
SatBool3
SatModel::operator[](SatLiteral lit) const
{
  ASSERT_COND( lit.is_valid() );
  int vid = lit.varid();
  ASSERT_COND( 0 <= vid && vid < mValArray.size() );
  SatBool3 val = mValArray[vid];
  if ( lit.is_negative() ) {
    val = ~val;
  }
  return val;
}

// @brief 対象の値を返す(operator[] の別名)．
// @param[in] lit リテラル
inline
SatBool3
SatModel::get(SatLiteral lit) const
{
  return operator[](lit);
}

// @brief 配列のサイズを設定する．
inline
void
SatModel::resize(int size)
{
  mValArray.clear();
  mValArray.resize(size, SatBool3::X);
}

// @brief 値をセットする．
// @param[in] pos 位置
// @param[in] val 値
inline
void
SatModel::set(int pos,
	      SatBool3 val)
{
  ASSERT_COND( 0 <= pos && pos < mValArray.size() );

  mValArray[pos] = val;
}

END_NAMESPACE_YM_SAT

#endif // SATMODEL_H
