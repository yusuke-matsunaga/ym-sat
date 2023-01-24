
/// @file ControllerMS2.cc
/// @brief ControllerMS2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ControllerMS2.h"
#include "SatCore.h"


BEGIN_NAMESPACE_YM_SAT

#if 0
#define YMSAT_VAR_FREQ 0.02
#else
#define YMSAT_VAR_FREQ 0.00
#endif

#if YMSAT_USE_LBD
const
ControllerMS2::Params kDefaultParams{ 0.95, 0.999, false, YMSAT_VAR_FREQ, true };
#else
const
ControllerMS2::Params kDefaultParams{ 0.95, 0.999, YMSAT_VAR_FREQ, true };
#endif


//////////////////////////////////////////////////////////////////////
// ControllerMS2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ControllerMS2::ControllerMS2(
  SatCore& core
) : mCore{core},
    mParams{kDefaultParams}
{
}

// @brief デストラクタ
ControllerMS2::~ControllerMS2()
{
}

BEGIN_NONAMESPACE

// Luby restart strategy
double
luby(
  double y,
  int x
)
{
  // なんのこっちゃわかんないコード
  int size;
  int seq;
  for (size = 1, seq = 0; size < x + 1; ++ seq, size = size * 2 + 1) ;

  while ( size - 1 != x ) {
    size = (size - 1) >> 1;
    -- seq;
    x = x % size;
  }

  return pow(y, seq);
}

END_NONAMESPACE


// @brief solve() の初期化
void
ControllerMS2::_init()
{
  double restart_inc = 2.0;
  mCore.set_conflict_limit(static_cast<int>(luby(restart_inc, 0)) * 100);

  mLearntLimitD = mCore.clause_num() / 3.0;
  mLearntSizeAdjustConfl = 100.0;
  mLearntSizeAdjustInc = 1.5;
  mLearntSizeAdjustCount = static_cast<int>(mLearntSizeAdjustConfl);

  mCore.set_learnt_limit(static_cast<int>(mLearntLimitD));

  mCore.set_decay(mParams.mVarDecay, mParams.mClauseDecay);
}

// @brief リスタート時の処理
void
ControllerMS2::_update_on_restart(
  SizeType restart
)
{
  double restart_inc = 2.0;
  mCore.set_conflict_limit(static_cast<int>(luby(restart_inc, restart)) * 100);
}

// @brief 矛盾発生時の処理
void
ControllerMS2::_update_on_conflict()
{
  -- mLearntSizeAdjustCount;
  if ( mLearntSizeAdjustCount == 0 ) {
    mLearntSizeAdjustConfl *= mLearntSizeAdjustInc;
    mLearntSizeAdjustCount = static_cast<int>(mLearntSizeAdjustConfl);
    mLearntLimitD *= 1.1;
    mCore.set_learnt_limit(static_cast<int>(mLearntLimitD));
  }
}

END_NAMESPACE_YM_SAT
