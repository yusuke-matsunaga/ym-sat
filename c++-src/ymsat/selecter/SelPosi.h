#ifndef SELPOSI_H
#define SELPOSI_H

/// @file SelPosi.h
/// @brief SelPosi のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SelBase.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SelPosi SelPosi.h "SelPosi.h"
/// @brief 常に正極性を選ぶ
//////////////////////////////////////////////////////////////////////
class SelPosi :
  public SelBase
{
public:

  /// @brief コンストラクタ
  SelPosi(
    SatCore& core,   ///< [in] コアマネージャ
    double var_freq, ///< [in] 変数をランダムに選ぶ確率 ( 0.0 <= var_freq <= 1.0 )
    bool phase_cache ///< [in] 以前の極性を使うとき true にするフラグ
  ) : SelBase{core, var_freq, phase_cache}
  {
  }

  /// @brief デストラクタ
  ~SelPosi() = default;


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

#endif // SELPOSI_H
