#ifndef SATBINARYNUM_H
#define SATBINARYNUM_H

/// @file SatBinaryNum.h
/// @brief SatBinaryNum のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
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
  SatBinaryNum(
    SatSolver& solver, ///< [in] SATソルバ
    SizeType bit_num   ///< [in] ビット幅
  );

  /// @brief デストラクタ
  ~SatBinaryNum();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  void
  init(
    SatSolver& solver, ///< [in] SATソルバ
    SizeType bit_num   ///< [in] ビット幅
  );

  /// @brief ビット数を返す．
  SizeType
  bit_num() const
  {
    return mBitNum;
  }

  /// @brief ビットに対応する変数を返す．
  SatLiteral
  bit_var(
    SizeType bit ///< [in] ビット位置 ( 0 <= bit < bit_num() )
  ) const
  {
    ASSERT_COND( 0 <= bit && bit < bit_num() );

    return mVarArray[bit];
  }

  /// @brief 値に対応する変数のベクタを返す．
  const vector<SatLiteral>&
  bit_vars() const
  {
    return mVarArray;
  }

  /// @brief SATの解から値を得る．
  SizeType
  val(
    const SatModel& model ///< [in] SATの解
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // ビット数
  SizeType mBitNum;

  // 変数(実際にはリテラル)の配列
  vector<SatLiteral> mVarArray;

};

END_NAMESPACE_YM_SAT

#endif // SATBINARYNUM_H
