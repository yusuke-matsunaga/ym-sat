#ifndef YM_SATORDEREDSET_H
#define YM_SATORDEREDSET_H

/// @file ym/SatOrderedSet.h
/// @brief SatOrderedSet のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatOrderedSet SatOrderedSet.h "SatOrderedSet.h"
/// @brief 順序符号化で多値を表すクラス
///
/// * 最小値と最大値の間の自然数を値として取る．
/// * 自然数の大小関係に基づく順序関係を持つ．
//////////////////////////////////////////////////////////////////////
class SatOrderedSet
{
public:

  /// @brief コンストラクタ
  ///
  /// 中身は不定
  SatOrderedSet();

  /// @brief 内容を指定したコンストラクタ
  /// @param[in] solver SATソルバ
  /// @param[in] min 最小値
  /// @param[in] max 最大値
  SatOrderedSet(SatSolver& solver,
		int min,
		int max);

  /// @brief デストラクタ
  ~SatOrderedSet();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化を行う．
  /// @param[in] solver SATソルバ
  /// @param[in] min 最小値
  /// @param[in] max 最大値
  void
  init(SatSolver& solver,
       int min,
       int max);

  /// @brief 最小値を返す．
  int
  min() const;

  /// @brief 最大値を返す．
  int
  max() const;

  /// @brief 値に対応する変数を返す．
  /// @param[in] val 値 ( min <= val <= max )
  ///
  /// var(val) == True <==> 値が val
  SatLiteral
  var(int val) const;

  /// @brief 値に対応する変数のベクタを返す．
  const vector<SatLiteral>&
  vars() const;

  /// @brief 順序符号化された変数を返す．
  /// @param[in] val 値 ( min < val <= max )
  ///
  /// この変数が true == 値が val 以上
  /// 値が min 以上は常に成り立つので val > min となる．
  /// つまり var() よりも範囲が一つ短い．
  SatLiteral
  pri_var(int val) const;

  /// @brief 順序符号化された変数のベクタを返す．
  const vector<SatLiteral>&
  pri_vars() const;

  /// @brief この変数の値が lval 以上になるという制約を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] lval 値
  void
  add_ge_constraint(SatSolver& solver,
		    int lval);

  /// @brief この変数の値が uval 以下になるという制約を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] uval 値
  void
  add_le_constraint(SatSolver& solver,
		    int uval);

  /// @brief この変数の値が uval 以下か lval 以上という制約を作る．
  /// @param[in] solver SATソルバ
  /// @param[in] uval 上限値
  /// @param[in] lval 下限値
  ///
  /// uval < lval を仮定する．
  /// そうでなければ常に成り立つ．
  void
  add_dropoff_constraint(SatSolver& solver,
			 int uval,
			 int lval);

  /// @brief SATの解から値を得る．
  int
  val(const SatModel& model) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 最小値
  int mMin;

  // 最大値
  int mMax;

  // 変数(実際にはリテラル)の配列
  vector<SatLiteral> mVarArray;

  // 順序符号化された裏の変数
  vector<SatLiteral> mPriVarArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 最小値を返す．
inline
int
SatOrderedSet::min() const
{
  return mMin;
}

// @brief 最大値を返す．
inline
int
SatOrderedSet::max() const
{
  return mMax;
}

// @brief 値に対応する変数を返す．
// @param[in] val 値 ( min <= val <= max )
inline
SatLiteral
SatOrderedSet::var(int val) const
{
  ASSERT_COND( mMin <= val && val <= mMax );

  return mVarArray[val - mMin];
}

// @brief 値に対応する変数のベクタを返す．
inline
const vector<SatLiteral>&
SatOrderedSet::vars() const
{
  return mVarArray;
}

// @brief 順序符号化された変数を返す．
// @param[in] val 値 ( min < val <= max )
//
// この変数が true == 値が val 以上
// 値が min 以上は常に成り立つので val > min となる．
inline
SatLiteral
SatOrderedSet::pri_var(int val) const
{
  ASSERT_COND( mMin < val && val <= mMax );

  return mPriVarArray[val - mMin - 1];
}

// @brief 順序符号化された変数のベクタを返す．
inline
const vector<SatLiteral>&
SatOrderedSet::pri_vars() const
{
  return mPriVarArray;
}

END_NAMESPACE_YM_SAT

#endif // YM_SATORDEREDSET_H
