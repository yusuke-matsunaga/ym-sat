
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
Selecter*
new_Selecter1(
  CoreMgr& mgr,
  bool phase_cache
)
{
  return new Selecter1{mgr, phase_cache};
}


//////////////////////////////////////////////////////////////////////
// クラス Selecter1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
Selecter1::Selecter1(
  CoreMgr& mgr,
  bool phase_cache
) : mMgr{mgr},
    mPhaseCache{phase_cache}
{
}

// @brief デストラクタ
Selecter1::~Selecter1()
{
}

// @brief 次の割り当てを選ぶ．
Literal
Selecter1::next_decision()
{
  auto dvar = mMgr.next_var();
  if ( dvar != BAD_SATVARID ) {
    auto dlit = Literal::conv_from_varid(dvar, false);
    if ( mPhaseCache ) {
      // 以前の割り当てを使う．
      auto pval = mMgr.prev_val(dvar);
      if ( pval == SatBool3::False ) {
	return ~dlit;
      }
      else if ( pval == SatBool3::True ) {
	return dlit;
      }
    }

    return ~dlit;
  }

  return Literal::X;
}

END_NAMESPACE_YM_SAT
