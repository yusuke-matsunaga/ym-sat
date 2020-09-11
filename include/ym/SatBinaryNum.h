#ifndef SATBINARYNUM_H
#define SATBINARYNUM_H

/// @file SatBinaryNum.h
/// @brief SatBinaryNum のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatBinaryNum SatBinaryNum.h "SatBinaryNum.h"
/// @brief 2進符号化を用いて数値を表すクラス
//////////////////////////////////////////////////////////////////////
class SatBinaryNum
{
public:

  /// @brief コンストラクタ
  SatBinaryNum();

  /// @brief コンストラクタ
  /// @param[in] solver SATソルバ
  /// @param[in] bit_num ビット幅
  SatBinaryNum(SatSolver& solver,
	       SizeType bit_num);

  /// @brief デストラクタ
  ~SatBinaryNum();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  /// @param[in] solver SATソルバ
  /// @param[in] bit_num ビット幅
  void
  init(SatSolver& solver,
       SizeType bit_num);

  /// @brief ビット数を返す．
  SizeType
  bit_num() const;

  /// @brief ビットに対応する変数を返す．
  /// @param[in] bit 値 ( 0 <= bit < bit_num() )
  SatLiteral
  bit_var(SizeType bit) const;

  /// @brief 値に対応する変数のベクタを返す．
  const vector<SatLiteral>&
  bit_vars() const;

  /// @brief SATの解から値を得る．
  /// @param[in] model SATの解
  SizeType
  val(const SatModel& model) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ビット数
  SizeType mBitNum;

  // 変数(実際にはリテラル)の配列
  vector<SatLiteral> mVarArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief ビット数を返す．
inline
SizeType
SatBinaryNum::bit_num() const
{
  return mBitNum;
}

// @brief ビットに対応する変数を返す．
// @param[in] bit 値 ( 0 <= bit < bit_num() )
inline
SatLiteral
SatBinaryNum::bit_var(SizeType bit) const
{
  ASSERT_COND( 0 <= bit && bit < bit_num() );

  return mVarArray[bit];
}

// @brief 値に対応する変数のベクタを返す．
inline
const vector<SatLiteral>&
SatBinaryNum::bit_vars() const
{
  return mVarArray;
}

END_NAMESPACE_YM_SAT

#endif // SATBINARYNUM_H
