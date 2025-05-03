
/// @file SatCore.cc
/// @brief SatCore の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatCore.h"
#include "Controller.h"
#include "Analyzer.h"
#include "Selecter.h"
#include "Clause.h"
#include "ym/SatStats.h"
#include "ym/SatModel.h"
#include "ym/SatMsgHandler.h"
#include "ym/Range.h"

#define DOUT cout


BEGIN_NAMESPACE_YM_SAT

const Literal Literal::X;

//////////////////////////////////////////////////////////////////////
// SatCore
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatCore::SatCore(
  const JsonValue& js_obj
) : mController{Controller::new_obj(*this, js_obj)},
    mAnalyzer{Analyzer::new_obj(*this, js_obj)},
    mSelecter{Selecter::new_obj(*this, js_obj)}
{
  mConflictBudget = 0;
  mPropagationBudget = 0;

  mSweep_assigns = -1;
  mSweep_props = 0;

  // place-holder なので中身はダミー
  mTmpBinClause = Clause::new_clause({Literal::X, Literal::X});
}

// @brief デストラクタ
SatCore::~SatCore()
{
  for ( auto c: mConstrClauseList ) {
    Clause::delete_clause(c);
  }
  for ( auto c: mLearntClauseList ) {
    Clause::delete_clause(c);
  }
  Clause::delete_clause(mTmpBinClause);
}

// @brief 変数を追加する．
SatLiteral
SatCore::new_variable(
  bool decision
)
{
  if ( decision_level() != 0 ) {
    // エラー
    throw std::runtime_error{"decision_level() != 0"};
  }

  mDvarArray.push_back(decision);
  if ( decision ) {
    ++ mDvarNum;
  }

  // ここではカウンタを増やすだけ
  // 実際の処理は alloc_var() でまとめて行う．
  SatVarId n = mVarNum;
  ++ mVarNum;
  return get_lit(n, false);
}

// 実際に変数に関するデータ構造を生成する．
void
SatCore::alloc_var()
{
  if ( mOldVarNum < mVarNum ) {
    if ( mVarSize < mVarNum ) {
      expand_var();
    }
    for ( SizeType var: Range(mOldVarNum, mVarNum) ) {
      mVal[var] = conv_from_Bool3(SatBool3::X) | (conv_from_Bool3(SatBool3::X) << 2);
      if ( is_decision_variable(var) ) {
	mVarHeap.add_var(var);
      }
    }
    mOldVarNum = mVarNum;
  }
}

// 変数に関する配列を拡張する．
void
SatCore::expand_var()
{
  // 新しいサイズを計算する．
  if ( mVarSize == 0 ) {
    mVarSize = 1024;
  }
  while ( mVarSize < mVarNum ) {
    mVarSize <<= 1;
  }

  // 新しい配列を確保する．
  mVal.resize(mVarSize);
  mDecisionLevel.resize(mVarSize);
  mReason.resize(mVarSize, Reason::None);
  mWatcherList.resize(mVarSize * 2);
  mVarHeap.alloc_var(mVarSize);
  mAssignList.reserve(mVarSize);
}

// @brief 節を追加する．
void
SatCore::add_clause(
  const vector<SatLiteral>& lits
)
{
  if ( decision_level() != 0 ) {
    // エラー
    throw std::runtime_error{"decision_level() != 0"};
  }

  if ( !sane() ) {
    throw std::runtime_error{"mSane == false"};
  }

  // lits の内容を変更するので作業用のコピーを作る．
  vector<Literal> tmp_lits;
  tmp_lits.reserve(lits.size());
  for ( auto lit: lits ) {
    auto l = Literal{lit};
    tmp_lits.push_back(l);
  }

  // 変数用のデータ構造の確保
  alloc_var();

  // tmp_lits をソートする．
  sort(tmp_lits.begin(), tmp_lits.end(),
       [](Literal a, Literal b) {
	 return a.index() < b.index();
       });

  // - 重複したリテラルの除去
  // - false literal の除去
  // - true literal を持つかどうかのチェック
  auto rpos = tmp_lits.begin();
  auto epos = tmp_lits.end();
  auto wpos = rpos;
  Literal prev = Literal::X;
  for ( ; rpos != epos; ++ rpos ) {
    auto l = *rpos;
    if ( l == prev ) {
      // 重複している．
      continue;
    }
    if ( l.varid() == prev.varid() ) {
      // 同じ変数の相反するリテラル
      // この節は常に充足する．
      // ので無視して追加しない．
      return;
    }

    auto v = eval(l);
    if ( v == SatBool3::False ) {
      // false literal は追加しない．
      continue;
    }
    if ( v == SatBool3::True ) {
      // true literal があったら既に充足している
      // ので無視して追加しない．
      return;
    }
    if ( l.varid() < 0 || l.varid() >= mVarNum ) {
      // 範囲外
      // 普通はありえないが異常探知のためにチェックしておく．
      ostringstream buf;
      buf << "literal(" << l << "): out of range";
      throw std::runtime_error{buf.str()};
    }
    // 追加する．
    if ( wpos != rpos ) {
      *wpos = l;
    }
    ++ wpos;
  }
  if ( wpos != epos ) {
    tmp_lits.erase(wpos, epos);
  }
  auto lit_num = tmp_lits.size();

  if ( lit_num == 0 ) {
    // empty clause があったら unsat
    mSane = false;
    return;
  }

  // リテラル数の更新
  // 単項節のリテラル数も含めることにする．
  mConstrLitNum += lit_num;

  auto l0 = tmp_lits[0];
  if ( lit_num == 1 ) {
    // unit clause があったら値の割り当てを行う．
    bool stat = check_and_assign(l0);

    if ( debug & debug_assign ) {
      DOUT << "add_clause: (" << l0 << ")" << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << endl;
      if ( !stat )  {
	DOUT << "\t--> conflict(#" << mConflictNum
	     << ") with previous assignment" << endl
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
      mSane = false;
    }

    mConstrUnitList.push_back(l0);

    return;
  }

  // 項数の更新
  ++ mConstrClauseNum;

  auto l1 = tmp_lits[1];

  if ( lit_num == 2 ) {
    if ( debug & debug_assign ) {
      DOUT << "add_clause: (" << l0 << " + " << l1 << ")" << endl;
    }

    mConstrBinList.push_back(BinClause{l0, l1});

    // watcher-list の設定
    add_watcher(~l0, Watcher(l1));
    add_watcher(~l1, Watcher(l0));
  }
  else {
    // 節の生成
    auto clause = Clause::new_clause(tmp_lits);

    if ( debug & debug_assign ) {
      DOUT << "add_clause: " << (*clause) << endl;
    }

    mConstrClauseList.push_back(clause);

    // watcher-list の設定
    add_watcher(~l0, Watcher(clause));
    add_watcher(~l1, Watcher(clause));
  }
}

// @brief 学習節を追加する．
// @param[in] lits 追加するリテラルのリスト
void
SatCore::add_learnt_clause(
  const vector<Literal>& lits
)
{
  SizeType n = lits.size();
  mLearntLitNum += n;

  if ( n == 0 ) {
    // empty clause があったら unsat
    throw std::runtime_error{"add_learnt_clause() with empty clause"};
  }

  auto l0 = lits[0];
  if ( n == 1 ) {
    // unit clause があったら値の割り当てを行う．
    bool stat = check_and_assign(l0);
    if ( debug & debug_assign ) {
      DOUT << "\tassign " << l0 << " @" << decision_level()
	   << endl;
      if ( !stat )  {
	DOUT << "\t--> conflict(#" << mConflictNum
	     << " with previous assignment" << endl
	     << "\t    " << ~l0 << " was assigned at level "
	     << decision_level(l0.varid()) << endl;
      }
    }
    if ( !stat ) {
      throw std::runtime_error{"add_learnt_clause() with conflicting clause"};
    }
    return;
  }

  Reason reason;
  auto l1 = lits[1];
  if ( n == 2 ) {
    if ( debug & debug_assign ) {
      DOUT << "add_learnt_clause: "
	   << "(" << l0 << " + " << l1 << ")" << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << " from (" << l0 << " + " << l1 << ")" << endl;
    }

    // watcher-list の設定
    add_watcher(~l0, Watcher(l1));
    add_watcher(~l1, Watcher(l0));

    reason = Reason(l1);

    ++ mLearntBinNum;
  }
  else {
    // 節の生成
    auto clause = Clause::new_clause(lits, true);

    if ( debug & debug_assign ) {
      DOUT << "add_learnt_clause: " << *clause << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << " from " << *clause << endl;
    }

    bump_clause_activity(clause);

    mLearntClauseList.push_back(clause);

    // watcher-list の設定
    add_watcher(~l0, Watcher(clause));
    add_watcher(~l1, Watcher(clause));

    reason = Reason(clause);
  }

  // learnt clause の場合には必ず unit clause になっているはず．
  ASSERT_COND( eval(l0) != SatBool3::False );

  assign(l0, reason);
}

// CNF を簡単化する．
void
SatCore::reduce_CNF()
{
  if ( !sane() ) {
    return;
  }
  if ( decision_level() > 0 ) {
    throw std::runtime_error{"reduce_CNF(): decision_level() should be 0"};
  }

  if ( implication() != Reason::None ) {
    throw std::runtime_error{"something wrong"};
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
  for ( SatVarId var: Range(mVarNum) ) {
    if ( eval(var) == SatBool3::X ) {
      if ( is_decision_variable(var) ) {
	var_list.push_back(var);
      }
    }
    else {
      del_satisfied_watcher(Literal::conv_from_varid(var, false));
      del_satisfied_watcher(Literal::conv_from_varid(var, true));
    }
  }
  build(var_list);

  // 現在の状況を記録しておく．
  mSweep_assigns = mAssignList.size();
  mSweep_props = mConstrLitNum + mLearntLitNum;
}

// @brief 充足している節を取り除く
void
SatCore::sweep_clause(
  vector<Clause*>& clause_list
)
{
  SizeType n = clause_list.size();
  SizeType wpos = 0;
  for ( SizeType rpos: Range(n) ) {
    auto c = clause_list[rpos];
    SizeType nl = c->lit_num();
    bool satisfied = false;
    for ( SizeType i: Range(nl) ) {
      if ( eval(c->lit(i)) == SatBool3::True ) {
	satisfied = true;
	break;
      }
    }
    if ( satisfied && !is_locked(c) ) {
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

// 使われていない学習節を削除する．
void
SatCore::reduce_learnt_clause()
{
  SizeType n = mLearntClauseList.size();
  SizeType n2 = n / 2;

  // 足切りのための制限値
  double abs_limit = mClauseBump / n;

  sort(mLearntClauseList.begin(), mLearntClauseList.end(), ClauseLess());

  auto wpos = mLearntClauseList.begin();
  for ( SizeType i: Range(n2) ) {
    auto clause = mLearntClauseList[i];
    if ( clause->lit_num() > 2 && !is_locked(clause) ) {
      delete_clause(clause);
    }
    else {
      *wpos = clause;
      ++ wpos;
    }
  }
  for ( SizeType i: Range(n2, n) ) {
    auto clause = mLearntClauseList[i];
    if ( clause->lit_num() > 2 && !is_locked(clause) &&
	 clause->activity() < abs_limit ) {
      delete_clause(clause);
    }
    else {
      *wpos = clause;
      ++ wpos;
    }
  }
  if ( wpos != mLearntClauseList.end() ) {
    mLearntClauseList.erase(wpos, mLearntClauseList.end());
  }
}

// @brief 節を削除する．
void
SatCore::delete_clause(
  Clause* clause
)
{
  if ( debug & debug_assign ) {
    DOUT << " delete_clause: " << (*clause) << endl;
  }

  // watch list を更新
  del_watcher(~clause->wl0(), Watcher(clause));
  del_watcher(~clause->wl1(), Watcher(clause));

  if ( clause->is_learnt() ) {
    mLearntLitNum -= clause->lit_num();
  }

  Clause::delete_clause(clause);
}

// @brief SAT 問題を解く．
SatBool3
SatCore::solve(
  const vector<SatLiteral>& assumptions,
  SatModel& model,
  vector<SatLiteral>& conflicts
)
{
  alloc_var();

  mAssignList.reserve(mVarNum + assumptions.size());

  if ( debug & debug_solve ) {
    DOUT << "YmSat::solve starts" << endl;
    DOUT << " Assumptions: ";
    const char* and_str = "";
    for ( auto lit: assumptions ) {
      DOUT << and_str << lit;
      and_str = " & ";
    }
    DOUT << endl;
    DOUT << " Clauses:" << endl;
    for ( auto clause_p: mConstrClauseList ) {
      DOUT << "  " << *clause_p << endl;
    }
    DOUT << " VarNum: " << variable_num() << endl
	 << " DVarNum: " << mDvarNum << endl;
  }

  // メッセージハンドラにヘッダの出力を行わせる．
  print_header();

  // タイマーを開始する．
  start_timer();

  // 変数領域の確保を行う．
  mAnalyzer->alloc_var(variable_num());

  // パラメータの初期化
  mController->_init();

  mGoOn = true;
  mConflicts.clear();

  // 最終的な結果を納める変数
  auto sat_stat = SatBool3::X;

  ASSERT_COND( decision_level() == 0 );

  // 自明な簡単化を行う．
  reduce_CNF();
  if ( !sane() ) {
    // その時点で充足不可能なら終わる．
    sat_stat = SatBool3::False;
    goto end;
  }

  // assumption を Literal に変換する．
  mAssumptions.clear();
  mAssumptions.reserve(assumptions.size());
  for ( auto l: assumptions ) {
    auto lit = Literal{l};
    mAssumptions.push_back(lit);
  }

  // 探索の本体
  for ( ; ; ) {
    // 探索の本体
    sat_stat = search();

    // メッセージ出力を行う．
    print_stats();

    if ( debug & debug_assign ) {
      DOUT << "restart" << endl;
    }

    if ( !check_budget() ) {
      // 制限値に達した．(アボート)
      break;
    }

    if ( sat_stat != SatBool3::X ) {
      // 結果が求められた．
      break;
    }

    mController->_update_on_restart(restart_num());
  }

  if ( sat_stat == SatBool3::True ) {
    // SAT ならモデル(充足させる変数割り当てのリスト)を作る．
    model.resize(mVarNum);
    for ( SatVarId var: Range(mVarNum) ) {
      if ( is_decision_variable(var) ) {
	auto val = eval(var);
	ASSERT_COND( val == SatBool3::True || val == SatBool3::False );
	model.set(var, val);
      }
    }
  }
  else if ( sat_stat == SatBool3::False ) {
    // UNSAt なら矛盾の原因を作る．
    conflicts.clear();
    SizeType n = mConflicts.size();
    if ( n == 0 ) {
      // 矛盾の原因がないということは
      // 元々の式に矛盾があったということ．
      mSane = false;
    }
    else {
      conflicts.reserve(n);
      for ( auto l: mConflicts ) {
	SatVarId vid = l.varid();
	bool inv = l.is_negative();
	conflicts.push_back(get_lit(vid, inv));
      }
    }
  }

  // 最初の状態に戻す．
  backtrack(0);

  stop_timer();

 end:

  // 終了メッセージを出力させる．
  print_footer();

  if ( debug & debug_solve ) {
    switch ( sat_stat ) {
    case SatBool3::True:  DOUT << "SAT" << endl; break;
    case SatBool3::False: DOUT << "UNSAT" << endl; break;
    case SatBool3::X:     DOUT << "UNKNOWN" << endl; break;
    default: ASSERT_NOT_REACHED;
    }
  }

  return sat_stat;
}

// @brief 停止する．
void
SatCore::stop()
{
  mGoOn = false;
}

// @brief 時間計測機能を制御する
void
SatCore::timer_on(
  bool enable
)
{
  mTimerOn = enable;
}

// @brief 探索を行う本体の関数
SatBool3
SatCore::search()
{
  ++ mRestartNum;

  // 今回の矛盾の回数
  SizeType cur_confl_num = 0;
  for ( ; ; ) {
    // キューにつまれている割り当てから含意される値の割り当てを行う．
    auto conflict = implication();
    if ( conflict != Reason::None ) {
      // 矛盾が生じた．
      ++ mConflictNum;
      ++ cur_confl_num;
      if ( decision_level() == 0 ) {
	// トップレベルで矛盾が起きたら充足不可能
	return SatBool3::False;
      }

      // 今の矛盾の解消に必要な条件を「学習」する．
      vector<Literal> learnt_lits;
      int bt_level = mAnalyzer->analyze(conflict, learnt_lits);

      if ( debug & debug_analyze ) {
	DOUT << endl
	     << "analyze for " << conflict << endl
	     << endl
	     << "learnt clause is ";
	const char* plus = "";
	for ( auto l: learnt_lits ) {
	  DOUT << plus << l << " @" << decision_level(l.varid());
	  plus = " + ";
	}
	DOUT << endl;
      }

      // バックトラック
      backtrack(bt_level);

      // 学習節の生成
      add_learnt_clause(learnt_lits);

      decay_var_activity();
      decay_clause_activity();

      // パラメータの更新
      mController->_update_on_conflict();

      continue;
    }

    if ( cur_confl_num >= conflict_limit() || !check_budget() ) {
      // 矛盾の回数が制限値を越えた．
      backtrack(0);
      return SatBool3::X;
    }

    if ( decision_level() == 0 ) {
      // 一見，無意味に思えるが，学習節を追加した結果，真偽値が確定する節が
      // あるかもしれないのでそれを取り除く．
      reduce_CNF();
    }

    if ( learnt_clause_num() >=  last_assign() + mLearntLimit ) {
      // 学習節の数が制限値を超えたら整理する．
      reduce_learnt_clause();
    }

    // 次の割り当てを選ぶ．
    auto next_lit = Literal::X;
    while ( decision_level() < mAssumptions.size() ) {
      // assumption の割当を行う．
      auto p = mAssumptions[decision_level()];
      auto val = eval(p);
      if ( val == SatBool3::True ) {
	// すでに値が決まっていた．
	set_marker();
      }
      else if ( val == SatBool3::False ) {
	// 矛盾が生じた
	analyze_final(~p);
	return SatBool3::False;
      }
      else { // val == SatBool3::X
	next_lit = p;
	break;
      }
    }
    if ( next_lit == Literal::X ) {
      next_lit = mSelecter->next_decision();
      if ( !next_lit.is_valid() ) {
	// すべての変数を割り当てた．
	// ということは充足しているはず．
	return SatBool3::True;
      }

      ++ mDecisionNum;

      if ( debug & (debug_assign | debug_decision) ) {
	DOUT << endl
	     << "choose " << next_lit << " :"
	     << mVarHeap.activity(next_lit.varid())
	     << endl;
      }
    }

    // バックトラックポイントを記録
    set_marker();

    if ( debug & debug_assign ) {
      DOUT << "\tassign " << next_lit << " @" << decision_level() << endl;
    }

    // 選ばれたリテラルに基づいた割当を行う．
    // 未割り当ての変数を選んでいるのでエラーになるはずはない．
    assign(next_lit);
  }
  // ここに来るのは時間切れの時だけ
  return SatBool3::X;
}

// @brief 矛盾の原因を求める．
void
SatCore::analyze_final(
  Literal p
)
{
  mConflicts.push_back(p);

  if ( decision_level() == 0 ) {
    return;
  }

  unordered_set<SatVarId> mark;
  mark.emplace(p.varid());

  // 値割り当てを末尾から取り出す．
  for ( SizeType pos = mAssignList.size(); pos > 0; ) {
    -- pos;
    auto l = mAssignList.get(pos);
    auto v = l.varid();
    if ( mark.count(v) > 0 ) {
      auto r = reason(v);
      if ( r == Reason::None ) {
	mConflicts.push_back(~l);
      }
      else if ( r.is_literal() ) {
	auto l1 = r.literal();
	auto v1 = l1.varid();
	if ( decision_level(v1) > 0 ) {
	  mark.emplace(v1);
	}
      }
      else {
	auto c = r.clause();
	SizeType n = c->lit_num();
	for ( SizeType i = 0; i < n; ++ i ) {
	  auto v1 = c->lit(i).varid();
	  if ( decision_level(v1) > 0 ) {
	    mark.emplace(v1);
	  }
	}
      }
      mark.erase(v);
    }
  }
}

// @brief 割当てキューに基づいて implication を行う．
Reason
SatCore::implication()
{
  SizeType prop_num = 0;
  auto conflict = Reason::None;
  while ( mAssignList.has_elem() && conflict == Reason::None ) {
    auto l = mAssignList.get_next();
    ++ prop_num;

    if ( debug & debug_implication ) {
      DOUT << "\tpick up " << l << endl;
    }
    // l の割り当てによって無効化された watcher-list の更新を行う．
    auto nl = ~l;

    auto& wlist = watcher_list(l);
    SizeType rpos = 0;
    SizeType wpos = 0;
    SizeType wnum = wlist.size();
    while ( rpos < wnum ) {
      auto& w = wlist.elem(rpos);
      wlist.set_elem(wpos, w);
      ++ rpos;
      ++ wpos;
      if ( w.is_literal() ) {
	// 2-リテラル節の場合は相方のリテラルに基づく値の割り当てを行う．
	auto l0 = w.literal();
	auto val0 = eval(l0);
	if ( val0 == SatBool3::True ) {
	  // すでに充足していた．
	  continue;
	}
	if ( debug & debug_assign ) {
	  DOUT << "\tassign " << l0 << " @" << decision_level()
	       << " from (" << l0
	       << " + " << ~l << "): " << l << endl;
	}
	if ( val0 == SatBool3::X ) {
	  assign(l0, Reason{nl});
	}
	else { // val0 == SatBool3::False ) {
	  // 矛盾がおこった．
	  if ( debug & debug_assign ) {
	    DOUT << "\t--> conflict(#" << mConflictNum
		 << ") with previous assignment" << endl
		 << "\t    " << ~l0 << " was assigned at level "
		 << decision_level(l0.varid()) << endl;
	  }

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
	  if ( eval(c->wl1()) == SatBool3::True ) {
	    continue;
	  }
	  // nl を 1番めのリテラルにする．
	  c->xchange_wl();
	  // 新しい wl0 を得る．
	  l0 = c->wl0();
	}
	// else { l1 == nl ならなにもしない．

	auto val0 = eval(l0);
	if ( val0 == SatBool3::True ) {
	  // すでに充足していた．
	  continue;
	}

	if ( debug & debug_implication ) {
	  DOUT << "\t\texamining watcher clause " << (*c) << endl;
	}

	// nl の替わりのリテラルを見つける．
	// この時，替わりのリテラルが未定かすでに充足しているかどうか
	// は問題でない．
	bool found = false;
	SizeType n = c->lit_num();
	for ( SizeType i = 2; i < n; ++ i ) {
	  auto l2 = c->lit(i);
	  auto v = eval(l2);
	  if ( v != SatBool3::False ) {
	    // l2 を 1番めの watch literal にする．
	    c->xchange_wl1(i);
	    if ( debug & debug_implication ) {
	      DOUT << "\t\t\tsecond watching literal becomes "
		   << l2 << endl;
	    }
	    // w を l の watcher list から取り除き，
	    // ~l2 の watcher list に追加する．
	    -- wpos;
	    add_watcher(~l2, w);

	    found = true;
	    break;
	  }
	}
	if ( found ) {
	  continue;
	}

	if ( debug & debug_implication ) {
	  DOUT << "\t\tno other watching literals" << endl;
	}

	// 見付からなかったので l0 に従った割り当てを行う．
	if ( debug & debug_assign ) {
	  DOUT << "\tassign " << l0 << " @" << decision_level()
	       << " from " << w << ": " << l << endl;
	}
	if ( val0 == SatBool3::X ) {
	  assign(l0, w);
	}
	else {
	  // 矛盾がおこった．
	  if ( debug & debug_assign ) {
	    DOUT << "\t--> conflict(#" << mConflictNum
		 << ") with previous assignment" << endl
		 << "\t    " << ~l0 << " was assigned at level "
		 << decision_level(l0.varid()) << endl;
	  }

	  // この場合は w が矛盾の理由を表す節になっている．
	  conflict = w;
	  break;
	}
      }
    }
    wlist.move_elem(rpos, wnum, wpos);
  }
  mPropagationNum += prop_num;
  mSweep_props -= prop_num;

  return conflict;
}

// @brief level までバックトラックする
void
SatCore::backtrack(
  int level
)
{
  if ( debug & (debug_assign | debug_decision) ) {
    DOUT << endl
	 << "backtrack until @" << level << endl;
  }

  if ( level < decision_level() ) {
    mAssignList.backtrack(level);
    while ( mAssignList.has_elem() ) {
      auto p = mAssignList.get_prev();
      auto varid = p.varid();
      mVal[varid] = (mVal[varid] << 2) | conv_from_Bool3(SatBool3::X);
      push(varid);
      if ( debug & debug_assign ) {
	DOUT << "\tdeassign " << p << endl;
      }
    }
  }

  if ( debug & (debug_assign | debug_decision) ) {
    DOUT << endl;
  }
}

// @brief 充足された watcher を削除する．
void
SatCore::del_satisfied_watcher(
  Literal watch_lit
)
{
  // watch_lit に関係する watcher リストをスキャンして
  // literal watcher が充足していたらその watcher を削除する．
  auto& wlist = watcher_list(watch_lit);
  SizeType wpos = 0;
  SizeType n = wlist.size();
  for ( auto rpos = 0; rpos < n; ++ rpos ) {
    auto& w = wlist.elem(rpos);
    if ( w.is_literal() ) {
      auto val = eval(w.literal());
      if ( val == SatBool3::True ) {
	// この watcher は削除する．
	continue;
      }
    }
    wlist.set_elem(wpos, w);
    ++ wpos;
  }
  if ( wpos < n ) {
    wlist.erase(wpos);
  }
}

// 学習節のアクティビティを増加させる．
void
SatCore::bump_clause_activity(
  Clause* clause
)
{
  clause->increase_activity(mClauseBump);
  if ( clause->activity() > 1e+100 ) {
    for ( auto clause1_p: mLearntClauseList ) {
      clause1_p->factor_activity(1e-100);
    }
    mClauseBump *= 1e-100;
  }
}

#if 0 // forget_larnt_clause() で使うかも．
// @brief 変数のアクティビティを初期化する．
void
SatCore::reset_activity()
{
  for ( int i: Range(mVarSize) ) {
    mActivity[i] = 0.0;
  }
}
#endif

// @brief 現在の内部状態を得る．
SatStats
SatCore::get_stats() const
{
  SatStats stats;
  stats.mVarNum = variable_num();
  stats.mConstrClauseNum = mConstrClauseNum;
  stats.mConstrLitNum = literal_num();
  stats.mLearntClauseNum = learnt_clause_num() + learnt_bin_clause_num();
  stats.mLearntLitNum = learnt_literal_num();
  stats.mRestart = mRestartNum;
  stats.mDecisionNum = mDecisionNum;
  stats.mPropagationNum = mPropagationNum;
  stats.mConflictNum = mConflictNum;
  stats.mConflictLimit = conflict_limit();
  stats.mLearntLimit = learnt_limit();
  stats.mTime = mAccTime;
  return stats;
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
SatCore::reg_msg_handler(
  SatMsgHandler* msg_handler
)
{
  mMsgHandlerList.push_back(msg_handler);
}

// @brief ヘッダを出力する．
void
SatCore::print_header()
{
  for ( auto handler_p: mMsgHandlerList ) {
    SatMsgHandler& handler = *(handler_p);
    handler.print_header();
  }
}

// @brief 現在の状況を出力する．
void
SatCore::print_stats()
{
  auto stats = get_stats();
  for ( auto handler_p: mMsgHandlerList ) {
    SatMsgHandler& handler = *(handler_p);
    handler.print_message(stats);
  }
}

// @brief フッタを出力する．
void
SatCore::print_footer()
{
  auto stats = get_stats();
  for ( auto handler_p: mMsgHandlerList ) {
    SatMsgHandler& handler = *(handler_p);
    handler.print_footer(stats);
  }
}

END_NAMESPACE_YM_SAT
