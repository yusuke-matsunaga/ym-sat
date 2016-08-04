#ifndef YMSAT_PT1_H
#define YMSAT_PT1_H

/// @file YmSatPt1.h
/// @brief YmSatPt1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "YmSat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class YmSat YmSat.h "YmSat.h"
/// @brief SatSolver の実装クラス
//////////////////////////////////////////////////////////////////////
class YmSatPt1 :
  public YmSat
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @class YmSatPt1::Params YmSat.h "YmSat.h"
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
  /// @param[in] option オプション文字列
  YmSatPt1(const string& option = string());

  /// @brief デストラクタ
  virtual
  ~YmSatPt1();


private:
  //////////////////////////////////////////////////////////////////////
  // YmSat の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief solve() の初期化
  virtual
  void
  _solve_init();

  /// @brief リスタート時の処理
  /// @param[in] restart リスタート回数
  virtual
  void
  _update_on_restart(ymuint64 restart);

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

  // 制御用パラメータ
  Params mParams;

  // 実数版の矛盾回数制限
  double mRealConflictLimit;

  // 実数版の学習節制限
  double mRealLearntLimit;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

END_NAMESPACE_YM_SAT

#endif // YMSAT_PT1_H
