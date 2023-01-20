
/// @file YmSat_base.cc
/// @brief YmSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "YmSat.h"
#include "ym/SatStats.h"
#include "ym/SatMsgHandler.h"
#include "Analyzer.h"
#include "Clause.h"


BEGIN_NAMESPACE_YM_YMSATOLD

#if YMSAT_USE_LBD
const
YmSat::Params kDefaultParams{0.95, 0.999, false};
#else
const
YmSat::Params kDefaultParams{0.95, 0.999};
#endif

//////////////////////////////////////////////////////////////////////
// YmSat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSat::YmSat(
  const string& option
) :
  mSane(true),
  mConstrClauseNum(0),
  mConstrLitNum(0),
  mLearntBinNum(0),
  mLearntLitNum(0),
  mVarNum(0),
  mOldVarNum(0),
  mVarSize(0),
  mVal(nullptr),
  mDecisionLevel(nullptr),
  mReason(nullptr),
  mWatcherList(nullptr),
#if YMSAT_USE_WEIGHTARRAY
  mWeightArray(nullptr),
#endif
#if YMSAT_USE_LBD
  mLbdTmp(nullptr),
  mLbdTmpSize(0),
#endif
  mRootLevel(0),
  mClauseBump(1.0),
  mClauseDecay(1.0),
  mParams(kDefaultParams),
  mConflictNum(0),
  mDecisionNum(0),
  mPropagationNum(0),
  mConflictLimit(0),
  mLearntLimit(0),
  mMaxConflict(1024 * 100)
{
  mAnalyzer = SaFactory::gen_analyzer(this, option);

  mSweep_assigns = -1;
  mSweep_props = 0;

#if YMSAT_USE_LBD
  mLbdTmpSize = 1024;
  mLbdTmp = new bool[mLbdTmpSize];
#endif

  mTmpLitsSize = 1024;
  mTmpLits = new Literal[mTmpLitsSize];

  mTmpBinClause = new_clause(2);

  mTimerOn = false;
}

// @brief デストラクタ
YmSat::~YmSat()
{
  for ( auto c: mConstrClauseList ) {
    delete_clause(c);
  }
  for ( auto c: mLearntClauseList ) {
    delete_clause(c);
  }
  for ( int i = 0; i < mVarSize * 2; ++ i ) {
    mWatcherList[i].finish();
  }

  delete mAnalyzer;
  delete [] mVal;
  delete [] mDecisionLevel;
  delete [] mReason;
  delete [] mWatcherList;
#if YMSAT_USE_WEIGHTARRAY
  delete [] mWeightArray;
#endif
#if YMSAT_USE_LBD
  delete [] mLbdTmp;
#endif
  delete [] mTmpLits;
}

// @brief 正しい状態のときに true を返す．
bool
YmSat::sane() const
{
  return mSane;
}

// @brief 変数を追加する．
SatVarId
YmSat::new_variable(
  bool decision
)
{
  if ( decision_level() != 0 ) {
    // エラー
    cout << "Error!: decision_level() != 0" << endl;
    ASSERT_NOT_REACHED;
    return BAD_SATVARID;
  }

#if YMSAT_USE_DVAR
  mDvarArray.push_back(decision);
#endif

  // ここではカウンタを増やすだけ
  // 実際の処理は alloc_var() でまとめて行う．
  SatVarId n = mVarNum;
  ++ mVarNum;
  return n;
}

// @brief リテラルを 'フリーズ' する．
//
// lingeling 以外は無効
void
YmSat::freeze_literal(
  SatLiteral lit
)
{
}

// @brief 節を追加する．
void
YmSat::add_clause(
  const vector<SatLiteral>& lits
)
{
  if ( decision_level() != 0 ) {
    // エラー
    cout << "Error![YmSat]: decision_level() != 0" << endl;
    ASSERT_NOT_REACHED;
    return;
  }

  if ( !mSane ) {
    // すでに充足不能になっていたらなにもしない．
    return;
  }

  // あとでリテラルの並び替えを行うので
  // 一旦 mTmpLits にコピーする．
  SizeType lit_num = lits.size();
  alloc_lits(lit_num);
  for ( SizeType i = 0; i < lit_num; ++ i ) {
    auto l = lits[i];
    mTmpLits[i] = Literal{l};
  }

  // 節を追加する本体
  // 変数領域の確保を行う．
  alloc_var();

  // mTmpLits をソートする．
  // たぶん要素数が少ないので挿入ソートが速いはず．
  for ( SizeType i = 1; i < lit_num; ++ i ) {
    // この時点で [0 : i - 1] までは整列している．
    auto l = mTmpLits[i];
    if ( mTmpLits[i - 1].index() <= l.index() ) {
      // このままで [0 : i] まで整列していることになる．
      continue;
    }

    // l の挿入位置を探す．
    SizeType j = i;
    for ( ; ; ) {
      mTmpLits[j] = mTmpLits[j - 1];
      -- j;
      if ( j == 0 || mTmpLits[j - 1].index() <= l.index() ) {
	// 先頭に達するか，l よりも小さい要素があった．
	break;
      }
    }
    mTmpLits[j] = l;
  }

  // - 重複したリテラルの除去
  //   整列したのでおなじリテラルは並んでいるはず．
  // - false literal の除去
  // - true literal を持つかどうかのチェック
  SizeType wpos = 0;
  for ( SizeType rpos = 0; rpos < lit_num; ++ rpos ) {
    auto l = mTmpLits[rpos];
    if ( wpos != 0 ) {
      auto l1 = mTmpLits[wpos - 1];
      if ( l1 == l ) {
	// 重複している．
	continue;
      }
      if ( l1.varid() == l.varid() ) {
	// 同じ変数の相反するリテラル
	// この節は常に充足する．
	return;
      }
    }

    auto v = eval(l);
    if ( v == SatBool3::False ) {
      // false literal は追加しない．
      continue;
    }
    if ( v == SatBool3::True ) {
      // true literal があったら既に充足している
      return;
    }
    if ( l.varid() < 0 || l.varid() >= mVarNum ) {
      // 範囲外
      // new_variable() で確保した変数番号よりも大きい変数番号が
      // 使われていた．
      // TODO: エラー対策．
      cout << "Error![YmSat]: literal(" << l << "): out of range"
	   << endl;
      ASSERT_NOT_REACHED;
      return;
    }
    // 追加する．
    mTmpLits[wpos] = l;
    ++ wpos;
  }
  lit_num = wpos;

  mConstrLitNum += lit_num;

  if ( lit_num == 0 ) {
    // empty clause があったら unsat
    mSane = false;
    return;
  }

  auto l0 = mTmpLits[0];
  if ( lit_num == 1 ) {
    // unit clause があったら値の割り当てを行う．
    bool stat = check_and_assign(l0);

    if ( debug & debug_assign ) {
      cout << "\tassign " << l0 << " @" << decision_level()
	   << endl;
      if ( !stat )  {
	cout << "\t--> conflict with previous assignment" << endl
	     << "\t    " << ~l0 << " was assigned at level "
	     << decision_level(l0.varid()) << endl;
      }
    }

    if ( stat ) {
      // 今の割当に基づく含意を行う．
      auto conflict = implication();
      if ( conflict != Reason::None ) {
	// 矛盾が起こった．
	stat = false;
      }
    }

    if ( !stat ) {
      // 矛盾が起こった．
      mSane = false;
      // ここは decision_level() == 0 のはずなのでバックトラックの必要はない．
    }
    return;
  }

#if YMSAT_USE_WEIGHTARRAY
  for ( SizeType i = 0; i < lit_num; ++ i ) {
    auto l = mTmpLits[i];
    auto index = l.index();
    mWeightArray[index] += 1.0 / static_cast<double>(lit_num);
  }
#endif

  ++ mConstrClauseNum;

  auto l1 = mTmpLits[1];

  if ( lit_num == 2 ) {
    // 二項節の watcher は相方のリテラルなので clause は使われない

    if ( debug & debug_assign ) {
      cout << "add_clause: " << l0 << " + " << l1 << endl;
    }

    mConstrBinList.push_back(BinClause(l0, l1));

    // watcher-list の設定
    add_watcher(~l0, Reason{l1});
    add_watcher(~l1, Reason{l0});
  }
  else {
    // 節の生成
    auto clause = new_clause(lit_num);

    if ( debug & debug_assign ) {
      cout << "add_clause: " << *clause << endl;
    }

    mConstrClauseList.push_back(clause);

    // watcher-list の設定
    add_watcher(~l0, Reason{clause});
    add_watcher(~l1, Reason{clause});
  }
}

// @brief 現在の内部状態を得る．
SatStats
YmSat::get_stats() const
{
  SatStats stats;
  stats.mRestart = mRestart;
  stats.mVarNum = mVarNum;
  stats.mConstrClauseNum = mConstrClauseNum;
  stats.mConstrLitNum = mConstrLitNum;
  stats.mLearntClauseNum = mLearntClauseList.size() + mLearntBinNum;
  stats.mLearntLitNum = mLearntLitNum;
  stats.mConflictNum = mConflictNum;
  stats.mDecisionNum = mDecisionNum;
  stats.mPropagationNum = mPropagationNum;
  stats.mConflictLimit = mConflictLimit;
  stats.mLearntLimit = mLearntLimit;
  stats.mTime = mAccTime;
  return stats;
}

// @brief conflict_limit の最大値
SizeType
YmSat::set_max_conflict(
  SizeType val
)
{
  auto old_val = mMaxConflict;
  mMaxConflict = val;
  return old_val;
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
void
YmSat::reg_msg_handler(
  SatMsgHandler* msg_handler
)
{
  mMsgHandlerList.push_back(msg_handler);
}

// @brief 時間計測機能を制御する
void
YmSat::timer_on(
  bool enable
)
{
  mTimerOn = enable;
}

// 学習節を追加する．
void
YmSat::add_learnt_clause(
  const vector<Literal>& learnt_lits
)
{
  // learnt_lits の 0 番目のリテラルに値の割り当てが生じる．
  // 残りのリテラルがその割り当ての原因となる．

  SizeType n = learnt_lits.size();
  mLearntLitNum += n;

  if ( n == 0 ) {
    // empty clause があったら unsat
    mSane = false;
    return;
  }

  auto l0 = learnt_lits[0];
  if ( n == 1 ) {
    // unit clause があったら値の割り当てを行う．
    bool stat = check_and_assign(l0);
    if ( debug & debug_assign ) {
      cout << "\tassign " << l0 << " @" << decision_level()
	   << endl;
      if ( !stat )  {
	cout << "\t--> conflict with previous assignment" << endl
	     << "\t    " << ~l0 << " was assigned at level "
	     << decision_level(l0.varid()) << endl;
      }
    }
    if ( !stat ) {
      mSane = false;
    }
    return;
  }

  Reason reason;
  auto l1 = learnt_lits[1];
  if ( n == 2 ) {
    // binary-clause の場合

    if ( debug & debug_assign ) {
      cout << "add_learnt_clause: "
	   << "(" << l0 << " + " << l1 << ")" << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << " from (" << l0 << " + " << l1 << ")" << endl;
    }

    // watcher-list の設定
    add_watcher(~l0, Reason{l1});
    add_watcher(~l1, Reason{l0});

    reason = Reason{l1};

    ++ mLearntBinNum;
  }
  else {
    // 節の生成
    alloc_lits(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      mTmpLits[i] = learnt_lits[i];
    }
    auto clause = new_clause(n, true);

    if ( debug & debug_assign ) {
      cout << "add_learnt_clause: " << *clause << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << " from " << *clause << endl;
    }

    bump_clause_activity(clause);

#if YMSAT_USE_LBD
    if ( mParams.mUseLbd ) {
      // LBD の計算
      int lbd = calc_lbd(clause);
      clause->set_lbd(lbd);
    }
#endif

    mLearntClauseList.push_back(clause);

    reason = Reason{clause};

    // watcher-list の設定
    add_watcher(~l0, reason);
    add_watcher(~l1, reason);
  }

  // learnt clause の場合には必ず unit clause になっているはず．
  ASSERT_COND( eval(l0) != SatBool3::False );

  assign(l0, reason);
}

// @brief mTmpLits を確保する．
void
YmSat::alloc_lits(
  SizeType lit_num
)
{
  auto old_size = mTmpLitsSize;
  while ( mTmpLitsSize <= lit_num ) {
    mTmpLitsSize <<= 1;
  }
  if ( old_size < mTmpLitsSize ) {
    delete [] mTmpLits;
    mTmpLits = new Literal[mTmpLitsSize];
  }
}

// @brief 新しい節を生成する．
Clause*
YmSat::new_clause(
  SizeType lit_num,
  bool learnt
)
{
  SizeType size = sizeof(Clause) + sizeof(Literal) * (lit_num - 1);

  auto p = new char[size];
  auto clause = new (p) Clause(lit_num, mTmpLits, learnt);

  return clause;
}

// @brief 節を削除する．
void
YmSat::delete_clause(
  Clause* clause
)
{
  if ( debug & debug_assign ) {
    cout << " delete_clause: " << (*clause) << endl;
  }

  // watch list を更新
  del_watcher(~clause->wl0(), Reason{clause});
  del_watcher(~clause->wl1(), Reason{clause});

  if ( clause->is_learnt() ) {
    mLearntLitNum -= clause->lit_num();
  }
  else {
    mConstrLitNum -= clause->lit_num();
  }

  auto p = reinterpret_cast<char*>(clause);
  delete [] p;
}

// @brief watcher を削除する．
void
YmSat::del_watcher(
  Literal watch_lit,
  Reason reason
)
{
  // watch_lit に関係する watcher リストから
  // reason を探して削除する．
  // watcher リストを配列で実装しているので
  // あたまからスキャンして該当の要素以降を
  // 1つづつ前に詰める．
  auto w0 = Watcher{reason};
  auto& wlist = watcher_list(watch_lit);
  SizeType n = wlist.num();
  SizeType wpos = 0;
  for ( ; wpos < n; ++ wpos) {
    auto w = wlist.elem(wpos);
    if ( w == w0 ) {
      break;
    }
  }
  ASSERT_COND( wpos < n );
  -- n;
  for ( ; wpos < n; ++ wpos) {
    auto w = wlist.elem(wpos + 1);
    wlist.set_elem(wpos, w);
  }
  wlist.erase(n);
}

// @brief 充足された watcher を削除する．
void
YmSat::del_satisfied_watcher(
  Literal watch_lit
)
{
  // watch_lit に関係する watcher リストをスキャンして
  // literal watcher が充足していたらその watcher を削除する．
  // watcher リストを配列で実装しているので
  // あたまからスキャンして該当の要素以降を
  // 1つづつ前に詰める．
  auto& wlist = watcher_list(watch_lit);
  SizeType n = wlist.num();
  SizeType wpos = 0;
  for ( SizeType rpos = 0; rpos < n; ++ rpos ) {
    auto w = wlist.elem(rpos);
    if ( w.is_literal() ) {
      auto l = w.literal();
      auto val = eval(l);
      if ( val == SatBool3::True ) {
	// この watcher は削除する．
	continue;
      }
    }
    if ( wpos != rpos ) {
      wlist.set_elem(wpos, w);
    }
    ++ wpos;
  }
  wlist.erase(wpos);
}

// 実際に変数に関するデータ構造を生成する．
void
YmSat::alloc_var()
{
  if ( mOldVarNum < mVarNum ) {
    if ( mVarSize < mVarNum ) {
      expand_var();
    }
    for ( SizeType i = mOldVarNum; i < mVarNum; ++ i ) {
      mVal[i] = conv_from_Bool3(SatBool3::X) | (conv_from_Bool3(SatBool3::X) << 2);
#if YMSAT_USE_WEIGHTARRAY
      mWeightArray[i * 2 + 0] = 0.0;
      mWeightArray[i * 2 + 1] = 0.0;
#endif
      mVarHeap.add_var(i);
    }
    mOldVarNum = mVarNum;
  }
}

// 変数に関する配列を拡張する．
void
YmSat::expand_var()
{
  auto old_size = mVarSize;
  auto old_val = mVal;
  auto old_decision_level = mDecisionLevel;
  auto old_reason = mReason;
  auto old_watcher_list = mWatcherList;
#if YMSAT_USE_WEIGHTARRAY
  auto old_weight_array = mWeightArray;
#endif
  if ( mVarSize == 0 ) {
    mVarSize = 1024;
  }
  while ( mVarSize < mVarNum ) {
    mVarSize <<= 1;
  }

  mVal = new ymuint8[mVarSize];
  mDecisionLevel = new int[mVarSize];
  mReason = new Reason[mVarSize];
  mWatcherList = new WatcherList[mVarSize * 2];
#if YMSAT_USE_WEIGHTAARAY
  mWeightArray = new double[mVarSize * 2];
#endif

  for ( SizeType i = 0; i < mOldVarNum; ++ i ) {
    mVal[i] = old_val[i];
    mDecisionLevel[i] = old_decision_level[i];
    mReason[i] = old_reason[i];
  }
  SizeType n2 = mOldVarNum * 2;
  for ( SizeType i = 0; i < n2; ++ i ) {
    mWatcherList[i].move(old_watcher_list[i]);
#if YMSAT_USE_WEIGHTARRAY
    mWeightArray[i] = old_weight_array[i];
#endif
  }

  if ( old_size > 0 ) {
    delete [] old_val;
    delete [] old_decision_level;
    delete [] old_reason;
    delete [] old_watcher_list;
#if YMSAT_USE_WEIGHTARRAY
    delete [] old_weight_array;
#endif
  }

  mAssignList.reserve(mVarSize);
  mVarHeap.alloc_var(mVarSize);
  mAnalyzer->alloc_var(mVarSize);
}

END_NAMESPACE_YM_YMSATOLD
