
/// @file YmSatMS2.cc
/// @brief YmSatMS2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015 Yusuke Matsunaga
/// All rights reserved.


#include "YmSatMS2.h"
#include "SatAnalyzer.h"
#include "Selecter1.h"


BEGIN_NAMESPACE_YM_SAT

#if 0
#define YMSAT_VAR_FREQ 0.02
#else
#define YMSAT_VAR_FREQ 0.00
#endif

#if YMSAT_USE_LBD
const
YmSatMS2::Params kDefaultParams(0.95, 0.999, false, YMSAT_VAR_FREQ, true, false, false);
#else
const
YmSatMS2::Params kDefaultParams(0.95, 0.999, YMSAT_VAR_FREQ, true, false, false);
#endif

//////////////////////////////////////////////////////////////////////
// YmSatMS2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] option オプション文字列
YmSatMS2::YmSatMS2(const string& option) :
  YmSat(option),
  mParams(kDefaultParams)
{
  bool phase_cache = true;
  if ( option == "no_phase_cache" ) {
    phase_cache = false;
  }

  set_analyzer(SaFactory::gen_analyzer(mgr(), option));
  set_selecter(new Selecter1(phase_cache));
}

// @brief デストラクタ
YmSatMS2::~YmSatMS2()
{
}

BEGIN_NONAMESPACE

// Luby restart strategy
double
luby(double y,
     int x)
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
YmSatMS2::_solve_init()
{
  double restart_inc = 2.0;
  set_conflict_limit(static_cast<ymuint64>(luby(restart_inc, 0)) * 100);

  mLearntLimitD = clause_num() / 3.0;
  mLearntSizeAdjustConfl = 100.0;
  mLearntSizeAdjustInc = 1.5;
  mLearntSizeAdjustCount = static_cast<ymuint>(mLearntSizeAdjustConfl);
  set_learnt_limit(static_cast<ymuint64>(mLearntLimitD));

  mgr().set_decay(mParams.mVarDecay, mParams.mClauseDecay);
}

// @brief リスタート時の処理
// @param[in] restart リスタート回数
void
YmSatMS2::_update_on_restart(ymuint64 restart)
{
  double restart_inc = 2.0;
  set_conflict_limit(static_cast<ymuint64>(luby(restart_inc, restart)) * 100);
}

// @brief 矛盾発生時の処理
void
YmSatMS2::_update_on_conflict()
{
  -- mLearntSizeAdjustCount;
  if ( mLearntSizeAdjustCount == 0 ) {
    mLearntSizeAdjustConfl *= mLearntSizeAdjustInc;
    mLearntSizeAdjustCount = static_cast<ymuint>(mLearntSizeAdjustConfl);
    mLearntLimitD *= 1.1;
    set_learnt_limit(static_cast<ymuint64>(mLearntLimitD));
  }
}

END_NAMESPACE_YM_SAT
