﻿
/// @file YmSatMS2.cc
/// @brief YmSatMS2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "YmSatMS2.h"


BEGIN_NAMESPACE_YM_YMSATOLD

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
  for ( size = 1, seq = 0; size < x + 1; ++ seq, size = size * 2 + 1 ) ;

  while ( size - 1 != x ) {
    size = (size - 1) >> 1;
    -- seq;
    x = x % size;
  }

  return pow(y, seq);
}

END_NONAMESPACE

// @brief search() 用の条件パラメータの初期化を行う．
void
YmSatMS2::init_control_parameters()
{
  double restart_inc = 2.0;
  set_conflict_limit(static_cast<int>(luby(restart_inc, 0)) * 100);

  mLearntLimitD = clause_num() / 3.0;
  mLearntSizeAdjustConfl = 100.0;
  mLearntSizeAdjustInc = 1.5;
  mLearntSizeAdjustCount = static_cast<int>(mLearntSizeAdjustConfl);
  set_learnt_limit(static_cast<int>(mLearntLimitD));
}

// @brief リスタート時に制御パラメータの更新を行う．
// @param[in] restart リスタート回数
void
YmSatMS2::update_on_restart(int restart)
{
  double restart_inc = 2.0;
  set_conflict_limit(static_cast<int>(luby(restart_inc, restart)) * 100);
}

// @brief コンフリクト時に制御パラメータの更新を行う．
void
YmSatMS2::update_on_conflict()
{
  -- mLearntSizeAdjustCount;
  if ( mLearntSizeAdjustCount == 0 ) {
    mLearntSizeAdjustConfl *= mLearntSizeAdjustInc;
    mLearntSizeAdjustCount = static_cast<int>(mLearntSizeAdjustConfl);
    mLearntLimitD *= 1.1;
    set_learnt_limit(static_cast<int>(mLearntLimitD));
  }
}

// 次の割り当てを選ぶ
SatLiteral
YmSatMS2::next_decision()
{
  // 一定確率でランダムな変数を選ぶ．
  std::uniform_real_distribution<double> rd_freq(0, 1.0);
  std::uniform_int_distribution<int> rd_var(0, variable_num() - 1);
  if ( rd_freq(mRandGen) < mParams.mVarFreq && !var_heap().empty() ) {
    auto vid = rd_var(mRandGen);
    if ( eval(vid) == SatBool3::X ) {
      bool inv = rd_freq(mRandGen) < 0.5;
      return SatLiteral::conv_from_varid(vid, inv);
    }
  }

  while ( !var_heap().empty() ) {
    // activity の高い変数を取り出す．
    auto vid{var_heap().pop_top()};
    if ( eval(vid) != SatBool3::X ) {
      // すでに確定していたらスキップする．
      // もちろん，ヒープからも取り除く．
      continue;
    }

    bool inv = false;
    if ( mParams.mPhaseCache ) {
      auto val{old_val(vid)};
      if ( val != SatBool3::X ) {
	// 以前割り当てた極性を選ぶ
	if ( val == SatBool3::False ) {
	  inv = true;
	}
	goto end;
      }
    }
    {
#if 0
      auto plit{SatLiteral::conv_from_varid(vid, false)};
      auto nlit{SatLiteral::conv_from_varid(vid, true)};
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
	inv = rd_freq(mRandGen) < 0.5;
      }
#else
      inv = true; // 意味はない．
#endif
    }
  end:
    return SatLiteral::conv_from_varid(vid, inv);
  }

  return kSatLiteralX;
}

BEGIN_NONAMESPACE
// reduce_learnt_clause で用いる SatClause の比較関数
class SatClauseLess
{
public:
  bool
  operator()(SatClause* a,
	     SatClause* b)
  {
    return a->activity() < b->activity();
  }
};
END_NONAMESPACE

// @brief 学習節の整理を行なう．
void
YmSatMS2::reduce_learnt_clause()
{
  vector<SatClause*>& lc_list = learnt_clause_list();

  int n = lc_list.size();
  int n2 = n / 2;

  // 足切りのための制限値
  double abs_limit = clause_bump() / n;

  // SatClauseLess を用いて学習節をソートする．
  sort(lc_list.begin(), lc_list.end(), SatClauseLess());

  // 前半の節は基本削除する．
  // 残す節は，
  // - binary clause (今の実装では SatClause の形にはなっていない)
  // - 現在の割当の理由となっている節
  int wpos = 0;
  for ( int i = 0; i < n2; ++ i ) {
    auto clause{lc_list[i]};
    if ( !is_locked(clause) ) {
      delete_clause(clause);
    }
    else {
      lc_list[wpos] = clause;
      ++ wpos;
    }
  }

  // 残りの節はアクティビティが規定値以下の節を削除する．
  // ただし，上と同じ例外はある．
  for ( int i = n2; i < n; ++ i ) {
    auto clause{lc_list[i]};
    if ( !is_locked(clause) && clause->activity() < abs_limit ) {
      delete_clause(clause);
    }
    else {
      lc_list[wpos] = clause;
      ++ wpos;
    }
  }

  // vector を切り詰める．
  if ( wpos != lc_list.size() ) {
    lc_list.erase(lc_list.begin() + wpos, lc_list.end());
  }
}

END_NAMESPACE_YM_YMSATOLD
