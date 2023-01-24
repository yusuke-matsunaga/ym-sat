
/// @file SelWlPosi.cc
/// @brief SelWlPosi の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SelWlPosi.h"
#include "SatCore.h"


BEGIN_NAMESPACE_YM_SAT

// @brief 極性を選ぶ
bool
SelWlPosi::choose_phase(
  SatVarId vid
)
{
  auto plit = Literal::conv_from_varid(vid, false);
  auto nlit = Literal::conv_from_varid(vid, true);
  // Watcher の多い方の極性を選ぶ
  bool inv = false;
  if ( mCore.watcher_list(nlit).num() >= mCore.watcher_list(plit).num() ) {
    inv = true;
  }
  return inv;
}

END_NAMESPACE_YM_SAT
