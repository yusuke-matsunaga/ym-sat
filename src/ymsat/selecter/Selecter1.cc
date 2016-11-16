
/// @file Selecter1.cc
/// @brief Selecter1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "Selecter1.h"
#include "CoreMgr.h"


BEGIN_NAMESPACE_YM_SAT

// @brief Selecter1 を生成する．
// @param[in] mgr Coreマネージャ
// @param[in] phase_cache phase キャッシュフラグ
Selecter*
new_Selecter1(CoreMgr& mgr,
	      bool phase_cache)
{
  return new Selecter1(mgr, phase_cache);
}

//////////////////////////////////////////////////////////////////////
// クラス Selecter1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] mgr Coreマネージャ
// @param[in] phase_cache 以前の極性を使うヒューリスティック
Selecter1::Selecter1(CoreMgr& mgr,
		     bool phase_cache) :
  mMgr(mgr),
  mPhaseCache(phase_cache)
{
}

// @brief デストラクタ
Selecter1::~Selecter1()
{
}

// @brief 次の割り当てを選ぶ．
SatLiteral
Selecter1::next_decision()
{
  SatVarId dvar = mMgr.next_var();
  if ( dvar != kSatVarIdIllegal ) {
    SatLiteral dlit(dvar);
    if ( mPhaseCache ) {
      // 以前の割り当てを使う．
      SatBool3 pval = mMgr.prev_val(dvar);
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
