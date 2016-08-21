
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

// @brief 次の割り当てを選ぶ．
// @param[in] mgr コアマネージャ
SatLiteral
Selecter2::next_decision(CoreMgr& mgr)
{
  SatVarId dvar = mgr.next_var();
  if ( dvar != kSatVarIdIllegal ) {
    SatLiteral dlit(dvar);
#if 1
    if ( true ) {
      // 以前の割り当てを使う．
      SatBool3 pval = mgr.prev_val(dvar);
      if ( pval == kB3False ) {
	return ~dlit;
      }
      else if ( pval == kB3True ) {
	return dlit;
      }
    }
#endif
    // Watcher の多い方の極性を(わざと)選ぶ
    if ( mgr.watcher_list(dlit).num() >= mgr.watcher_list(~dlit).num() ) {
      return dlit;
    }
    else {
      return ~dlit;
    }
  }

  return kSatLiteralX;
}

END_NAMESPACE_YM_SAT
