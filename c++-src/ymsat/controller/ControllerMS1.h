#ifndef CONTROLLERMS1_H
#define CONTROLLERMS1_H

/// @file ControllerMS1.h
/// @brief ControllerMS1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Controller.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class ControllerMS1 ControllerMS1.h "ControllerMS1.h"
/// @brief MiniSat-1 風のControllerの実装クラス
//////////////////////////////////////////////////////////////////////
class ControllerMS1 :
  public Controller
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @class YmSat::Params YmSat.h "YmSat.h"
  /// @brief YmSat の挙動を制御するパラメータ
  //////////////////////////////////////////////////////////////////////
  struct Params
  {
    /// @brief 変数の decay 値
    double mVarDecay;

    /// @brief 学習節の decay 値
    double mClauseDecay;

#if YMSAT_USE_LBD
    /// @brief LBD ヒューリスティックを使うとき true
    bool mUseLbd;

    /// @brief コンストラクタ
    Params() :
      mVarDecay(1.0),
      mClauseDecay(1.0),
      mUseLbd(false)
    {
    }

    /// @brief 値を指定したコンストラクタ
    Params(double var_decay,
	   double clause_decay,
	   bool use_lbd) :
      mVarDecay(var_decay),
      mClauseDecay(clause_decay),
      mUseLbd(use_lbd)
    {
    }
#else

    /// @brief コンストラクタ
    Params() :
      mVarDecay(1.0),
      mClauseDecay(1.0)
    {
    }

    /// @brief 値を指定したコンストラクタ
    Params(double var_decay,
	   double clause_decay) :
      mVarDecay(var_decay),
      mClauseDecay(clause_decay)
    {
    }
#endif
  };


public:

  /// @brief コンストラクタ
  ControllerMS1(
    SatCore& core ///< [in] Coreマネージャ
  );

  /// @brief デストラクタ
  ~ControllerMS1();


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

  // 制御用パラメータ
  Params mParams;

  // 実数版の矛盾回数の制限
  double mRealConflLimit;

  // 実数版の学習節の制限
  double mRealLearntLimit;

};

END_NAMESPACE_YM_SAT

#endif // CONTROLLERMS1_H
