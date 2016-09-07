
/// @file libym_logic/sat/ymsat/YmSat1.cc
/// @brief YmSat1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "YmSat1.h"
#include "ym/SatStats.h"
#include "SatAnalyzer.h"
#include "Selecter1.h"


BEGIN_NAMESPACE_YM_SAT

#if YMSAT_USE_LBD
const
YmSat1::Params kDefaultParams(0.95, 0.999, false);
#else
const
YmSat1::Params kDefaultParams(0.95, 0.999);
#endif

//////////////////////////////////////////////////////////////////////
// YmSat1
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSat1::YmSat1(const string& option) :
  YmSat(option),
  mParams(kDefaultParams)
{
  set_analyzer(SaFactory::gen_analyzer(mgr(), option));
  set_selecter(new Selecter1(true));
}

// @brief デストラクタ
YmSat1::~YmSat1()
{
}

// @brief solve() の初期化
void
YmSat1::_solve_init()
{
  mRealConflLimit = 100.0;
  mRealLearntLimit = _clause_num() / 3.0;

  set_conflict_limit(static_cast<ymuint64>(mRealConflLimit));
  set_learnt_limit(static_cast<ymuint64>(mRealLearntLimit));

  mgr().set_decay(mParams.mVarDecay, mParams.mClauseDecay);
}

// @brief リスタート時の処理
// @param[in] restart リスタート回数
void
YmSat1::_update_on_restart(ymuint64 restart)
{
  // 判定できなかったのでパラメータを更新して次のラウンドへ
  mRealConflLimit = mRealConflLimit * 1.5;
  mRealLearntLimit = mRealLearntLimit * 1.1;

  set_conflict_limit(static_cast<ymuint64>(mRealConflLimit));
  set_learnt_limit(static_cast<ymuint64>(mRealLearntLimit));
}

// @brief 矛盾発生時の処理
void
YmSat1::_update_on_conflict()
{
  // なにもしない．
}

END_NAMESPACE_YM_SAT
