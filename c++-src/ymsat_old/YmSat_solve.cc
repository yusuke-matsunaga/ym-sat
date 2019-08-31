
/// @file YmSat_solve.cc
/// @brief YmSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "YmSat.h"
#include "ym/SatStats.h"
#include "ym/SatModel.h"
#include "ym/SatMsgHandler.h"
#include "SatAnalyzer.h"
#include "SatClause.h"


BEGIN_NAMESPACE_YM_YMSATOLD

//////////////////////////////////////////////////////////////////////
// YmSat
//////////////////////////////////////////////////////////////////////

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
	     SatModel& model,
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
      cout << "  " << *clause << endl;
    }
    cout << " VarNum: " << mVarNum << endl;
  }

  // メッセージハンドラにヘッダの出力を行わせる．
  for ( auto handler: mMsgHandlerList ) {
    handler->print_header();
  }

  if ( mTimerOn ) {
    mTimer.stop();
    mTimer.reset();
    mTimer.start();
  }

  mGoOn = true;

  // 変数領域の確保を行う．
  alloc_var();

  // パラメータの初期化
  mRestart = 0;
  mConflictNum = 0;
  mDecisionNum = 0;
  mPropagationNum = 0;

  init_control_parameters();
  mVarHeap.set_decay(mParams.mVarDecay);
  mClauseDecay = mParams.mClauseDecay;

  // 最終的な結果を納める変数
  SatBool3 sat_stat = SatBool3::X;

  ASSERT_COND( decision_level() == 0 );

  // 自明な簡単化を行う．
  reduce_CNF();
  if ( !mSane ) {
    // その時点で充足不可能なら終わる．
    sat_stat = SatBool3::False;
    goto end;
  }

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

    if ( stat ) {
      // 今の割当に基づく含意を行う．
      SatReason reason = implication();
      if ( reason != kNullSatReason ) {
	// 矛盾が起こった．
	stat = false;
      }
    }

    if ( !stat ) {
      // 矛盾が起こった．
      backtrack(0);
      sat_stat = SatBool3::False;
      goto end;
    }
  }

  // 以降，現在のレベルが基底レベルとなる．
  mRootLevel = decision_level();
  if ( debug & (debug_assign | debug_decision) ) {
    cout << "RootLevel = " << mRootLevel << endl;
  }

  for ( ; ; ) {
    // 探索の本体
    sat_stat = search();

    // メッセージ出力を行う．
    {
      SatStats stats;
      get_stats(stats);
      for ( auto handler: mMsgHandlerList ) {
	handler->print_message(stats);
      }
    }

    if ( sat_stat != SatBool3::X ) {
      // 結果が求められた．
      break;
    }

    if ( !mGoOn || mConflictNum == mMaxConflict ) {
      // 制限値に達した．(アボート)
      break;
    }

    if ( debug & debug_assign ) {
      cout << "restart" << endl;
    }

    ++ mRestart;

    // 判定できなかったのでパラメータを更新して次のラウンドへ
    update_on_restart(mRestart);
  }

  if ( sat_stat == SatBool3::True ) {
    // SAT ならモデル(充足させる変数割り当てのリスト)を作る．
    model.resize(mVarNum);
    for ( int i = 0; i < mVarNum; ++ i ) {
      SatBool3 val = cur_val(mVal[i]);
      ASSERT_COND( val != SatBool3::X );
      model.set(i, val);
    }
  }
  // 最初の状態に戻す．
  backtrack(0);

  if ( mTimerOn ) {
    mTimer.stop();
  }

 end:

  // 終了メッセージを出力させる．
  {
    SatStats stats;
    get_stats(stats);
    for ( auto handler: mMsgHandlerList ) {
      handler->print_footer(stats);
    }
  }

  if ( debug & debug_solve ) {
    switch ( sat_stat ) {
    case SatBool3::True:  cout << "SAT" << endl; break;
    case SatBool3::False: cout << "UNSAT" << endl; break;
    case SatBool3::X:     cout << "UNKNOWN" << endl; break;
    default: ASSERT_NOT_REACHED;
    }
  }

  return sat_stat;
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
YmSat::stop()
{
  mGoOn = false;
}

// @brief 探索を行う本体の関数
// @retval SatBool3::True 充足した．
// @retval SatBool3::False 充足できないことがわかった．
// @retval SatBool3::X 矛盾の生起回数が mConflictLimit を超えた．
//
// 矛盾の結果新たな学習節が追加される場合もあるし，
// 内部で reduce_learnt_clause() を呼んでいるので学習節が
// 削減される場合もある．
SatBool3
YmSat::search()
{
  int cur_confl_num = 0;
  for ( ; ; ) {
    // キューにつまれている割り当てから含意される値の割り当てを行う．
    SatReason conflict = implication();
    if ( conflict != kNullSatReason ) {
      // 矛盾が生じた．
      ++ mConflictNum;
      ++ cur_confl_num;
      if ( decision_level() == mRootLevel ) {
	// トップレベルで矛盾が起きたら充足不可能
	return SatBool3::False;
      }

      // 今の矛盾の解消に必要な条件を「学習」する．
      vector<SatLiteral> learnt_lits;
      int bt_level = mAnalyzer->analyze(conflict, learnt_lits);

      if ( debug & debug_analyze ) {
	cout << endl
	     << "analyze for " << conflict << endl
	     << endl
	     << "learnt clause is ";
	const char* plus = "";
	for ( int i = 0; i < learnt_lits.size(); ++ i ) {
	  SatLiteral l = learnt_lits[i];
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
      add_learnt_clause(learnt_lits);

      decay_var_activity();
      decay_clause_activity();

      // パラメータの更新
      update_on_conflict();

      continue;
    }

    if ( cur_confl_num >= mConflictLimit ) {
      // 矛盾の回数が制限値を越えた．
      backtrack(mRootLevel);
      return SatBool3::X;
    }

    if ( decision_level() == 0 ) {
      // 一見，無意味に思えるが，学習節を追加した結果，真偽値が確定する節が
      // あるかもしれないのでそれを取り除く．
      reduce_CNF();
    }

    if ( mLearntClauseList.size() >=  mAssignList.size() + mLearntLimit ) {
      // 学習節の数が制限値を超えたら整理する．
      reduce_learnt_clause();
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

    if ( debug & (debug_assign | debug_decision) ) {
      cout << endl
	   << "choose " << lit << " :"
	   << mVarHeap.activity(lit.varid()) << endl;
    }

    if ( debug & debug_assign ) {
      cout << "\tassign " << lit << " @" << decision_level() << endl;
    }

    // 選ばれたリテラルに基づいた割当を行う．
    // 未割り当ての変数を選んでいるのでエラーになるはずはない．
    assign(lit);
  }
}

// 割当てキューに基づいて implication を行う．
SatReason
YmSat::implication()
{
  int prop_num = 0;
  SatReason conflict = kNullSatReason;
  while ( mAssignList.has_elem() ) {
    SatLiteral l = mAssignList.get_next();
    ++ prop_num;

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
	if ( val0 == SatBool3::True ) {
	  // すでに充足していた．
	  continue;
	}
	if ( debug & debug_assign ) {
	  cout << "\tassign " << l0 << " @" << decision_level()
	       << " from (" << l0
	       << " + " << ~l << "): " << l << endl;
	}
	if ( val0 == SatBool3::X ) {
	  assign(l0, SatReason(nl));
	}
	else { // val0 == SatBool3::False
	  // 矛盾がおこった．
	  if ( debug & debug_assign ) {
	    cout << "\t--> conflict(#" << mConflictNum << ") with previous assignment" << endl
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
	  if ( eval(c->wl1()) == SatBool3::True ) {
	    continue;
	  }
	  // nl を 1番めのリテラルにする．
	  c->xchange_wl();
	  // 新しい wl0 を得る．
	  l0 = c->wl0();
	}
	else { // l1 == nl
	  if ( debug & debug_implication ) {
	    // この assert は重いのでデバッグ時にしかオンにしない．
	    // ※ debug と debug_implication が const なので結果が0の
	    // ときにはコンパイル時に消されることに注意
	    ASSERT_COND(c->wl1() == nl );
	  }
	}

	SatBool3 val0 = eval(l0);
	if ( val0 == SatBool3::True ) {
	  // すでに充足していた．
	  continue;
	}

	if ( debug & debug_implication ) {
	  cout << "\t\texamining watcher clause " << (*c) << endl;
	}

	// nl の替わりのリテラルを見つける．
	// この時，替わりのリテラルが未定かすでに充足しているかどうか
	// は問題でない．
	bool found = false;
	int n = c->lit_num();
	for ( int i = 2; i < n; ++ i ) {
	  SatLiteral l2 = c->lit(i);
	  SatBool3 v = eval(l2);
	  if ( v != SatBool3::False ) {
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
	if ( debug & debug_assign ) {
	  cout << "\tassign " << l0 << " @" << decision_level()
	       << " from " << w << ": " << l << endl;
	}
	if ( val0 == SatBool3::X ) {
	  assign(l0, w);

#if YMSAT_USE_LBD
	  if ( mParams.mUseLbd ) {
	    int lbd = calc_lbd(c) + 1;
	    if ( c->lbd() > lbd ) {
	      c->set_lbd(lbd);
	    }
	  }
#endif
	}
	else {
	  // 矛盾がおこった．
	  if ( debug & debug_assign ) {
	    cout << "\t--> conflict(#" << mConflictNum << ") with previous assignment" << endl
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

  mPropagationNum += prop_num;
  mSweep_props -= prop_num;

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
      if ( debug & debug_assign ) {
	cout << "\tdeassign " << p << endl;
      }
      SatVarId varid = p.varid();
      int vindex = varid.val();
      mVal[vindex] = (mVal[vindex] << 2) | conv_from_Bool3(SatBool3::X);
      mVarHeap.push(varid);
    }
  }

  if ( debug & (debug_assign | debug_decision) ) {
    cout << endl;
  }
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

  if ( mAssignList.size() == mSweep_assigns || mSweep_props > 0 ) {
    // 前回から変化がなかったらスキップする．
    return;
  }

  // 制約節をスキャンする
  sweep_clause(mConstrClauseList);

  // 学習節をスキャンする．
  sweep_clause(mLearntClauseList);

  // 変数ヒープを再構成する．
  vector<SatVarId> var_list;
  var_list.reserve(mVarNum);
  for ( int i = 0; i < mVarNum; ++ i ) {
    SatVarId var(i);
    if ( eval(var) == SatBool3::X ) {
      var_list.push_back(SatVarId(i));
    }
    else {
      del_satisfied_watcher(SatLiteral(var, false));
      del_satisfied_watcher(SatLiteral(var, true));
    }
  }
  mVarHeap.build(var_list);

  // 現在の状況を記録しておく．
  mSweep_assigns = mAssignList.size();
  mSweep_props = mConstrLitNum + mLearntLitNum;
}

// @brief 充足している節を取り除く
// @param[in] clause_list 節のリスト
void
YmSat::sweep_clause(vector<SatClause*>& clause_list)
{
  int n = clause_list.size();
  int wpos = 0;
  for ( int rpos = 0; rpos < n; ++ rpos ) {
    SatClause* c = clause_list[rpos];
    int nl = c->lit_num();
    bool satisfied = false;
    for ( int i = 0; i < nl; ++ i ) {
      if ( eval(c->lit(i)) == SatBool3::True ) {
	satisfied = true;
	break;
      }
    }
    if ( satisfied ) {
      // c を削除する．
      delete_clause(c);
    }
    else {
      if ( wpos != rpos ) {
	clause_list[wpos] = c;
      }
      ++ wpos;
    }
  }
  if ( wpos != n ) {
    clause_list.erase(clause_list.begin() + wpos, clause_list.end());
  }
}

// @brief 学習節をすべて削除する．
void
YmSat::forget_learnt_clause()
{
  // こんな乱暴なことができるは初期状態だけ．
  ASSERT_COND( decision_level() == 0 );

  // 学習節を本当に削除する．
  for ( auto clause: mLearntClauseList ) {
    delete_clause(clause);
  }
  mLearntClauseList.clear();

  // 変数ヒープも再構成する．
  // 同時に変数の履歴もリセットする．
  mVarHeap.reset_activity();
  vector<SatVarId> var_list;
  var_list.reserve(mVarSize);
  for ( int i = 0; i < mVarSize; ++ i ) {
    var_list.push_back(SatVarId(i));
    mVal[i] = conv_from_Bool3(SatBool3::X) | (conv_from_Bool3(SatBool3::X) << 2);
  }
  mVarHeap.build(var_list);
}

#if YMSAT_USE_LBD
// @brief LBD を計算する．
// @param[in] clause 対象の節
int
YmSat::calc_lbd(const SatClause* clause)
{
  // 割当レベルの最大値 + 1 だけ mLbdTmp を確保する．
  int max_level = decision_level() + 1;
  int old_size = mLbdTmpSize;
  while ( mLbdTmpSize < max_level ) {
    mLbdTmpSize <<= 1;
  }
  if ( mLbdTmpSize != old_size ) {
    delete [] mLbdTmp;
    mLbdTmp = new bool[mLbdTmpSize];
  }

  int n = clause->lit_num();

  // mLbdTmp をクリア
  // ただし， clause に現れるリテラルのレベルだけでよい．
  for ( int i = 0; i < n; ++ i ) {
    SatLiteral l = clause->lit(i);
    SatVarId v = l.varid();
    int level = decision_level(v);
    mLbdTmp[level] = false;
  }

  // 異なる決定レベルの個数を数える．
  int c = 0;
  for ( int i = 0; i < n; ++ i ) {
    SatLiteral l = clause->lit(i);
    SatVarId v = l.varid();
    int level = decision_level(v);
    if ( !mLbdTmp[level] ) {
      // はじめてこのレベルの変数が現れた．
      mLbdTmp[level] = true;
      ++ c;
    }
  }

  return c;
}
#endif

// 学習節のアクティビティを増加させる．
// @param[in] clause 対象の節
void
YmSat::bump_clause_activity(SatClause* clause)
{
  clause->increase_activity(mClauseBump);
  if ( clause->activity() > 1e+100 ) {
    for ( auto clause1: mLearntClauseList ) {
      clause1->factor_activity(1e-100);
    }
    mClauseBump *= 1e-100;
  }
}

// 学習節のアクティビティを定率で減少させる．
void
YmSat::decay_clause_activity()
{
  mClauseBump /= mClauseDecay;
}

END_NAMESPACE_YM_YMSATOLD
