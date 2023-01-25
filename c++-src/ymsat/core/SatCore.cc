
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


BEGIN_NAMESPACE_YM_SAT

const Literal Literal::X;

//////////////////////////////////////////////////////////////////////
// SatCore
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatCore::SatCore(
  const string& controller_type,
  const string& analyzer_type,
  const string& selecter_type,
  const unordered_map<string, string>& selopt_dic
) : mController{Controller::new_obj(*this, controller_type)},
    mAnalyzer{Analyzer::new_obj(*this, analyzer_type)},
    mSelecter{Selecter::new_obj(*this, selecter_type, selopt_dic)}
{
  mMaxConflict = 1024 * 100;

  mSweep_assigns = -1;
  mSweep_props = 0;

  mTmpLitsSize = 1024;
  mTmpLits = new Literal[mTmpLitsSize];

  mTmpBinClause = new_clause(2);
}

// @brief デストラクタ
SatCore::~SatCore()
{
  for ( auto c: mConstrClauseList ) {
    delete_clause(c);
  }
  for ( auto c: mLearntClauseList ) {
    delete_clause(c);
  }
  for ( int i: Range(mVarSize * 2) ) {
    mWatcherList[i].finish();
  }

  delete [] mVal;
  delete [] mDecisionLevel;
  delete [] mReason;
  delete [] mWatcherList;
#if YMSAT_USE_WEIGHTARRAY
  delete [] mWeightArray;
#endif
  delete [] mTmpLits;
}

// @brief 変数を追加する．
SatVarId
SatCore::new_variable(
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

// 実際に変数に関するデータ構造を生成する．
void
SatCore::alloc_var()
{
  if ( mOldVarNum < mVarNum ) {
    if ( mVarSize < mVarNum ) {
      expand_var();
    }
    for ( SizeType i: Range(mOldVarNum, mVarNum) ) {
      mVal[i] = conv_from_Bool3(SatBool3::X) | (conv_from_Bool3(SatBool3::X) << 2);
      mVarHeap.add_var(i);
    }
    mOldVarNum = mVarNum;
  }
}

// 変数に関する配列を拡張する．
void
SatCore::expand_var()
{
  // 古い配列を保存しておく．
  auto old_size = mVarSize;
  auto old_val = mVal;
  auto old_decision_level = mDecisionLevel;
  auto old_reason = mReason;
  auto old_watcher_list = mWatcherList;

  // 新しいサイズを計算する．
  if ( mVarSize == 0 ) {
    mVarSize = 1024;
  }
  while ( mVarSize < mVarNum ) {
    mVarSize <<= 1;
  }

  // 新しい配列を確保する．
  mVal = new ymuint8[mVarSize];
  mDecisionLevel = new int[mVarSize];
  mReason = new Reason[mVarSize];
  mWatcherList = new WatcherList[mVarSize * 2];

  // 古い配列から新しい配列へ内容をコピーする．
  for ( int i: Range(mOldVarNum) ) {
    mVal[i] = old_val[i];
    mDecisionLevel[i] = old_decision_level[i];
    mReason[i] = old_reason[i];
  }
  int n2 = mOldVarNum * 2;
  for ( int i: Range(n2) ) {
    mWatcherList[i].move(old_watcher_list[i]);
  }
  if ( old_size > 0 ) {
    delete [] old_val;
    delete [] old_decision_level;
    delete [] old_reason;
    delete [] old_watcher_list;
  }
  mAssignList.reserve(mVarNum);
  mVarHeap.alloc_var(mVarSize);
}

// @brief 節を追加する．
void
SatCore::add_clause(
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
  for ( SizeType i: Range(lit_num) ) {
    auto l = lits[i];
    mTmpLits[i] = Literal{l};
  }

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
  // - false literal の除去
  // - true literal を持つかどうかのチェック
  SizeType wpos = 0;
  for ( SizeType rpos: Range(lit_num) ) {
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

  if ( lit_num == 0 ) {
    // empty clause があったら unsat
    mSane = false;
    return;
  }

  // 単項節のリテラル数も含めることにする．
  mConstrLitNum += lit_num;

  auto l0 = mTmpLits[0];
  if ( lit_num == 1 ) {
    // unit clause があったら値の割り当てを行う．
    bool stat = check_and_assign(l0);

    if ( debug & debug_assign ) {
      cout << "add_clause: (" << l0 << ")" << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << endl;
      if ( !stat )  {
	cout << "\t--> conflict(#" << mConflictNum << ") with previous assignment" << endl
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

  ++ mConstrClauseNum;

  auto l1 = mTmpLits[1];

  if ( lit_num == 2 ) {
    if ( debug & debug_assign ) {
      cout << "add_clause: (" << l0 << " + " << l1 << ")" << endl;
    }

    mConstrBinList.push_back(BinClause{l0, l1});

    // watcher-list の設定
    add_watcher(~l0, Reason{l1});
    add_watcher(~l1, Reason{l0});
  }
  else {
    // 節の生成
    auto clause = new_clause(lit_num, mTmpLits);

    if ( debug & debug_assign ) {
      cout << "add_clause: " << (*clause) << endl;
    }

    mConstrClauseList.push_back(clause);

    // watcher-list の設定
    add_watcher(~l0, Reason{clause});
    add_watcher(~l1, Reason{clause});
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
    mSane = false;
    return;
  }

  auto l0 = lits[0];
  if ( n == 1 ) {
    // unit clause があったら値の割り当てを行う．
    bool stat = check_and_assign(l0);
    if ( debug & debug_assign ) {
      cout << "\tassign " << l0 << " @" << decision_level()
	   << endl;
      if ( !stat )  {
	cout << "\t--> conflict(#" << mConflictNum << " with previous assignment" << endl
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
  auto l1 = lits[1];
  if ( n == 2 ) {
    if ( debug & debug_assign ) {
      cout << "add_learnt_clause: "
	   << "(" << l0 << " + " << l1 << ")" << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << " from (" << l0 << " + " << l1 << ")" << endl;
    }

    // watcher-list の設定
    add_watcher(~l0, Reason(l1));
    add_watcher(~l1, Reason(l0));

    reason = Reason(l1);

    ++ mLearntBinNum;
  }
  else {
    // 節の生成
    alloc_lits(n);
    for ( SizeType i: Range(n) ) {
      mTmpLits[i] = lits[i];
    }
    auto clause = new_clause(n, true);

    if ( debug & debug_assign ) {
      cout << "add_learnt_clause: " << *clause << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << " from " << *clause << endl;
    }

    bump_clause_activity(clause);

    mLearntClauseList.push_back(clause);

    reason = Reason(clause);

    // watcher-list の設定
    add_watcher(~l0, reason);
    add_watcher(~l1, reason);
  }

  // learnt clause の場合には必ず unit clause になっているはず．
  ASSERT_COND( eval(l0) != SatBool3::False );

  assign(l0, reason);
}

// @brief 新しい節を生成する．
Clause*
SatCore::new_clause(
  SizeType lit_num,
  bool learnt
)
{
  SizeType size = sizeof(Clause) + sizeof(Literal) * (lit_num - 1);
  auto p = new char[size];
  auto clause = new (p) Clause{lit_num, mTmpLits, learnt};

  return clause;
}

// CNF を簡単化する．
void
SatCore::reduce_CNF()
{
  if ( !mSane ) {
    return;
  }
  ASSERT_COND( decision_level() == 0 );

  if ( implication() != Reason::None ) {
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
  for ( SatVarId var: Range(mVarNum) ) {
    if ( eval(var) == SatBool3::X ) {
      var_list.push_back(var);
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

// @brief clase が含意の理由になっているか調べる．
bool
SatCore::is_locked(
  Clause* clause
) const
{
  // 直感的には分かりにくいが，節の最初のリテラルは
  // 残りのリテラルによって含意されていることになっている．
  // そこで最初のリテラルの変数の割り当て理由が自分自身か
  // どうかを調べれば clause が割り当て理由として用いられて
  // いるかわかる．
  return reason(clause->wl0().varid()) == Reason{clause};
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

// @brief mTmpLits を確保する．
void
SatCore::alloc_lits(
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

// @brief 節を削除する．
void
SatCore::delete_clause(
  Clause* clause
)
{
  ASSERT_COND( clause->is_learnt() );

  if ( debug & debug_assign ) {
    cout << " delete_clause: " << (*clause) << endl;
  }

  // watch list を更新
  del_watcher(~clause->wl0(), Reason{clause});
  del_watcher(~clause->wl1(), Reason{clause});

  mLearntLitNum -= clause->lit_num();

  auto p = reinterpret_cast<char*>(clause);
  delete [] p;
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
    for ( auto clause_p: mConstrClauseList ) {
      cout << "  " << *clause_p << endl;
    }
    cout << " VarNum: " << variable_num() << endl;
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
  mRestartNum = 0;
  mConflictNum = 0;
  mDecisionNum = 0;
  mPropagationNum = 0;

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

  // assumption の割り当てを行う．
  for ( auto l: assumptions ) {
    set_marker();
    auto lit = Literal{l};
    bool stat = check_and_assign(lit);

    if ( debug & (debug_assign | debug_decision) ) {
      cout << endl
	   << "assume " << lit << " @" << decision_level()
	   << endl;
      if ( !stat )  {
	cout << "\t--> conflict(#" << mConflictNum << " with previous assignment" << endl
	     << "\t    " << ~lit << " was assigned at level "
	     << decision_level(lit.varid()) << endl;
      }
    }

    if ( stat ) {
      // 今の割当に基づく含意を行う．
      auto reason = implication();
      if ( reason != Reason::None ) {
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
  {
    int root_level = decision_level();
    if ( debug & (debug_assign | debug_decision) ) {
      cout << "RootLevel = " << root_level << endl;
    }
  }

  // 探索の本体
  for ( ; ; ) {
    // 探索の本体
    sat_stat = search();

    // メッセージ出力を行う．
    print_stats();

    if ( sat_stat != SatBool3::X ) {
      // 結果が求められた．
      break;
    }

    if ( !go_on() || conflict_num() == max_conflict() ) {
      // 制限値に達した．(アボート)
      break;
    }

    if ( debug & debug_assign ) {
      cout << "restart" << endl;
    }

    ++ mRestartNum;

    mController->_update_on_restart(restart_num());
  }

  if ( sat_stat == SatBool3::True ) {
    // SAT ならモデル(充足させる変数割り当てのリスト)を作る．
    model.resize(mVarNum);
    for ( SatVarId var: Range(mVarNum) ) {
      auto val = eval(var);
      ASSERT_COND( val == SatBool3::True || val == SatBool3::False );
      model.set(var, val);
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
    case SatBool3::True:  cout << "SAT" << endl; break;
    case SatBool3::False: cout << "UNSAT" << endl; break;
    case SatBool3::X:     cout << "UNKNOWN" << endl; break;
    default: ASSERT_NOT_REACHED;
    }
  }

  return sat_stat;
}

// @brief 探索を行う本体の関数
SatBool3
SatCore::search()
{
  int root_level = decision_level();

  SizeType cur_confl_num = 0;
  for ( ; ; ) {
    // キューにつまれている割り当てから含意される値の割り当てを行う．
    auto conflict = implication();
    if ( conflict != Reason::None ) {
      // 矛盾が生じた．
      ++ mConflictNum;
      ++ cur_confl_num;
      if ( decision_level() == root_level ) {
	// トップレベルで矛盾が起きたら充足不可能
	return SatBool3::False;
      }

      // 今の矛盾の解消に必要な条件を「学習」する．
      vector<Literal> learnt_lits;
      int bt_level = mAnalyzer->analyze(conflict, learnt_lits);

      if ( debug & debug_analyze ) {
	cout << endl
	     << "analyze for " << conflict << endl
	     << endl
	     << "learnt clause is ";
	const char* plus = "";
	for ( auto l: learnt_lits ) {
	  cout << plus << l << " @" << decision_level(l.varid());
	  plus = " + ";
	}
	cout << endl;
      }

      // バックトラック
      if ( bt_level < root_level ) {
	bt_level = root_level;
      }
      backtrack(bt_level);

      // 学習節の生成
      add_learnt_clause(learnt_lits);

      decay_var_activity();
      decay_clause_activity();

      // パラメータの更新
      mController->_update_on_conflict();

      continue;
    }

    if ( !mGoOn || (cur_confl_num >= mConflictLimit) ) {
      // 矛盾の回数が制限値を越えた．
      backtrack(root_level);
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
    auto lit = mSelecter->next_decision();
    if ( !lit.is_valid() ) {
      // すべての変数を割り当てた．
      // ということは充足しているはず．
      return SatBool3::True;
    }
    ++ mDecisionNum;

    // バックトラックポイントを記録
    set_marker();

    if ( debug & (debug_assign | debug_decision) ) {
      cout << endl
	   << "choose " << lit << " :"
	   << mVarHeap.activity(lit.varid())
	   << endl;
    }

    if ( debug & debug_assign ) {
      cout << "\tassign " << lit << " @" << decision_level() << endl;
    }

    // 選ばれたリテラルに基づいた割当を行う．
    // 未割り当ての変数を選んでいるのでエラーになるはずはない．
    assign(lit);
  }
}

// @brief 割当てキューに基づいて implication を行う．
Reason
SatCore::implication()
{
  SizeType prop_num = 0;
  auto conflict = Reason::None;
  while ( mAssignList.has_elem() ) {
    auto l = mAssignList.get_next();
    ++ prop_num;

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
	  assign(l0, Reason{nl});
	}
	else { // val0 == SatBool3::False ) {
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
	  conflict = Reason(mTmpBinClause);
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
	else { // l1 == nl
	  if ( debug & debug_implication ) {
	    // この assert は重いのでデバッグ時にしかオンにしない．
	    // ※ debug と debug_implication が const なので結果が0の
	    // ときにはコンパイル時に消されることに注意
	    ASSERT_COND(c->wl1() == nl );
	  }
	}

	auto val0 = eval(l0);
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
	SizeType n = c->lit_num();
	for ( SizeType i: Range(2, n) ) {
	  auto l2 = c->lit(i);
	  auto v = eval(l2);
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

// @brief level までバックトラックする
void
SatCore::backtrack(
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
      auto varid = p.varid();
      mVal[varid] = (mVal[varid] << 2) | conv_from_Bool3(SatBool3::X);
      push(varid);
      if ( debug & debug_assign ) {
	cout << "\tdeassign " << p << endl;
      }
    }
  }

  if ( debug & (debug_assign | debug_decision) ) {
    cout << endl;
  }
}

// @brief watcher を削除する．
void
SatCore::del_watcher(
  Literal watch_lit,
  Reason reason)
{
  auto w0 = reason;
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
SatCore::del_satisfied_watcher(
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
  for ( SizeType rpos: Range(n) ) {
    auto w = wlist.elem(rpos);
    if ( w.is_literal() ) {
      auto val = eval(w.literal());
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

// @brief 次に割り当てる変数を取り出す．
//
// アクティビティが最大で未割り当ての変数を返す．
SatVarId
SatCore::next_var()
{
  while ( !empty() ) {
    auto dvar = pop_top();
    if ( eval(dvar) == SatBool3::X ) {
      return dvar;
    }
  }
  return BAD_SATVARID;
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
