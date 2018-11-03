
/// @file YmSat.cc
/// @brief YmSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ymsat1/YmSat.h"
#include "ymsat1/SatAnalyzer.h"
#include "ymsat1/SatClause.h"
#include "ym/SatStats.h"
#include "ym/SatMsgHandler.h"


BEGIN_NAMESPACE_YM_SAT1

#if YMSAT_USE_LBD
const
YmSat::Params kDefaultParams(0.95, 0.999, false);
#else
const
YmSat::Params kDefaultParams(0.95, 0.999);
#endif


//////////////////////////////////////////////////////////////////////
// YmSat
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
YmSat::YmSat(const string& option) :
  mSane(true),
  mAlloc(4096),
  mConstrBinNum(0),
  mConstrLitNum(0),
  mLearntBinNum(0),
  mLearntLitNum(0),
  mVarNum(0),
  mOldVarNum(0),
  mVarSize(0),
  mVal(NULL),
  mDecisionLevel(NULL),
  mReason(NULL),
  mWatcherList(NULL),
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
  mTmpLits = new SatLiteral[mTmpLitsSize];

  mTmpBinClause = new_clause(2);

  mRestart = 0;

  mTimerOn = false;
}

// @brief デストラクタ
YmSat::~YmSat()
{
  // SatClause は mAlloc が開放してくれる．

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
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
SatVarId
YmSat::new_variable(bool decision)
{
  if ( decision_level() != 0 ) {
    // エラー
    cout << "Error!: decision_level() != 0" << endl;
    return kSatVarIdIllegal;
  }

#if YMSAT_USE_DVAR
  mDvarArray.push_back(decision);
#endif

  // ここではカウンタを増やすだけ
  // 実際の処理は alloc_var() でまとめて行う．
  int n = mVarNum;
  ++ mVarNum;
  return SatVarId(n);
}

// @brief 条件リテラルを設定する．
// @param[in] lit_list 条件リテラルのリスト
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
void
YmSat::set_conditional_literals(const vector<SatLiteral>& lit_list)
{
  mCondLits.clear();
  int lit_num = lit_list.size();
  mCondLits.resize(lit_num);
  for ( int i = 0; i < lit_num; ++ i ) {
    mCondLits[i] = lit_list[i];
  }
}

// @brief 条件リテラルを設定する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
void
YmSat::set_conditional_literals(int lit_num,
				const SatLiteral* lits)
{
  mCondLits.clear();
  mCondLits.resize(lit_num);
  for ( int i = 0; i < lit_num; ++ i ) {
    mCondLits[i] = lits[i];
  }
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
YmSat::add_clause(const vector<SatLiteral>& lits)
{
  int lit_num = lits.size();
  int n2 = mCondLits.size();
  alloc_lits(lit_num + n2);
  for ( int i = 0; i < lit_num; ++ i ) {
    mTmpLits[i] = lits[i];
  }
  for ( int i = 0; i < n2; ++ i ) {
    SatLiteral lit = mCondLits[i];
    mTmpLits[lit_num + i] = ~lit;
  }
  add_clause_sub(lit_num + n2);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
YmSat::add_clause(int lit_num,
		  const SatLiteral* lits)
{
  int n2 = mCondLits.size();
  alloc_lits(lit_num + n2);
  for ( int i = 0; i < lit_num; ++ i ) {
    mTmpLits[i] = lits[i];
  }
  for ( int i = 0; i < n2; ++ i ) {
    SatLiteral lit = mCondLits[i];
    mTmpLits[lit_num + i] = ~lit;
  }
  add_clause_sub(lit_num + n2);
}

// @brief add_clause() の下請け関数
void
YmSat::add_clause_sub(int lit_num)
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

  alloc_var();

  // - 重複したリテラルの除去
  // - false literal の除去
  // - true literal を持つかどうかのチェック
  int wpos = 0;
  for ( int rpos = 0; rpos < lit_num; ++ rpos ) {
    SatLiteral l = mTmpLits[rpos];
    if ( wpos != 0 && mTmpLits[wpos - 1] == l ) {
      // 重複している．
      continue;
    }
    SatBool3 v = eval(l);
    if ( v == SatBool3::False ) {
      // false literal は追加しない．
      continue;
    }
    if ( v == SatBool3::True ) {
      // true literal があったら既に充足している
      return;
    }
    if ( l.varid().val() >= mVarNum ) {
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

  SatLiteral l0 = mTmpLits[0];
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

  SatLiteral l1 = mTmpLits[1];

  if ( lit_num == 2 ) {
    // watcher-list の設定
    add_watcher(~l0, SatReason(l1));
    add_watcher(~l1, SatReason(l0));

    ++ mConstrBinNum;
  }
  else {
    // 節の生成
    SatClause* clause = new_clause(lit_num, mTmpLits);
    mConstrClauseList.push_back(clause);

    // watcher-list の設定
    add_watcher(~l0, SatReason(clause));
    add_watcher(~l1, SatReason(clause));
  }
}

// 学習節を追加する．
void
YmSat::add_learnt_clause()
{
  int n = mLearntLits.size();
  mLearntLitNum += n;

  if ( n == 0 ) {
    // empty clause があったら unsat
    mSane = false;
    return;
  }

  SatLiteral l0 = mLearntLits[0];
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

  SatReason reason;
  SatLiteral l1 = mLearntLits[1];
  if ( n == 2 ) {
    reason = SatReason(l1);

    // watcher-list の設定
    add_watcher(~l0, SatReason(l1));
    add_watcher(~l1, SatReason(l0));

    ++ mLearntBinNum;
  }
  else {
    // 節の生成
    alloc_lits(n);
    for ( int i = 0; i < n; ++ i ) {
      mTmpLits[i] = mLearntLits[i];
    }
    SatClause* clause = new_clause(n, true);
    mLearntClause.push_back(clause);

    reason = SatReason(clause);

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
// @param[in] watch_lit リテラル
// @param[in] reason 理由
void
YmSat::del_watcher(SatLiteral watch_lit,
		   SatReason reason)
{
  Watcher w0(reason);
  WatcherList& wlist = watcher_list(watch_lit);
  int n = wlist.num();
  int wpos = 0;
  for ( ; wpos < n; ++ wpos) {
    Watcher w = wlist.elem(wpos);
    if ( w == w0 ) {
      break;
    }
  }
  ASSERT_COND( wpos < n );
  -- n;
  for ( ; wpos < n; ++ wpos) {
    Watcher w = wlist.elem(wpos + 1);
    wlist.set_elem(wpos, w);
  }
  wlist.erase(n);
}

// @brief SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
// @retval SatBool3::True 充足した．
// @retval SatBool3::False 充足不能が判明した．
// @retval SatBool3::X わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
YmSat::solve(const vector<SatLiteral>& assumptions,
	     vector<SatBool3>& model,
	     vector<SatLiteral>& conflicts)
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
    mTimer.stop();
    mTimer.reset();
    mTimer.start();
  }

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
  for ( auto lit: assumptions ) {
    mAssignList.set_marker();
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
    if ( !stat || implication() != kNullSatReason ) {
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
    SatStats stats;
    get_stats(stats);
    for (list<SatMsgHandler*>::iterator p = mMsgHandlerList.begin();
	 p != mMsgHandlerList.end(); ++ p) {
      SatMsgHandler& handler = *(*p);
      handler.print_message(stats);
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
    for ( int i = 0; i < mVarNum; ++ i ) {
      SatBool3 val = eval(SatVarId(i));
      ASSERT_COND( val == SatBool3::True || val == SatBool3::False );
      model[i] = val;
    }
  }
  backtrack(0);

  if ( mTimerOn ) {
    mTimer.stop();
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
// @param[in] val 設定する値
// @return 以前の設定値を返す．
int
YmSat::set_max_conflict(int val)
{
  int old_val = mMaxConflict;
  mMaxConflict = val;
  return old_val;
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
YmSat::get_stats(SatStats& stats) const
{
  stats.mRestart = mRestart;
  stats.mVarNum = mVarNum;
  stats.mConstrClauseNum = clause_num();
  stats.mConstrLitNum = mConstrLitNum;
  stats.mLearntClauseNum = mLearntClause.size() + mLearntBinNum;
  stats.mLearntLitNum = mLearntLitNum;
  stats.mConflictNum = mConflictNum;
  stats.mDecisionNum = mDecisionNum;
  stats.mPropagationNum = mPropagationNum;
  stats.mConflictLimit = mConflictLimit;
  stats.mLearntLimit = mLearntLimit;
  stats.mTime = mTimer.time();
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
YmSat::reg_msg_handler(SatMsgHandler* msg_handler)
{
  mMsgHandlerList.push_back(msg_handler);
}

// @brief 時間計測機能を制御する
void
YmSat::timer_on(bool enable)
{
  mTimerOn = enable;
}

// 探索を行う本体の関数
SatBool3
YmSat::search()
{
  // コンフリクトの起こった回数
  int n_confl = 0;
  for ( ; ; ) {
    // キューにつまれている割り当てから含意される値の割り当てを行う．
    SatReason conflict = implication();
    if ( conflict != kNullSatReason ) {
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
	  SatLiteral l = mLearntLits[i];
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
      SatLiteral lit = next_decision();
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
SatReason
YmSat::implication()
{
  SatReason conflict = kNullSatReason;
  while ( mAssignList.has_elem() ) {
    SatLiteral l = mAssignList.get_next();
    ++ mPropagationNum;

    if ( debug & debug_implication ) {
      cout << "\tpick up " << l << endl;
    }
    // l の割り当てによって無効化された watcher-list の更新を行う．
    SatLiteral nl = ~l;

    WatcherList& wlist = watcher_list(l);
    int n = wlist.num();
    int rpos = 0;
    int wpos = 0;
    while ( rpos < n ) {
      Watcher w = wlist.elem(rpos);
      wlist.set_elem(wpos, w);
      ++ rpos;
      ++ wpos;
      if ( w.is_literal() ) {
	// 2-リテラル節の場合は相方のリテラルに基づく値の割り当てを行う．
	SatLiteral l0 = w.literal();
	SatBool3 val0 = eval(l0);
	if ( val0 == SatBool3::X ) {
	  if ( debug & debug_assign ) {
	    cout << "\tassign " << l0 << " @" << decision_level()
		 << " from " << l << endl;
	  }
	  assign(l0, SatReason(nl));
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
	  conflict = SatReason(mTmpBinClause);
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
	SatClause* c = w.clause();
	SatLiteral l0 = c->wl0();
	if ( l0 == nl ) {
	  // nl を 1番めのリテラルにする．
	  c->xchange_wl();
	  // 新しい wl0 を得る．
	  l0 = c->wl0();
	}
	else { // l1 == nl
	  ASSERT_COND( c->wl1() == nl );
	}

	SatBool3 val0 = eval(l0);
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
	int n = c->lit_num();
	for ( int i = 2; i < n; ++ i ) {
	  SatLiteral l2 = c->lit(i);
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
YmSat::backtrack(int level)
{
  if ( debug & (debug_assign | debug_decision) ) {
    cout << endl
	 << "backtrack until @" << level << endl;
  }

  if ( level < decision_level() ) {
    mAssignList.backtrack(level);
    while ( mAssignList.has_elem() ) {
      SatLiteral p = mAssignList.get_prev();
      SatVarId varid = p.varid();
      int vindex = varid.val();
      mVal[vindex] = conv_from_Bool3(SatBool3::X);
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
SatLiteral
YmSat::next_decision()
{
#if 0
  // 一定確率でランダムな変数を選ぶ．
  if ( mRandGen.real1() < mParams.mVarFreq && !heap_empty() ) {
    int pos = mRandGen.int32() % mVarNum;
    SatVarId vid(pos);
    tPol pol = kPolNega;
    if ( eval(SatVarId(vid)) == SatBool3::X ) {
      return SatLiteral(vid, pol);
    }
  }
#endif
  while ( !mVarHeap.empty() ) {
    int vindex = mVarHeap.pop_top();
    if ( mVal[vindex] == conv_from_Bool3(SatBool3::X) ) {
      // Watcher の多い方の極性を(わざと)選ぶ
      int v2 = vindex * 2;
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
  return kSatLiteralX;
}

// CNF を簡単化する．
void
YmSat::reduce_CNF()
{
  if ( !mSane ) {
    return;
  }
  ASSERT_COND( decision_level() == 0 );

  if ( implication() != kNullSatReason ) {
    mSane = false;
    return;
  }

}

BEGIN_NONAMESPACE
// reduceDB で用いる SatClause の比較関数
class SatClauseLess
{
public:
  bool
  operator()(SatClause* a,
	     SatClause* b)
  {
    return a->lit_num() > 2 && (b->lit_num() == 2 || a->activity() < b->activity() );
  }
};
END_NONAMESPACE

// 使われていない学習節を削除する．
void
YmSat::reduceDB()
{
  int n = mLearntClause.size();
  int n2 = n / 2;

  // 足切りのための制限値
  double abs_limit = mClauseBump / n;

  sort(mLearntClause.begin(), mLearntClause.end(), SatClauseLess());

  vector<SatClause*>::iterator wpos = mLearntClause.begin();
  for ( int i = 0; i < n2; ++ i ) {
    SatClause* clause = mLearntClause[i];
    if ( clause->lit_num() > 2 && !is_locked(clause) ) {
      delete_clause(clause);
    }
    else {
      *wpos = clause;
      ++ wpos;
    }
  }
  for ( int i = n2; i < n; ++ i ) {
    SatClause* clause = mLearntClause[i];
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
SatClause*
YmSat::new_clause(int lit_num,
		  bool learnt)
{
  int size = sizeof(SatClause) + sizeof(SatLiteral) * (lit_num - 1);
  void* p = mAlloc.get_memory(size);
  SatClause* clause = new (p) SatClause(lit_num, mTmpLits, learnt);

  return clause;
}

// @brief mTmpLits を確保する．
void
YmSat::alloc_lits(int lit_num)
{
  int old_size = mTmpLitsSize;
  while ( mTmpLitsSize <= lit_num ) {
    mTmpLitsSize <<= 1;
  }
  if ( old_size < mTmpLitsSize ) {
    delete [] mTmpLits;
    mTmpLits = new SatLiteral[mTmpLitsSize];
  }
}

// 節を捨てる．
void
YmSat::delete_clause(SatClause* clause)
{
  ASSERT_COND( clause->is_learnt() );

  // watch list を更新
  del_watcher(~clause->wl0(), SatReason(clause));
  del_watcher(~clause->wl1(), SatReason(clause));

  mLearntLitNum -= clause->lit_num();

  int size = sizeof(SatClause) + sizeof(SatLiteral) * (clause->lit_num() - 1);
  mAlloc.put_memory(size, static_cast<void*>(clause));
}

// 学習節のアクティビティを増加させる．
void
YmSat::bump_clause_activity(SatClause* clause)
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
  int old_size = mVarSize;
  ymuint8* old_val = mVal;
  int* old_decision_level = mDecisionLevel;
  SatReason* old_reason = mReason;
  WatcherList* old_watcher_list = mWatcherList;
  if ( mVarSize == 0 ) {
    mVarSize = 1024;
  }
  while ( mVarSize < mVarNum ) {
    mVarSize <<= 1;
  }
  mVal = new ymuint8[mVarSize];
  mDecisionLevel = new int[mVarSize];
  mReason = new SatReason[mVarSize];
  mWatcherList = new WatcherList[mVarSize * 2];
  for ( int i = 0; i < mOldVarNum; ++ i ) {
    mVal[i] = old_val[i];
    mDecisionLevel[i] = old_decision_level[i];
    mReason[i] = old_reason[i];
  }
  int n2 = mOldVarNum * 2;
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

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
YmSat::write_DIMACS(ostream& s) const
{
}

END_NAMESPACE_YM_SAT1
