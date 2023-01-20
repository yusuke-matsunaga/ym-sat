#ifndef SELECTER_H
#define SELECTER_H

/// @file Selecter.h
/// @brief Selecter のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "Literal.h"


BEGIN_NAMESPACE_YM_SAT

class CoreMgr;

//////////////////////////////////////////////////////////////////////
/// @class Selecter Selecter.h "Selecter.h"
/// @brief 次の割り当てを選ぶクラス
//////////////////////////////////////////////////////////////////////
class Selecter
{
public:

  /// @brief デストラクタ
  virtual
  ~Selecter() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の割り当てを選ぶ．
  virtual
  Literal
  next_decision() = 0;

};

/// @brief Selecter1 を生成する．
Selecter*
new_Selecter1(
  CoreMgr& mgr,    ///< [in] Coreマネージャ
  bool phase_cache ///< [in] phase キャッシュフラグ
);

END_NAMESPACE_YM_SAT

#endif // SELECTER_H
