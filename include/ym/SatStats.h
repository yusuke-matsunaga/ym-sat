#ifndef YM_SATSTATS_H
#define YM_SATSTATS_H

/// @file ym/SatStats.h
/// @brief SatStats のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatStats SatStats.h "ym/SatStats.h"
/// @ingroup SatGroup
/// @brief SAT ソルバの状態を表すクラス
/// @sa SatSolver
//////////////////////////////////////////////////////////////////////
class SatStats
{
public:

  /// @brief コンストラクタ
  SatStats() = default;

  /// @brief デストラクタ
  ~SatStats() = default;

  /// @brief 内容をクリアする
  void
  clear();

  /// @brief 加算付き代入
  const SatStats&
  operator+=(const SatStats& right);

  /// @brief 減算付き代入
  const SatStats&
  operator-=(const SatStats& right);

  /// @brief MAX演算つき代入
  const SatStats&
  max_assign(const SatStats& right);


public:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  /// @brief restartの回数
  int mRestart{0};

  /// @brief 変数の数．
  int mVarNum{0};

  /// @brief 制約節の数
  int mConstrClauseNum{0};

  /// @brief 制約節のリテラル数
  int mConstrLitNum{0};

  /// @brief 学習節の数
  int mLearntClauseNum{0};

  /// @brief 学習節のリテラル数
  int mLearntLitNum{0};

  /// @brief コンフリクト数
  int mConflictNum{0};

  /// @brief decision 数
  int mDecisionNum{0};

  /// @brief implication数
  int mPropagationNum{0};

  /// @brief コンフリクト数の制限
  int mConflictLimit{0};

  /// @brief 学習節の制限
  int mLearntLimit{0};

  /// @brief 計算時間(ミリ秒)
  std::chrono::milliseconds mTime;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

/// @brief 加算
inline
SatStats
operator+(const SatStats& left,
	  const SatStats& right)
{
  return SatStats(left).operator+=(right);
}

/// @brief 減算
inline
SatStats
operator-(const SatStats& left,
	  const SatStats& right)
{
  return SatStats(left).operator-=(right);
}

/// @brief MAX演算
inline
SatStats
max(const SatStats& left,
    const SatStats& right)
{
  return SatStats(left).max_assign(right);
}

END_NAMESPACE_YM_SAT

#endif // YM_SATSTATS_H
