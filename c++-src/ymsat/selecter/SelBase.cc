
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
  // 一定確率でランダムな変数を選ぶ．
  std::uniform_real_distribution<double> rd_freq{0, 1.0};
  std::uniform_int_distribution<SizeType> rd_var{0, mCore.variable_num() - 1};
  if ( rd_freq(mRandGen) < mVarFreq && !mCore.var_heap().empty() ) {
    auto vid = rd_var(mRandGen);
    if ( mCore.eval(vid) == SatBool3::X ) {
      bool inv = rd_freq(mRandGen) < 0.5;
      return Literal::conv_from_varid(vid, inv);
    }
  }

  auto vid = BAD_SATVARID;
  while ( !mCore.var_heap().empty() ) {
    // activity の高い変数を取り出す．
    vid = mCore.var_heap().pop_top();
    if ( mCore.eval(vid) != SatBool3::X ) {
      // すでに確定していたらスキップする．
      // もちろん，ヒープからも取り除く．
      continue;
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

  return Literal::X;
}

END_NAMESPACE_YM_SAT
