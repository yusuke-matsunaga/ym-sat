
/// @file SelNega.cc
/// @brief SelNega の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SelNega.h"
#include "SatCore.h"


BEGIN_NAMESPACE_YM_SAT

// @brief 極性を選ぶ
bool
SelNega::choose_phase(
  SatVarId vid
)
{
  return true;
}

END_NAMESPACE_YM_SAT
