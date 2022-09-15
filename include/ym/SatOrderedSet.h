#ifndef YM_SATORDEREDSET_H
#define YM_SATORDEREDSET_H

/// @file ym/SatOrderedSet.h
/// @brief SatOrderedSet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatOrderedSet SatOrderedSet.h "SatOrderedSet.h"
/// @brief 順序符号化で多値を表すクラス
///
/// * 最小値と最大値の間の整数を値として取る．
/// * 最小値 <= 最大値であれば負数でも構わない
/// * 整数の大小関係に基づく順序関係を持つ．
//////////////////////////////////////////////////////////////////////
class SatOrderedSet
{
public:

  /// @brief コンストラクタ
  SatOrderedSet(
    SatSolver& solver, ///< [in] SATソルバ
    int min,           ///< [in] 最小値
    int max            ///< [in] 最大値
  );

  /// @brief デストラクタ
  ~SatOrderedSet() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 最小値を返す．
  int
  min() const
  {
    return mMin;
  }

  /// @brief 最大値を返す．
  int
  max() const
  {
    return mMax;
  }

  /// @brief 値に対応する変数を返す．
  ///
  /// var(val) == True <==> 値が val
  SatLiteral
  var(
    int val ///< [in] 値 ( min <= val <= max )
  ) const
  {
    ASSERT_COND( mMin <= val && val <= mMax );

    return mVarArray[val - mMin];
  }

  /// @brief 値に対応する変数のベクタを返す．
  const vector<SatLiteral>&
  vars() const
  {
    return mVarArray;
  }

  /// @brief 順序符号化された変数を返す．
  ///
  /// この変数が true == 値が val 以上
  /// 値が min 以上は常に成り立つので val > min となる．
  /// つまり var() よりも範囲が一つ短い．
  SatLiteral
  pri_var(
    int val ///< [in] 値 ( min < val <= max )
  ) const
  {
    ASSERT_COND( mMin < val && val <= mMax );

    return mPriVarArray[val - mMin - 1];
  }

  /// @brief 順序符号化された変数のベクタを返す．
  const vector<SatLiteral>&
  pri_vars() const
  {
    return mPriVarArray;
  }

  /// @brief この変数の値が lval 以上になるという制約を作る．
  void
  add_ge_constraint(
    int lval ///< [in] 下限値
  );

  /// @brief この変数の値が uval 以下になるという制約を作る．
  void
  add_le_constraint(
    int uval ///< [in] 上限値
  );

  /// @brief この変数の値が uval 以下か lval 以上という制約を作る．
  ///
  /// uval < lval を仮定する．
  /// そうでなければ常に成り立つ．
  void
  add_dropoff_constraint(
    int uval, ///< [in] 上限値
    int lval  ///< [in] 下限値
  );

  /// @brief SATの解から値を得る．
  int
  val(
    const SatModel& model ///< [in] SATの解
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバー
  SatSolver& mSolver;

  // 最小値
  int mMin;

  // 最大値
  int mMax;

  // 変数(実際にはリテラル)の配列
  vector<SatLiteral> mVarArray;

  // 順序符号化された裏の変数
  vector<SatLiteral> mPriVarArray;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATORDEREDSET_H
