#ifndef SELBASE_H
#define SELBASE_H

/// @file SelBase.h
/// @brief SelBase のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Selecter.h"
#include <random>


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SelBase SelBase.h "SelBase.h"
/// @brief Selecter の実装クラスの基底クラス
//////////////////////////////////////////////////////////////////////
class SelBase :
  public Selecter
{
public:

  /// @brief コンストラクタ
  SelBase(
    SatCore& core,   ///< [in] コアマネージャ
    double var_freq, ///< [in] 変数をランダムに選ぶ確率 ( 0.0 <= var_freq <= 1.0 )
    bool phase_cache ///< [in] 以前の極性を使うとき true にするフラグ
  ) : mCore{core},
      mVarFreq{var_freq},
      mPhaseCache{phase_cache}
  {
  }

  /// @brief デストラクタ
  ~SelBase() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の割り当てを選ぶ．
  Literal
  next_decision() override;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 極性を選ぶ
  virtual
  bool
  choose_phase(
    SatVarId vid ///< [in] 変数番号
  ) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コアマネージャ
  SatCore& mCore;

  // ランダム選択のパラメータ
  double mVarFreq;

  // 以前に割り当てられた極性を使うとき true
  bool mPhaseCache;

  // 乱数発生器
  std::mt19937 mRandGen;

};

END_NAMESPACE_YM_SAT

#endif // SELBASE_H
