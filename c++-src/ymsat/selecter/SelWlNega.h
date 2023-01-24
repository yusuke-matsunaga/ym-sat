#ifndef SELWLNEGA_H
#define SELWLNEGA_H

/// @file SelWlNega.h
/// @brief SelWlNega のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SelBase.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SelWlNega SelWlNega.h "SelWlNega.h"
/// @brief Watcher List の要素数の少ない極性を選ぶ Selecter
//////////////////////////////////////////////////////////////////////
class SelWlNega :
  public SelBase
{
public:

  /// @brief コンストラクタ
  SelWlNega(
    SatCore& core,   ///< [in] コアマネージャ
    double var_freq, ///< [in] 変数をランダムに選ぶ確率 ( 0.0 <= var_freq <= 1.0 )
    bool phase_cache ///< [in] 以前の極性を使うとき true にするフラグ
  ) : SelBase{core, var_freq, phase_cache}
  {
  }

  /// @brief デストラクタ
  ~SelWlNega() = default;


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

#endif // SELWLNEGA_H
