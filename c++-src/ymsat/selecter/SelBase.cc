
/// @file SelBase.cc
/// @brief SelBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SelBase.h"
#include "SatCore.h"


BEGIN_NAMESPACE_YM_SAT

// @brief 次の割り当てを選ぶ．
Literal
SelBase::next_decision()
{
  auto vid = BAD_SATVARID;

  // 一定確率でランダムな変数を選ぶ．
  std::uniform_real_distribution<double> rd_freq{0, 1.0};
  std::uniform_int_distribution<SizeType> rd_var{0, mCore.variable_num() - 1};
  if ( rd_freq(mRandGen) < mVarFreq && !mCore.var_heap().empty() ) {
    vid = rd_var(mRandGen);
    if ( mCore.eval(vid) == SatBool3::X && mCore.is_decision_variable(vid) ) {
      // mRndAssign ++
    }
  }

  while ( vid == BAD_SATVARID || mCore.eval(vid) != SatBool3::X || !mCore.is_decision_variable(vid) ) {
    if ( mCore.var_heap().empty() ) {
      return Literal::X;
    }
    // activity の高い変数を取り出す．
    vid = mCore.var_heap().pop_top();
  }

  bool inv = false;
  if ( mPhaseCache ) {
    auto val = mCore.prev_val(vid);
    if ( val != SatBool3::X ) {
      // 以前割り当てた極性を選ぶ
      if ( val == SatBool3::False ) {
	inv = true;
      }
      goto end;
    }
  }
  inv = choose_phase(vid);

end:
  return Literal::conv_from_varid(vid, inv);
}

END_NAMESPACE_YM_SAT
