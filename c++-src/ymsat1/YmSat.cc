
/// @file YmSat.cc
/// @brief YmSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "YmSat.h"
#include "ym/SatStats.h"
#include "ym/SatModel.h"
#include "ym/SatMsgHandler.h"
#include "Analyzer.h"
#include "Clause.h"


BEGIN_NAMESPACE_YM_SAT1

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
) : mSane(true),
    mConstrBinNum(0),
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
    mMaxConflict(1024 * 10)
{
  mAnalyzer = SaFactory::gen_analyzer(this, option);

#if YMSAT_USE_LBD
  mLbdTmpSize = 1024;
  mLbdTmp = new bool[mLbdTmpSize];
#endif

  mTmpLitsSize = 1024;
  mTmpLits = new Literal[mTmpLitsSize];

  mTmpBinClause = new_clause(2);

  mRestart = 0;

  mTimerOn = false;
}

// @brief デストラクタ
YmSat::~YmSat()
{
  for ( auto c: mConstrClauseList ) {
    delete_clause(c);
  }
  for ( auto c: mLearntClause ) {
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
int
YmSat::new_variable(
  bool decision
)
{
  if ( decision_level() != 0 ) {
    // エラー
    cout << "Error!: decision_level() != 0" << endl;
    return -1;
  }

#if YMSAT_USE_DVAR
  mDvarArray.push_back(decision);
#endif

  // ここではカウンタを増やすだけ
  // 実際の処理は alloc_var() でまとめて行う．
  int n = mVarNum;
  ++ mVarNum;
  return n;
}

// @brief リテラルを 'フリーズ' する．
//
// lingeling 以外は無効
void
YmSat::freeze_literal(SatLiteral lit)
{
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
YmSat::add_clause(
  const vector<SatLiteral>& lits
)
{
  if ( decision_level() != 0 ) {
    // エラー
    cout << "Error![YmSat]: decision_level() != 0" << endl;
    return;
  }

  if ( !mSane ) {
    cout << "Error![YmSat]: mSane == false" << endl;
    return;
  }

  SizeType lit_num = lits.size();
  alloc_lits(lit_num);
  for ( int i = 0; i < lit_num; ++ i ) {
    auto l = lits[i];
    mTmpLits[i] = Literal{l};
  }

  alloc_var();

  // - 重複したリテラルの除去
  // - false literal の除去
  // - true literal を持つかどうかのチェック
  SizeType wpos = 0;
  for ( SizeType rpos = 0; rpos < lit_num; ++ rpos ) {
    auto l = mTmpLits[rpos];
    if ( wpos != 0 && mTmpLits[wpos - 1] == l ) {
      // 重複している．
      continue;
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
      cout << "Error![YmSat]: literal(" << l << "): out of range"
	   << endl;
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
    if ( !stat ) {
      mSane = false;
    }
    return;
  }

  auto l1 = mTmpLits[1];

  if ( lit_num == 2 ) {
    // watcher-list の設定
    add_watcher(~l0, Reason{l1});
    add_watcher(~l1, Reason{l0});

    ++ mConstrBinNum;
  }
  else {
    // 節の生成
    auto clause = new_clause(lit_num, mTmpLits);
    mConstrClauseList.push_back(clause);

    // watcher-list の設定
    add_watcher(~l0, Reason{clause});
    add_watcher(~l1, Reason{clause});
  }
}

// 学習節を追加する．
void
YmSat::add_learnt_clause()
{
  SizeType n = mLearntLits.size();
  mLearntLitNum += n;

  if ( n == 0 ) {
    // empty clause があったら unsat
    mSane = false;
    return;
  }

  auto l0 = mLearntLits[0];
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
  auto l1{mLearntLits[1]};
  if ( n == 2 ) {
    reason = Reason{l1};

    // watcher-list の設定
    add_watcher(~l0, Reason{l1});
    add_watcher(~l1, Reason{l0});

    ++ mLearntBinNum;
  }
  else {
    // 節の生成
    alloc_lits(n);
    for ( SizeType i = 0; i < n; ++ i ) {
      mTmpLits[i] = mLearntLits[i];
    }
    auto clause = new_clause(n, true);
    mLearntClause.push_back(clause);

    reason = Reason{clause};

    // watcher-list の設定
    add_watcher(~l0, reason);
    add_watcher(~l1, reason);
  }

  // learnt clause の場合には必ず unit clause になっているはず．
  ASSERT_COND( eval(l0) != SatBool3::False );
  if ( debug & debug_assign ) {
    cout << "\tassign " << l0 << " @" << decision_level()
	 << " from " << reason << endl;
  }

  assign(l0, reason);
}

// @brief watcher を削除する．
void
YmSat::del_watcher(
  Literal watch_lit,
  Reason reason
)
{
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

// @brief SAT 問題を解く．
SatBool3
YmSat::solve(
  const vector<SatLiteral>& assumptions,
  SatModel& model,
  vector<SatLiteral>& conflicts
)
{
  if ( debug & debug_solve ) {
    cout << "YmSat::solve starts" << endl;
    cout << " Assumptions: ";
    const char* and_str = "";
    for ( auto lit: assumptions ) {
      cout << and_str << lit;
      and_str = " & ";
    }
    cout << endl;
    cout << " Clauses:" << endl;
    for ( auto clause: mConstrClauseList ) {
      cout << "  " << *(clause) << endl;
    }
  }

  if ( mTimerOn ) {
    mStartTime = Clock::now();
  }

  alloc_var();

  // 自明な簡単化を行う．
  reduce_CNF();
  if ( !mSane ) {
    // その時点で充足不可能なら終わる．
    if ( debug & debug_solve ) {
      cout << "UNSAT in simplifyDB()" << endl;
    }
    return SatBool3::False;
  }

  // パラメータの初期化
  double confl_limit = 100;
  double learnt_limit = mConstrClauseList.size() / 3;
  mVarHeap.set_decay(mParams.mVarDecay);
  mClauseDecay = mParams.mClauseDecay;

  ASSERT_COND( decision_level() == 0 );

  // assumption の割り当てを行う．
  for ( auto l: assumptions ) {
    mAssignList.set_marker();
    auto lit = Literal{l};
    bool stat = check_and_assign(lit);

    if ( debug & (debug_assign | debug_decision) ) {
      cout << endl
	   << "assume " << lit << " @" << decision_level()
	   << endl;
      if ( !stat )  {
	cout << "\t--> conflict with previous assignment" << endl
	     << "\t    " << ~lit << " was assigned at level "
	     << decision_level(lit.varid()) << endl;
      }
    }

    // 条件式のなかに重要な手続きが書いてあるあんまり良くないコード
    // だけど implication() は stat == true の時しか実行しないのでしょうがない．
    if ( !stat || implication() != Reason::None ) {
      // 矛盾が起こった．
      backtrack(0);
      if ( debug & debug_solve ) {
	cout << "UNSAT" << endl;
      }

      return SatBool3::False;
    }
  }

  // 以降，現在のレベルが基底レベルとなる．
  mRootLevel = decision_level();
  if ( debug & (debug_assign | debug_decision) ) {
    cout << "RootLevel = " << mRootLevel << endl;
  }

  SatBool3 stat = SatBool3::X;
  for ( ; ; ) {
    // 実際の探索を行う．
    mConflictLimit = static_cast<int>(confl_limit);
    if ( mConflictLimit > mMaxConflict ) {
      mConflictLimit = mMaxConflict;
    }
    mLearntLimit = static_cast<int>(learnt_limit);
    auto stats = get_stats();
    for ( auto handler: mMsgHandlerList ) {
      handler->print_message(stats);
    }
    ++ mRestart;
    stat = search();
    if ( stat != SatBool3::X ) {
      // 結果が求められた．
      break;
    }
    if ( mConflictLimit == mMaxConflict ) {
      // 制限値に達した．(アボート)
      break;
    }

    // 判定できなかったのでパラメータを更新して次のラウンドへ
    confl_limit = confl_limit * 1.5;
    learnt_limit = learnt_limit * 1.1;
  }
  if ( stat == SatBool3::True ) {
    // SAT ならモデル(充足させる変数割り当てのリスト)を作る．
    model.resize(mVarNum);
    for ( int var = 0; var < mVarNum; ++ var ) {
      auto val = eval(var);
      ASSERT_COND( val == SatBool3::True || val == SatBool3::False );
      model.set(var, val);
    }
  }
  backtrack(0);

  if ( mTimerOn ) {
    auto t = Clock::now();
    auto d = std::chrono::duration_cast<Duration>(t - mStartTime);
    mAccTime += d;
  }

  if ( debug & debug_solve ) {
    switch ( stat ) {
    case SatBool3::True:  cout << "SAT" << endl; break;
    case SatBool3::False: cout << "UNSAT" << endl; break;
    case SatBool3::X:     cout << "UNKNOWN" << endl; break;
    default: ASSERT_NOT_REACHED;
    }
  }

  return stat;
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
YmSat::stop()
{
}

// @brief 学習節をすべて削除する．
void
YmSat::forget_learnt_clause()
{
}

// @brief 学習節の整理を行なう．
void
YmSat::reduce_learnt_clause()
{
  reduceDB();
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

// @brief 現在の内部状態を得る．
SatStats
YmSat::get_stats() const
{
  SatStats stats;
  stats.mRestart = mRestart;
  stats.mVarNum = mVarNum;
  stats.mConstrClauseNum = mConstrClauseList.size() + mConstrBinNum;
  stats.mConstrLitNum = mConstrLitNum;
  stats.mLearntClauseNum = mLearntClause.size() + mLearntBinNum;
  stats.mLearntLitNum = mLearntLitNum;
  stats.mConflictNum = mConflictNum;
  stats.mDecisionNum = mDecisionNum;
  stats.mPropagationNum = mPropagationNum;
  stats.mConflictLimit = mConflictLimit;
  stats.mLearntLimit = mLearntLimit;
  stats.mTime = mAccTime;
  return stats;
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

// 探索を行う本体の関数
SatBool3
YmSat::search()
{
  // コンフリクトの起こった回数
  SizeType n_confl = 0;
  for ( ; ; ) {
    // キューにつまれている割り当てから含意される値の割り当てを行う．
    auto conflict = implication();
    if ( conflict != Reason::None ) {
      // 矛盾が生じた．
      ++ n_confl;
      ++ mConflictNum;
      if ( decision_level() == mRootLevel ) {
	// トップレベルで矛盾が起きたら充足不可能
	return SatBool3::False;
      }
      if ( n_confl > mConflictLimit ) {
	// 矛盾の回数が制限値を越えた．
	backtrack(mRootLevel);
	return SatBool3::X;
      }

      // 今の矛盾の解消に必要な条件を「学習」する．
      int bt_level = mAnalyzer->analyze(conflict, mLearntLits);

      if ( debug & debug_analyze ) {
	cout << endl
	     << "analyze for " << conflict << endl
	     << "learnt clause is ";
	const char* plus = "";
	for ( int i = 0; i < mLearntLits.size(); ++ i ) {
	  auto l = mLearntLits[i];
	  cout << plus << l << " @" << decision_level(l.varid());
	  plus = " + ";
	}
	cout << endl;
      }

      // バックトラック
      if ( bt_level < mRootLevel ) {
	bt_level = mRootLevel;
      }
      backtrack(bt_level);

      // 学習節の生成
      add_learnt_clause();

      decay_var_activity();
      decay_clause_activity();
    }
    else {
      if ( decision_level() == 0 ) {
	reduce_CNF();
      }
      if ( mLearntClause.size() > mLearntLimit ) {
	reduceDB();
      }

      // 次の割り当てを選ぶ．
      auto lit = next_decision();
      if ( !lit.is_valid() ) {
	// すべての変数を割り当てた．
	// ということは充足しているはず．
	return SatBool3::True;
      }
      ++ mDecisionNum;

      // バックトラックポイントを記録
      mAssignList.set_marker();

      // 未割り当ての変数を選んでいるのでエラーになるはずはない．
      if ( debug & (debug_assign | debug_decision) ) {
	cout << endl
	     << "choose " << lit << " @" << decision_level() << endl;
      }
      assign(lit);
    }
  }
}

// 割当てキューに基づいて implication を行う．
Reason
YmSat::implication()
{
  auto conflict = Reason::None;
  while ( mAssignList.has_elem() ) {
    auto l{mAssignList.get_next()};
    ++ mPropagationNum;

    if ( debug & debug_implication ) {
      cout << "\tpick up " << l << endl;
    }
    // l の割り当てによって無効化された watcher-list の更新を行う．
    auto nl = ~l;

    auto& wlist = watcher_list(l);
    SizeType n = wlist.num();
    SizeType rpos = 0;
    SizeType wpos = 0;
    while ( rpos < n ) {
      auto w = wlist.elem(rpos);
      wlist.set_elem(wpos, w);
      ++ rpos;
      ++ wpos;
      if ( w.is_literal() ) {
	// 2-リテラル節の場合は相方のリテラルに基づく値の割り当てを行う．
	auto l0 = w.literal();
	auto val0 = eval(l0);
	if ( val0 == SatBool3::X ) {
	  if ( debug & debug_assign ) {
	    cout << "\tassign " << l0 << " @" << decision_level()
		 << " from " << l << endl;
	  }
	  assign(l0, Reason{nl});
	}
	else if ( val0 == SatBool3::False ) {
	  // 矛盾がおこった．
	  if ( debug & debug_assign ) {
	    cout << "\t--> conflict with previous assignment" << endl
		 << "\t    " << ~l0 << " was assigned at level "
		 << decision_level(l0.varid()) << endl;
	  }

	  // ループを抜けるためにキューの末尾まで先頭を動かす．
	  mAssignList.skip_all();

	  // 矛盾の理由を表す節を作る．
	  mTmpBinClause->set(l0, nl);
	  conflict = Reason{mTmpBinClause};
	  break;
	}
      }
      else { // w.is_clause()
	// 3つ以上のリテラルを持つ節の場合は，
	// - nl(~l) を wl1() にする．(場合によっては wl0 を入れ替える)
	// - wl0() が充足していたらなにもしない．
	// - wl0() が不定，もしくは偽なら，nl の代わりの watch literal を探す．
	// - 代わりが見つかったらそのリテラルを wl1() にする．
	// - なければ wl0() に基づいた割り当てを行う．場合によっては矛盾が起こる．
	auto c = w.clause();
	auto l0 = c->wl0();
	if ( l0 == nl ) {
	  // nl を 1番めのリテラルにする．
	  c->xchange_wl();
	  // 新しい wl0 を得る．
	  l0 = c->wl0();
	}
	else { // l1 == nl
	  ASSERT_COND( c->wl1() == nl );
	}

	auto val0 = eval(l0);
	if ( val0 == SatBool3::True ) {
	  // すでに充足していた．
	  continue;
	}

	if ( debug & debug_implication ) {
	  cout << "\t\texamining watcher clause " << c << endl;
	}

	// nl の替わりのリテラルを見つける．
	// この時，替わりのリテラルが未定かすでに充足しているかどうか
	// は問題でない．
	bool found = false;
	SizeType n = c->lit_num();
	for ( SizeType i = 2; i < n; ++ i ) {
	  auto l2 = c->lit(i);
	  if ( eval(l2) != SatBool3::False ) {
	    // l2 を 1番めの watch literal にする．
	    c->xchange_wl1(i);
	    if ( debug & debug_implication ) {
	      cout << "\t\t\tsecond watching literal becomes "
		   << l2 << endl;
	    }
	    // l の watcher list から取り除く
	    -- wpos;
	    // ~l2 の watcher list に追加する．
	    add_watcher(~l2, w);

	    found = true;
	    break;
	  }
	}
	if ( found ) {
	  continue;
	}

	if ( debug & debug_implication ) {
	  cout << "\t\tno other watching literals" << endl;
	}

	// 見付からなかったので l0 に従った割り当てを行う．
	if ( val0 == SatBool3::X ) {
	  if ( debug & debug_assign ) {
	    cout << "\tassign " << l0 << " @" << decision_level()
		 << " from " << w << endl;
	  }
	  assign(l0, w);
	}
	else {
	  // 矛盾がおこった．
	  if ( debug & debug_assign ) {
	    cout << "\t--> conflict with previous assignment" << endl
		 << "\t    " << ~l0 << " was assigned at level "
		 << decision_level(l0.varid()) << endl;
	  }

	  // ループを抜けるためにキューの末尾まで先頭を動かす．
	  mAssignList.skip_all();

	  // この場合は w が矛盾の理由を表す節になっている．
	  conflict = w;
	  break;
	}
      }
    }
    // 途中でループを抜けた場合に wlist の後始末をしておく．
    if ( wpos != rpos ) {
      for ( ; rpos < n; ++ rpos) {
	wlist.set_elem(wpos, wlist.elem(rpos));
	++ wpos;
      }
      wlist.erase(wpos);
    }
  }

  return conflict;
}

// level までバックトラックする
void
YmSat::backtrack(
  int level
)
{
  if ( debug & (debug_assign | debug_decision) ) {
    cout << endl
	 << "backtrack until @" << level << endl;
  }

  if ( level < decision_level() ) {
    mAssignList.backtrack(level);
    while ( mAssignList.has_elem() ) {
      auto p = mAssignList.get_prev();
      int varid = p.varid();
      mVal[varid] = conv_from_Bool3(SatBool3::X);
      mVarHeap.push(varid);
      if ( debug & debug_assign ) {
	cout << "\tdeassign " << p << endl;
      }
    }
  }

  if ( debug & (debug_assign | debug_decision) ) {
    cout << endl;
  }
}

// 次の割り当てを選ぶ
Literal
YmSat::next_decision()
{
#if 0
  // 一定確率でランダムな変数を選ぶ．
  if ( mRandGen.real1() < mParams.mVarFreq && !heap_empty() ) {
    int vid = mRandGen.int32() % mVarNum;
    bool inv = false;
    if ( eval(vid) == SatBool3::X ) {
      return Literal::conv_from_varid(vid, inv);
    }
  }
#endif
  while ( !mVarHeap.empty() ) {
    int vindex = mVarHeap.pop_top();
    if ( mVal[vindex] == conv_from_Bool3(SatBool3::X) ) {
      // Watcher の多い方の極性を(わざと)選ぶ
      int v2 = vindex * 2;
      auto dlit = Literal::conv_from_varid(vindex, false);
      if ( mWatcherList[v2 + 0].num() >= mWatcherList[v2 + 1].num() ) {
	return dlit;
      }
      else {
	return ~dlit;
      }
    }
  }
  return SatLiteral::X;
}

// CNF を簡単化する．
void
YmSat::reduce_CNF()
{
  if ( !mSane ) {
    return;
  }
  ASSERT_COND( decision_level() == 0 );

  if ( implication() != Reason::None ) {
    mSane = false;
    return;
  }

}

// 使われていない学習節を削除する．
void
YmSat::reduceDB()
{
  SizeType n = mLearntClause.size();
  SizeType n2 = n / 2;

  // 足切りのための制限値
  double abs_limit = mClauseBump / n;

  sort(mLearntClause.begin(), mLearntClause.end(), ClauseLess());

  auto wpos = mLearntClause.begin();
  for ( SizeType i = 0; i < n2; ++ i ) {
    auto clause = mLearntClause[i];
    if ( clause->lit_num() > 2 && !is_locked(clause) ) {
      delete_clause(clause);
    }
    else {
      *wpos = clause;
      ++ wpos;
    }
  }
  for ( SizeType i = n2; i < n; ++ i ) {
    auto clause = mLearntClause[i];
    if ( clause->lit_num() > 2 && !is_locked(clause) &&
	 clause->activity() < abs_limit ) {
      delete_clause(clause);
    }
    else {
      *wpos = clause;
      ++ wpos;
    }
  }
  if ( wpos != mLearntClause.end() ) {
    mLearntClause.erase(wpos, mLearntClause.end());
  }
}

// 新しい節を生成する．
Clause*
YmSat::new_clause(
  SizeType lit_num,
  bool learnt
)
{
  SizeType size = sizeof(Clause) + sizeof(Literal) * (lit_num - 1);
  auto p = new char[size];
  auto clause = new (p) Clause{lit_num, mTmpLits, learnt};

  return clause;
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

// 節を捨てる．
void
YmSat::delete_clause(
  Clause* clause
)
{
  ASSERT_COND( clause->is_learnt() );

  // watch list を更新
  del_watcher(~clause->wl0(), Reason{clause});
  del_watcher(~clause->wl1(), Reason{clause});

  mLearntLitNum -= clause->lit_num();

  auto p = reinterpret_cast<char*>(clause);
  delete [] p;
}

// 学習節のアクティビティを増加させる．
void
YmSat::bump_clause_activity(
  Clause* clause
)
{
  clause->increase_activity(mClauseBump);
  if ( clause->activity() > 1e+100 ) {
    for ( auto clause1: mLearntClause ) {
      clause1->factor_activity(1e-100);
    }
    mClauseBump *= 1e-100;
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
  for ( int i = 0; i < mOldVarNum; ++ i ) {
    mVal[i] = old_val[i];
    mDecisionLevel[i] = old_decision_level[i];
    mReason[i] = old_reason[i];
  }
  SizeType n2 = mOldVarNum * 2;
  for ( int i = 0; i < n2; ++ i ) {
    mWatcherList[i].move(old_watcher_list[i]);
  }
  if ( old_size > 0 ) {
    delete [] old_val;
    delete [] old_decision_level;
    delete [] old_reason;
    delete [] old_watcher_list;
  }
  mAssignList.reserve(mVarSize);
  mVarHeap.alloc_var(mVarSize);
  mAnalyzer->alloc_var(mVarSize);
}

END_NAMESPACE_YM_SAT1
