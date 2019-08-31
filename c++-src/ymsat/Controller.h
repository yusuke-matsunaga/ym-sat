#ifndef CONTROLLER_H
#define CONTROLLER_H

/// @file Controller.h
/// @brief Controller のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"


BEGIN_NAMESPACE_YM_SAT

class CoreMgr;

//////////////////////////////////////////////////////////////////////
/// @class Controller Controller.h "Controller.h"
/// @brief SAT の探索戦術をコントロールするクラス
///
/// このクラスはインターフェイスのみを定義した純粋仮想基底クラス
//////////////////////////////////////////////////////////////////////
class Controller
{
public:
  //////////////////////////////////////////////////////////////////////
  /// @name コンストラクタ/デストラクタ
  /// @{
  //////////////////////////////////////////////////////////////////////

  /// @brief デストラクタ
  virtual
  ~Controller() { }

  //////////////////////////////////////////////////////////////////////
  /// @}
  //////////////////////////////////////////////////////////////////////


public:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスが用意する仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief solve() の初期化
  virtual
  void
  _init() = 0;

  /// @brief リスタート時の処理
  /// @param[in] restart リスタート回数
  virtual
  void
  _update_on_restart(int restart) = 0;

  /// @brief 矛盾発生時の処理
  virtual
  void
  _update_on_conflict() = 0;

};

/// @brief MiniSat1 風のコントローラを作る．
/// @param[in] mgr コアマネージャ
Controller*
new_ControllerMS1(CoreMgr& mgr);

/// @brief MiniSat2 風のコントローラを作る．
/// @param[in] mgr コアマネージャ
Controller*
new_ControllerMS2(CoreMgr& mgr);


END_NAMESPACE_YM_SAT

#endif // CONTROLLER_H
