
/// @file ControllerMS1.cc
/// @brief ControllerMS1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ControllerMS1.h"
#include "CoreMgr.h"


BEGIN_NAMESPACE_YM_SAT

#if YMSAT_USE_LBD
const
ControllerMS1::Params kDefaultParams(0.95, 0.999, false);
#else
const
ControllerMS1::Params kDefaultParams(0.95, 0.999);
#endif

// @brief MiniSat1 風のコントローラを作る．
// @param[in] mgr コアマネージャ
Controller*
new_ControllerMS1(CoreMgr& mgr)
{
  return new ControllerMS1(mgr);
}

//////////////////////////////////////////////////////////////////////
// ControllerMS1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
ControllerMS1::ControllerMS1(CoreMgr& mgr) :
  mMgr(mgr),
  mParams(kDefaultParams)
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
  mRealLearntLimit = mMgr.clause_num() / 3.0;

  mMgr.set_conflict_limit(static_cast<int>(mRealConflLimit));
  mMgr.set_learnt_limit(static_cast<int>(mRealLearntLimit));
  mMgr.set_decay(mParams.mVarDecay, mParams.mClauseDecay);
}

// @brief リスタート時の処理
// @param[in] restart リスタート回数
void
ControllerMS1::_update_on_restart(int restart)
{
  // 判定できなかったのでパラメータを更新して次のラウンドへ
  mRealConflLimit = mRealConflLimit * 1.5;
  mRealLearntLimit = mRealLearntLimit * 1.1;

  mMgr.set_conflict_limit(static_cast<int>(mRealConflLimit));
  mMgr.set_learnt_limit(static_cast<int>(mRealLearntLimit));
}

// @brief 矛盾発生時の処理
void
ControllerMS1::_update_on_conflict()
{
  // なにもしない．
}

END_NAMESPACE_YM_SAT
