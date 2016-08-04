
/// @file YmSatPt1.cc
/// @brief YmSatPt1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "YmSatPt1.h"
#include "SatAnalyzer.h"
#include "Selecter1.h"


BEGIN_NAMESPACE_YM_SAT

#if YMSAT_USE_LBD
const
YmSatPt1::Params kDefaultParams(0.95, 0.999, false);
#else
const
YmSatPt1::Params kDefaultParams(0.95, 0.999);
#endif


//////////////////////////////////////////////////////////////////////
// YmSat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSatPt1::YmSatPt1(const string& option) :
  YmSat(option),
  mParams(kDefaultParams)
{
  set_analyzer(SaFactory::gen_analyzer(mgr(), option));
  set_selecter(new Selecter1());
}

// @brief デストラクタ
YmSatPt1::~YmSatPt1()
{
}

// @brief solve() の初期化
void
YmSatPt1::_solve_init()
{
  mRealConflictLimit = 100;
  mRealLearntLimit = _clause_num() / 3.0;

  set_conflict_limit(static_cast<ymuint64>(mRealConflictLimit));
  set_learnt_limit(static_cast<ymuint64>(mRealLearntLimit));

  mgr().set_decay(mParams.mVarDecay, mParams.mClauseDecay);
}

// @brief リスタート時の処理
// @param[in] restart リスタート回数
void
YmSatPt1::_update_on_restart(ymuint64 restart)
{
  mRealConflictLimit = mRealConflictLimit * 1.5;
  mRealLearntLimit = mRealLearntLimit * 1.1;

  set_conflict_limit(static_cast<ymuint64>(mRealConflictLimit));
  set_learnt_limit(static_cast<ymuint64>(mRealLearntLimit));
}

// @brief 矛盾発生時の処理
void
YmSatPt1::_update_on_conflict()
{
  // なにもしない．
}

#if 0
// 次の割り当てを選ぶ
SatLiteral
YmSatPt1::next_decision()
{
#if 0
#if 0
  // 一定確率でランダムな変数を選ぶ．
  if ( mRandGen.real1() < mParams.mVarFreq && !heap_empty() ) {
    ymuint pos = mRandGen.int32() % mVarNum;
    SatVarId vid(pos);
    tPol pol = kPolNega;
    if ( eval(SatVarId(vid)) == kB3X ) {
      return SatLiteral(vid, pol);
    }
  }
#endif
  while ( !mVarHeap.empty() ) {
    ymuint vindex = mVarHeap.pop_top();
    if ( mVal[vindex] == conv_from_Bool3(kB3X) ) {
      // Watcher の多い方の極性を(わざと)選ぶ
      ymuint v2 = vindex * 2;
      SatVarId dvar(vindex);
      SatLiteral dlit(dvar);
      if ( mWatcherList[v2 + 0].num() >= mWatcherList[v2 + 1].num() ) {
	return dlit;
      }
      else {
	return ~dlit;
      }
    }
  }
#else
  SatVarId dvar = mgr().next_var();
  if ( dvar != kSatVarIdIllegal ) {
    SatLiteral dlit(dvar);
#if 1
    if ( true ) {
      SatBool3 pval = mgr().prev_val(dvar);
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

END_NAMESPACE_YM_SAT
