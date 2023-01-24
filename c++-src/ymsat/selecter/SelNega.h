#ifndef SELNEGA_H
#define SELNEGA_H

/// @file SelNega.h
/// @brief SelNega のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SelBase.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SelNega SelNega.h "SelNega.h"
/// @brief 常に負極性を選ぶ
//////////////////////////////////////////////////////////////////////
class SelNega :
  public SelBase
{
public:

  /// @brief コンストラクタ
  SelNega(
    SatCore& core,   ///< [in] コアマネージャ
    double var_freq, ///< [in] 変数をランダムに選ぶ確率 ( 0.0 <= var_freq <= 1.0 )
    bool phase_cache ///< [in] 以前の極性を使うとき true にするフラグ
  ) : SelBase{core, var_freq, phase_cache}
  {
  }

  /// @brief デストラクタ
  ~SelNega() = default;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 極性を選ぶ
  bool
  choose_phase(
    SatVarId vid ///< [in] 変数番号
  ) override;

};

END_NAMESPACE_YM_SAT

#endif // SELNEGA_H
