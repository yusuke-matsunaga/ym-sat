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
#include "ym/json.h"


BEGIN_NAMESPACE_YM_SAT

class SatCore;

//////////////////////////////////////////////////////////////////////
/// @class Selecter Selecter.h "Selecter.h"
/// @brief 次の割り当てを選ぶクラス
//////////////////////////////////////////////////////////////////////
class Selecter
{
public:

  /// @brief インスタンスを作るクラスメソッド
  static
  Selecter*
  new_obj(
    SatCore& core,          ///< [in] Coreマネージャ
    const JsonValue& js_obj ///< [in] 初期化パラメータ
  );

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

END_NAMESPACE_YM_SAT

#endif // SELECTER_H
