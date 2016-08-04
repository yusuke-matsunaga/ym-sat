
/// @file YmSatPt2.cc
/// @brief YmSatPt2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "YmSatPt2.h"
#include "SatAnalyzer.h"
#include "Selecter1.h"
#include "SaUIP2.h"


BEGIN_NAMESPACE_YM_SAT

#if YMSAT_USE_LBD
const
YmSatPt2::Params kDefaultParams(0.95, 0.999, false);
#else
const
YmSatPt2::Params kDefaultParams(0.95, 0.999);
#endif


//////////////////////////////////////////////////////////////////////
// YmSatPt2
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSatPt2::YmSatPt2(const string& option) :
  mParams(kDefaultParams)
{
  set_analyzer(new SaUIP2(mgr()));
  set_selecter(new Selecter1());
}

// @brief デストラクタ
YmSatPt2::~YmSatPt2()
{
}

// @brief solve() の初期化
void
YmSatPt2::_solve_init()
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
YmSatPt2::_update_on_restart(ymuint64 restart)
{
  mRealConflictLimit = mRealConflictLimit * 1.5;
  mRealLearntLimit = mRealLearntLimit * 1.1;

  set_conflict_limit(static_cast<ymuint64>(mRealConflictLimit));
  set_learnt_limit(static_cast<ymuint64>(mRealLearntLimit));
}

// @brief 矛盾発生時の処理
void
YmSatPt2::_update_on_conflict()
{
}

#if 0
// 次の割り当てを選ぶ
SatLiteral
YmSatPt2::next_decision()
{
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

  SatVarId dvar = mgr().next_var();
  if ( dvar != kSatVarIdIllegal ) {
    SatLiteral dlit(dvar);
#if 0
    if ( true ) {
      ymuint y = mVal[vindex] >> 2;
      if ( y == 0 ) {
	return ~dlit;
      }
      else if ( y == 2 ) {
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

  return kSatLiteralX;
}
#endif

END_NAMESPACE_YM_SAT
