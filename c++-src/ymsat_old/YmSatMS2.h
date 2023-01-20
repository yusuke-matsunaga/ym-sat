#ifndef YMSAT1OLD_YMSATMS2_H
#define YMSAT1OLD_YMSATMS2_H

/// @file YmSatMS2.h
/// @brief YmSatMS2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "YmSat.h"


BEGIN_NAMESPACE_YM_YMSATOLD

//////////////////////////////////////////////////////////////////////
/// @class YmSatMS2 YmSatMS2.h "YmSatMS2.h"
/// @brief MiniSat2 もどきの YmSat
//////////////////////////////////////////////////////////////////////
class YmSatMS2 :
  public YmSat
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @class YmSatMS2::Params YmSat.h "YmSat.h"
  /// @brief YmSatMS2 の挙動を制御するパラメータ
  //////////////////////////////////////////////////////////////////////
  struct Params :
    public YmSat::Params
  {

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
      YmSat::Params(var_decay, clause_decay),
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
  /// @param[in] option オプション文字列
  YmSatMS2(const string& option = string());

  /// @brief デストラクタ
  virtual
  ~YmSatMS2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatSolver で定義されている仮想関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 実装用のプライベート関数
  //////////////////////////////////////////////////////////////////////

  /// @brief search() 用の条件パラメータの初期化を行う．
  void
  init_control_parameters() override;

  /// @brief リスタート時に制御パラメータの更新を行う．
  /// @param[in] restart リスタート回数
  void
  update_on_restart(
    SizeType restart
  ) override;

  /// @brief コンフリクト時に制御パラメータの更新を行う．
  void
  update_on_conflict() override;

  /// @brief 次の割り当てを選ぶ．
  ///
  /// 割り当てられる変数がない場合には Literal::X を返す．
  Literal
  next_decision() override;

  /// @brief 学習節の整理を行なう．
  ///
  /// 実際には条件が成り立った時のみ整理する．
  void
  reduce_learnt_clause() override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 制御用のパラメータ
  Params mParams;

  // 変数選択用乱数発生器
  std::mt19937 mRandGen;

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

END_NAMESPACE_YM_YMSATOLD

#endif // YMSAT1OLD_YMSATMS2_H
