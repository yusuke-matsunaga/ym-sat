
/// @file Selecter2.cc
/// @brief Selecter2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "Selecter2.h"
#include "CoreMgr.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス Selecter2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Selecter2::Selecter2(
  CoreMgr& mgr
) : mMgr{mgr}
{
}

// @brief 次の割り当てを選ぶ．
Literal
Selecter2::next_decision()
{
  int dvar = mMgr.next_var();
  if ( dvar >= 0 ) {
    auto dlit = Literal::conv_from_varid(dvar, false);
    if ( true ) {
      // 以前の割り当てを使う．
      auto pval = mMgr.prev_val(dvar);
      if ( pval == kB3False ) {
	return ~dlit;
      }
      else if ( pval == kB3True ) {
	return dlit;
      }
    }
    // Watcher の多い方の極性を(わざと)選ぶ
    if ( mMgr.watcher_list(dlit).num() >= mMgr.watcher_list(~dlit).num() ) {
      return dlit;
    }
    else {
      return ~dlit;
    }
  }

  return Literal::X;
}

END_NAMESPACE_YM_SAT
