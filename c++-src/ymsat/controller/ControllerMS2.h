﻿#ifndef CONTROLLERMS2_H
#define CONTROLLERMS2_H

/// @file ControllerMS2.h
/// @brief ControllerMS2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "Controller.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class ControllerMS2 ControllerMS2.h "ControllerMS2.h"
/// @brief MiniSat2 風の Controller
//////////////////////////////////////////////////////////////////////
class ControllerMS2 :
  public Controller
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @class ControllerMS2::Params YmSat.h "YmSat.h"
  /// @brief ControllerMS2 の挙動を制御するパラメータ
  //////////////////////////////////////////////////////////////////////
  struct Params
  {
    /// @brief 変数の decay 値
    double mVarDecay;

    /// @brief 学習節の decay 値
    double mClauseDecay;

    /// @brief 変数選択用の閾値
    double mVarFreq;

    /// @brief phase-cache ヒューリスティックを使うとき true
    bool mPhaseCache;

    /// @brief コンストラクタ
    Params() :
      mVarFreq(0.0),
      mPhaseCache(true)
    {
    }

#if YMSAT_USE_LBD
    /// @brief 値を指定したコンストラクタ
    Params(
      double var_decay,
      double clause_decay,
      bool use_lbd,
      double var_freq,
      bool phase_cache
    ) : mVarDecay{var_decay},
	mClauseDecay{clause_decay},
	mUseLbd{use_lbd},
	mVarFreq(var_freq),
	mPhaseCache(phase_cache)
    {
    }
#else
    /// @brief 値を指定したコンストラクタ
    Params(
      double var_decay,
      double clause_decay,
      double var_freq,
      bool phase_cache
    ) : mVarDecay{var_decay},
	mClauseDecay{clause_decay},
	mVarFreq(var_freq),
	mPhaseCache(phase_cache)
    {
    }
#endif
  };


public:

  /// @brief コンストラクタ
  ControllerMS2(
    SatCore& core ///< [in] Coreマネージャ
  );

  /// @brief デストラクタ
  ~ControllerMS2();


private:
  //////////////////////////////////////////////////////////////////////
  // Controller の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief solve() の初期化
  void
  _init() override;

  /// @brief リスタート時の処理
  void
  _update_on_restart(
    SizeType restart ///< [in] リスタート回数
  ) override;

  /// @brief 矛盾発生時の処理
  void
  _update_on_conflict() override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Coreマネージャ
  SatCore& mCore;

  // 制御用のパラメータ
  Params mParams;

  // mLearntLimit の計算用の値
  double mLearntLimitD;

  // mLearntLimit 用のパラメータ
  // mLearntSizeAdjustCount の初期値
  double mLearntSizeAdjustConfl;

  // mLearntLimit 用のパラメータ
  // mLearntSizeAdjustConfl の増加量
  double mLearntSizeAdjustInc;

  // mLearntLimit 用のパラメータ
  // 矛盾の数がこの回数になった時に mLearntLimit を更新する．
  int mLearntSizeAdjustCount;

};

END_NAMESPACE_YM_SAT

#endif // CONTROLLERMS2_H
