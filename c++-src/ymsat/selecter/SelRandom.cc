
/// @file SelRandom.cc
/// @brief SelRandom の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SelRandom.h"
#include "SatCore.h"


BEGIN_NAMESPACE_YM_SAT

// @brief 極性を選ぶ
bool
SelRandom::choose_phase(
  SatVarId vid
)
{
  std::uniform_real_distribution<double> rd_freq{0, 1.0};
  return rd_freq(mRandGen) < 0.5;
}

END_NAMESPACE_YM_SAT
