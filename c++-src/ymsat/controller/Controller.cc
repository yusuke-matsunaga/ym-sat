
/// @file Controller.cc
/// @brief Controller の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Controller.h"
#include "ControllerMS1.h"
#include "ControllerMS2.h"


BEGIN_NAMESPACE_YM_SAT

// @brief インスタンスを作るクラスメソッド
Controller*
Controller::new_obj(
  SatCore& core,
  const string& type
)
{
  if ( type == "minisat1" ) {
    return new ControllerMS1{core};
  }
  if ( type == "minisat2" ) {
    return new ControllerMS2{core};
  }
  cerr << type << ": Unknown type, ignored." << endl;
  return new ControllerMS2{core};
}

END_NAMESPACE_YM_SAT
