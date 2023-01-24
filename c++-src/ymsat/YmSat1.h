#ifndef YMSAT1_H
#define YMSAT1_H

/// @file YmSat1.h
/// @brief YmSat1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatCore.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class YmSat1 YmSat1.h "YmSat1.h"
/// @brief SatSolver の実装クラス
//////////////////////////////////////////////////////////////////////
class YmSat1 :
  public SatCore
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @class YmSat::Params YmSat.h "YmSat.h"
  /// @brief YmSat の挙動を制御するパラメータ
  //////////////////////////////////////////////////////////////////////
  struct Params
  {
    /// @brief 変数の decay 値
    double mVarDecay{1.0};

    /// @brief 学習節の decay 値
    double mClauseDecay{1.0};

#if YMSAT_USE_LBD
    /// @brief LBD ヒューリスティックを使うとき true
    bool mUseLbd{false};
#endif

    /// @brief コンストラクタ
    Params() = default;

    /// @brief 値を指定したコンストラクタ
    Params(
      double var_decay,
      double clause_decay
#if YMSAT_USE_LBD
      , bool use_lbd
#endif
    ) : mVarDecay{var_decay},
	mClauseDecay{clause_decay}
#if YMSAT_USE_LBD
      , mUseLbd{use_lbd}
#endif
    {
    }
  };


public:

  /// @brief コンストラクタ
  YmSat1(
    const string& option = string{} ///< [in] オプション文字列
  );

  /// @brief デストラクタ
  ~YmSat1();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 制御用パラメータ
  Params mParams;

};

END_NAMESPACE_YM_SAT

#endif // YMSAT1_H
