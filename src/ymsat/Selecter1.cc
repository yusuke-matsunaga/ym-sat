
/// @file Selecter1.cc
/// @brief Selecter1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "Selecter1.h"
#include "CoreMgr.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス Selecter1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] phase_cache 以前の極性を使うヒューリスティック
Selecter1::Selecter1(bool phase_cache)
{
  mPhaseCache = phase_cache;
}

// @brief デストラクタ
Selecter1::~Selecter1()
{
}

// @brief 次の割り当てを選ぶ．
// @param[in] mgr コアマネージャ
SatLiteral
Selecter1::next_decision(CoreMgr& mgr)
{
  SatVarId dvar = mgr.next_var();
  if ( dvar != kSatVarIdIllegal ) {
    SatLiteral dlit(dvar);
    if ( mPhaseCache ) {
      // 以前の割り当てを使う．
      SatBool3 pval = mgr.prev_val(dvar);
      if ( pval == kB3False ) {
	return ~dlit;
      }
      else if ( pval == kB3True ) {
	return dlit;
      }
    }

    return ~dlit;
  }

  return kSatLiteralX;
}

END_NAMESPACE_YM_SAT
