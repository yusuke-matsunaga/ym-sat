
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
  if ( option == "no_phase_cache" ) {
    mParams.mPhaseCache = false;
  }

  set_analyzer(SaFactory::gen_analyzer(mgr(), option));
  set_selecter(new Selecter1());
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

#if 0
// 次の割り当てを選ぶ
SatLiteral
YmSatMS2::next_decision()
{
#if 0
  // 一定確率でランダムな変数を選ぶ．
  if ( mRandGen.real1() < mParams.mVarFreq && !var_heap().empty() ) {
    ymuint pos = mRandGen.int32() % variable_num();
    SatVarId vid(pos);
    if ( eval(vid) == kB3X ) {
      bool inv = mRandGen.real1() < 0.5;
      return SatLiteral(vid, inv);
    }
  }
#endif

#if 0
  while ( !var_heap().empty() ) {
    // activity の高い変数を取り出す．
    ymuint vindex = var_heap().pop_top();
    SatVarId vid(vindex);
    if ( eval(vid) != kB3X ) {
      // すでに確定していたらスキップする．
      // もちろん，ヒープからも取り除く．
      continue;
    }

    bool inv = false;
    if ( mParams.mPhaseCache ) {
      SatBool3 val = prev_val(vid);
      if ( val != kB3X ) {
	// 以前割り当てた極性を選ぶ
	if ( val == kB3False ) {
	  inv = true;
	}
	goto end;
      }
    }
    {
#if 0
      SatLiteral plit(SatVarId(vindex), false);
      SatLiteral nlit(SatVarId(vindex), true);
      if ( mParams.mWlPosi ) {
	// Watcher の多い方の極性を(わざと)選ぶ
	if ( watcher_list(nlit).num() >= watcher_list(plit).num() ) {
	  inv = true;
	}
      }
      else if ( mParams.mWlNega ) {
	// Watcher の少ない方の極性を選ぶ
	if ( watcher_list(nlit).num() < watcher_list(plit).num() ) {
	  inv = true;
	}
      }
      else {
	// mWlPosi/mWlNega が指定されていなかったらランダムに選ぶ．
	inv = mRandGen.real1() < 0.5;
      }
#else
      inv = true; // 意味はない．
#endif
    }
  end:
    return SatLiteral(SatVarId(vindex), inv);
  }
#else

  SatVarId dvar = next_var();
  if ( dvar != kSatVarIdIllegal ) {
    SatLiteral dlit(dvar);
#if 1
    if ( true ) {
      SatBool3 pval = prev_val(dvar);
      if ( pval == kB3False ) {
	return ~dlit;
      }
      else if ( pval == kB3True ) {
	return dlit;
      }
    }
#endif
    // Watcher の多い方の極性を(わざと)選ぶ
    if ( watcher_list(dlit).num() >= watcher_list(~dlit).num() ) {
      return dlit;
    }
    else {
      return ~dlit;
    }
  }

#endif

  return kSatLiteralX;
}
#endif

#if YMSAT_USE_LBD
// @brief LBD を計算する．
// @param[in] clause 対象の節
ymuint
YmSat::calc_lbd(const SatClause* clause)
{
  // 割当レベルの最大値 + 1 だけ mLbdTmp を確保する．
  ymuint max_level = decision_level() + 1;
  ymuint32 old_size = mLbdTmpSize;
  while ( mLbdTmpSize < max_level ) {
    mLbdTmpSize <<= 1;
  }
  if ( mLbdTmpSize != old_size ) {
    delete [] mLbdTmp;
    mLbdTmp = new bool[mLbdTmpSize];
  }

  ymuint n = clause->lit_num();

  // mLbdTmp をクリア
  // ただし， clause に現れるリテラルのレベルだけでよい．
  for (ymuint i = 0; i < n; ++ i) {
    SatLiteral l = clause->lit(i);
    SatVarId v = l.varid();
    ymuint level = decision_level(v);
    mLbdTmp[level] = false;
  }

  // 異なる決定レベルの個数を数える．
  ymuint c = 0;
  for (ymuint i = 0; i < n; ++ i) {
    SatLiteral l = clause->lit(i);
    SatVarId v = l.varid();
    ymuint level = decision_level(v);
    if ( !mLbdTmp[level] ) {
      // はじめてこのレベルの変数が現れた．
      mLbdTmp[level] = true;
      ++ c;
    }
  }

  return c;
}
#endif

END_NAMESPACE_YM_SAT
