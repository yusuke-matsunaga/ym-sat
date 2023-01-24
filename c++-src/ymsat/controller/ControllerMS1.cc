
/// @file ControllerMS1.cc
/// @brief ControllerMS1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ControllerMS1.h"
#include "SatCore.h"


BEGIN_NAMESPACE_YM_SAT

#if YMSAT_USE_LBD
const
ControllerMS1::Params kDefaultParams(0.95, 0.999, false);
#else
const
ControllerMS1::Params kDefaultParams(0.95, 0.999);
#endif


//////////////////////////////////////////////////////////////////////
// ControllerMS1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ControllerMS1::ControllerMS1(
  SatCore& core
) : mCore{core},
    mParams{kDefaultParams}
{
}

// @brief デストラクタ
ControllerMS1::~ControllerMS1()
{
}

// @brief solve() の初期化
void
ControllerMS1::_init()
{
  mRealConflLimit = 100.0;
  mRealLearntLimit = mCore.clause_num() / 3.0;

  mCore.set_conflict_limit(static_cast<int>(mRealConflLimit));
  mCore.set_learnt_limit(static_cast<int>(mRealLearntLimit));
  mCore.set_decay(mParams.mVarDecay, mParams.mClauseDecay);
}

// @brief リスタート時の処理
void
ControllerMS1::_update_on_restart(
  SizeType restart
)
{
  // 判定できなかったのでパラメータを更新して次のラウンドへ
  mRealConflLimit = mRealConflLimit * 1.5;
  mRealLearntLimit = mRealLearntLimit * 1.1;

  mCore.set_conflict_limit(static_cast<int>(mRealConflLimit));
  mCore.set_learnt_limit(static_cast<int>(mRealLearntLimit));
}

// @brief 矛盾発生時の処理
void
ControllerMS1::_update_on_conflict()
{
  // なにもしない．
}

END_NAMESPACE_YM_SAT
