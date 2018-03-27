#ifndef CONTROLLERMS2_H
#define CONTROLLERMS2_H

/// @file ControllerMS2.h
/// @brief ControllerMS2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "Controller.h"


BEGIN_NAMESPACE_YM_SAT

class CoreMgr;

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

    /// @brief watcher list の多い極性を選ぶヒューリスティックを使うとき true
    bool mWlPosi;

    /// @brief watcher list の少ない極性を選ぶヒューリスティックを使うとき true
    bool mWlNega;

    /// @brief コンストラクタ
    Params() :
      mVarFreq(0.0),
      mPhaseCache(true),
      mWlPosi(false),
      mWlNega(false)
    {
    }

#if YMSAT_USE_LBD
    /// @brief 値を指定したコンストラクタ
    Params(double var_decay,
	   double clause_decay,
	   bool use_lbd,
	   double var_freq,
	   bool phase_cache,
	   bool wl_posi,
	   bool wl_nega) :
      YmSat::Params(var_decay, clause_decay, use_lbd),
      mVarFreq(var_freq),
      mPhaseCache(phase_cache),
      mWlPosi(wl_posi),
      mWlNega(!wl_posi && wl_nega)
    {
    }
#else
    /// @brief 値を指定したコンストラクタ
    Params(double var_decay,
	   double clause_decay,
	   double var_freq,
	   bool phase_cache,
	   bool wl_posi,
	   bool wl_nega) :
      mVarDecay(var_decay),
      mClauseDecay(clause_decay),
      mVarFreq(var_freq),
      mPhaseCache(phase_cache),
      mWlPosi(wl_posi),
      mWlNega(!wl_posi && wl_nega)
    {
    }
#endif
  };


public:

  /// @brief コンストラクタ
  /// @param[in] mgr Coreマネージャ
  ControllerMS2(CoreMgr& mgr);

  /// @brief デストラクタ
  virtual
  ~ControllerMS2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatSolver で定義されている仮想関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // YmSat の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief solve() の初期化
  virtual
  void
  _init();

  /// @brief リスタート時の処理
  /// @param[in] restart リスタート回数
  virtual
  void
  _update_on_restart(int restart);

  /// @brief 矛盾発生時の処理
  virtual
  void
  _update_on_conflict();


private:
  //////////////////////////////////////////////////////////////////////
  // 実装用のプライベート関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Coreマネージャ
  CoreMgr& mMgr;

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
