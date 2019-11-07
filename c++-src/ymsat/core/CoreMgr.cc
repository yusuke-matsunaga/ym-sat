
/// @file CoreMgr.cc
/// @brief CoreMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "../CoreMgr.h"
#include "../Controller.h"
#include "../Analyzer.h"
#include "../SatClause.h"
#include "../Selecter.h"
#include "ym/SatStats.h"
#include "ym/SatModel.h"
#include "ym/SatMsgHandler.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// CoreMgr
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
CoreMgr::CoreMgr() :
  mVarBump(1.0),
  mVarDecay(0.95),
  mAlloc(4096),
  mSane(true),
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
  mHeapPos(nullptr),
  mActivity(nullptr),
  mHeap(nullptr),
  mHeapNum(0),

  mRestartNum(0),
  mConflictNum(0),
  mDecisionNum(0),
  mPropagationNum(0),
  mConflictLimit(0),
  mLearntLimit(0),
  mMaxConflict(1024 * 100)
{
  mSweep_assigns = -1;
  mSweep_props = 0;

  mTmpLitsSize = 1024;
  mTmpLits = new SatLiteral[mTmpLitsSize];

  mTmpBinClause = new_clause(2);
}

// @brief デストラクタ
CoreMgr::~CoreMgr()
{
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
  delete [] mHeapPos;
  delete [] mActivity;
  delete [] mHeap;
  delete [] mTmpLits;
}

// @brief 変数を追加する．
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
int
CoreMgr::new_variable(bool decision)
{
  if ( decision_level() != 0 ) {
    // エラー
    cout << "Error!: decision_level() != 0" << endl;
    ASSERT_NOT_REACHED;
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

// 実際に変数に関するデータ構造を生成する．
void
CoreMgr::alloc_var()
{
  if ( mOldVarNum < mVarNum ) {
    if ( mVarSize < mVarNum ) {
      expand_var();
    }
    for ( int i: Range(mOldVarNum, mVarNum) ) {
      mVal[i] = conv_from_Bool3(SatBool3::X) | (conv_from_Bool3(SatBool3::X) << 2);
      add_var(i);
    }
    mOldVarNum = mVarNum;
  }
}

// 変数に関する配列を拡張する．
void
CoreMgr::expand_var()
{
  // 古い配列を保存しておく．
  int old_size = mVarSize;
  ymuint8* old_val = mVal;
  int* old_decision_level = mDecisionLevel;
  SatReason* old_reason = mReason;
  WatcherList* old_watcher_list = mWatcherList;
  int* old_heap_pos = mHeapPos;
  double* old_activity = mActivity;
  int* old_heap = mHeap;

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
  mReason = new SatReason[mVarSize];
  mWatcherList = new WatcherList[mVarSize * 2];
  mHeapPos = new int[mVarSize];
  mActivity = new double[mVarSize];
  mHeap = new int[mVarSize];

  // 古い配列から新しい配列へ内容をコピーする．
  for ( int i: Range(mOldVarNum) ) {
    mVal[i] = old_val[i];
    mDecisionLevel[i] = old_decision_level[i];
    mReason[i] = old_reason[i];
    mHeapPos[i] = old_heap_pos[i];
    mActivity[i] = old_activity[i];
  }
  int n2 = mOldVarNum * 2;
  for ( int i: Range(n2) ) {
    mWatcherList[i].move(old_watcher_list[i]);
  }
  for ( int i: Range(mHeapNum) ) {
    mHeap[i] = old_heap[i];
  }
  if ( old_size > 0 ) {
    delete [] old_val;
    delete [] old_decision_level;
    delete [] old_reason;
    delete [] old_watcher_list;
    delete [] old_heap_pos;
    delete [] old_activity;
    delete [] old_heap;
  }
  mAssignList.reserve(mVarNum);
}

// @brief 条件リテラルのリストを設定する．
// @param[in] lits リテラルのベクタ
void
CoreMgr::set_conditional_literals(const vector<SatLiteral>& lits)
{
  int lit_num = lits.size();
  mCondLits.clear();
  mCondLits.resize(lit_num);
  for ( int i: Range(lit_num) ) {
    mCondLits[i] = lits[i];
  }
}

// @brief 条件リテラルのリストを設定する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
CoreMgr::set_conditional_literals(int lit_num,
				  const SatLiteral* lits)
{
  mCondLits.clear();
  mCondLits.resize(lit_num);
  for ( int i: Range(lit_num) ) {
    mCondLits[i] = lits[i];
  }
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
CoreMgr::add_clause(const vector<SatLiteral>& lits)
{
  int lit_num = lits.size();
  int n2 = mCondLits.size();
  alloc_lits(lit_num + n2);
  for ( int i: Range(lit_num) ) {
    mTmpLits[i] = lits[i];
  }
  for ( int i: Range(n2) ) {
    mTmpLits[lit_num + i] = ~mCondLits[i];
  }
  add_clause_sub(lit_num + n2);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
CoreMgr::add_clause(int lit_num,
		    const SatLiteral* lits)
{
  int n2 = mCondLits.size();
  alloc_lits(lit_num + n2);
  for ( int i: Range(lit_num) ) {
    mTmpLits[i] = lits[i];
  }
  for ( int i: Range(n2) ) {
    mTmpLits[lit_num + i] = ~mCondLits[i];
  }
  add_clause_sub(lit_num + n2);
}

// @brief add_clause() の下請け関数
//
// リテラルの実体は mTmpLits[] に入っている．
void
CoreMgr::add_clause_sub(int lit_num)
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
  for ( int rpos: Range(lit_num) ) {
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

    mConstrUnitList.push_back(l0);

    return;
  }

  auto l1 = mTmpLits[1];

  if ( lit_num == 2 ) {
    // watcher-list の設定
    add_watcher(~l0, SatReason(l1));
    add_watcher(~l1, SatReason(l0));

    mConstrBinList.push_back(BinClause(l0, l1));
  }
  else {
    // 節の生成
    auto clause = new_clause(lit_num, mTmpLits);
    mConstrClauseList.push_back(clause);

    // watcher-list の設定
    add_watcher(~l0, SatReason(clause));
    add_watcher(~l1, SatReason(clause));
  }
}

// @brief 学習節を追加する．
// @param[in] lits 追加するリテラルのリスト
void
CoreMgr::add_learnt_clause(const vector<SatLiteral>& lits)
{
  int n = lits.size();
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
  auto l1 = lits[1];
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
    for ( int i: Range(n) ) {
      mTmpLits[i] = lits[i];
    }
    auto clause = new_clause(n, true);
    mLearntClauseList.push_back(clause);

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

// @brief 新しい節を生成する．
// @param[in] lit_num リテラル数
// @param[in] learnt 学習節のとき true とするフラグ
// @note リテラルは mTmpLits に格納されている．
SatClause*
CoreMgr::new_clause(int lit_num,
		    bool learnt)
{
  int size = sizeof(SatClause) + sizeof(SatLiteral) * (lit_num - 1);
  auto p = mAlloc.get_memory(size);
  auto clause = new (p) SatClause(lit_num, mTmpLits, learnt);

  return clause;
}

// CNF を簡単化する．
void
CoreMgr::reduce_CNF()
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
  vector<int> var_list;
  var_list.reserve(mVarNum);
  for ( int var: Range(mVarNum) ) {
    if ( eval(var) == SatBool3::X ) {
      var_list.push_back(var);
    }
    else {
      del_satisfied_watcher(SatLiteral::conv_from_varid(var, false));
      del_satisfied_watcher(SatLiteral::conv_from_varid(var, true));
    }
  }
  build(var_list);

  // 現在の状況を記録しておく．
  mSweep_assigns = mAssignList.size();
  mSweep_props = mConstrLitNum + mLearntLitNum;
}

// @brief 充足している節を取り除く
// @param[in] clause_list 節のリスト
void
CoreMgr::sweep_clause(vector<SatClause*>& clause_list)
{
  int n = clause_list.size();
  int wpos = 0;
  for ( int rpos: Range(n) ) {
    auto c{clause_list[rpos]};
    int nl = c->lit_num();
    bool satisfied = false;
    for ( int i: Range(nl) ) {
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

// @brief clase が含意の理由になっているか調べる．
bool
CoreMgr::is_locked(SatClause* clause) const
{
  // 直感的には分かりにくいが，節の最初のリテラルは
  // 残りのリテラルによって含意されていることになっている．
  // そこで最初のリテラルの変数の割り当て理由が自分自身か
  // どうかを調べれば clause が割り当て理由として用いられて
  // いるかわかる．
  return reason(clause->wl0().varid()) == SatReason(clause);
}

// 使われていない学習節を削除する．
void
CoreMgr::reduce_learnt_clause()
{
  int n = mLearntClauseList.size();
  int n2 = n / 2;

  // 足切りのための制限値
  double abs_limit = mClauseBump / n;

  sort(mLearntClauseList.begin(), mLearntClauseList.end(), SatClauseLess());

  auto wpos{mLearntClauseList.begin()};
  for ( int i: Range(n2) ) {
    auto clause{mLearntClauseList[i]};
    if ( clause->lit_num() > 2 && !is_locked(clause) ) {
      delete_clause(clause);
    }
    else {
      *wpos = clause;
      ++ wpos;
    }
  }
  for ( int i: Range(n2, n) ) {
    auto clause{mLearntClauseList[i]};
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
CoreMgr::alloc_lits(int lit_num)
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

// @brief 節を削除する．
// @param[in] clause 削除する節
void
CoreMgr::delete_clause(SatClause* clause)
{
  ASSERT_COND( clause->is_learnt() );

  // watch list を更新
  del_watcher(~clause->wl0(), SatReason{clause});
  del_watcher(~clause->wl1(), SatReason{clause});

  mLearntLitNum -= clause->lit_num();

  int size = sizeof(SatClause) + sizeof(SatLiteral) * (clause->lit_num() - 1);
  mAlloc.put_memory(size, static_cast<void*>(clause));
}

// @brief モデルを得る．
// @param[out] model 割り当て結果を格納する配列
void
CoreMgr::get_model(SatModel& model)
{
  model.resize(mVarNum);
  for ( int var: Range(mVarNum) ) {
    auto val{eval(var)};
    ASSERT_COND( val == SatBool3::True || val == SatBool3::False );
    model.set(var, val);
  }
}

// @brief SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @param[in] controller コントローラー
// @param[in] analyzer 解析器
// @param[in] selecter リテラル選択器
// @retval SatBool3::True 充足した．
// @retval SatBool3::False 充足不能が判明した．
// @retval SatBool3::X わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
CoreMgr::solve(const vector<SatLiteral>& assumptions,
	       SatModel& model,
	       Controller& controller,
	       Analyzer& analyzer,
	       Selecter& selecter)
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
  analyzer.alloc_var(variable_num());

  // パラメータの初期化
  controller._init();

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
  for ( auto lit: assumptions ) {
    set_marker();
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
      auto reason = implication();
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
  {
    int root_level = decision_level();
    if ( debug & (debug_assign | debug_decision) ) {
      cout << "RootLevel = " << root_level << endl;
    }
  }

  // 探索の本体
  for ( ; ; ) {
    // 探索の本体
    sat_stat = search(controller, analyzer, selecter);

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

    controller._update_on_restart(restart_num());
  }

  if ( sat_stat == SatBool3::True ) {
    // SAT ならモデル(充足させる変数割り当てのリスト)を作る．
    get_model(model);
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
// @param[in] controller Controller オブジェクト
// @param[in] analyzer Analyzer オブジェクト
// @param[in] selecter Selecter オブジェクト
// @retval SatBool3::True 充足した．
// @retval SatBool3::False 充足できないことがわかった．
// @retval SatBool3::X 矛盾の生起回数が mConflictLimit を超えた．
//
// 矛盾の結果新たな学習節が追加される場合もあるし，
// 内部で reduce_learnt_clause() を呼んでいるので学習節が
// 削減される場合もある．
SatBool3
CoreMgr::search(Controller& controller,
		Analyzer& analyzer,
		Selecter& selecter)
{
  int root_level = decision_level();

  ++ mRestartNum;

  int cur_confl_num = 0;
  for ( ; ; ) {
    // キューにつまれている割り当てから含意される値の割り当てを行う．
    auto conflict = implication();
    if ( conflict != kNullSatReason ) {
      // 矛盾が生じた．
      ++ mConflictNum;
      ++ cur_confl_num;
      if ( decision_level() == root_level ) {
	// トップレベルで矛盾が起きたら充足不可能
	return SatBool3::False;
      }

      // 今の矛盾の解消に必要な条件を「学習」する．
      vector<SatLiteral> learnt_lits;
      int bt_level;
      tie(bt_level, learnt_lits) = analyzer.analyze(conflict);

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
      controller._update_on_conflict();

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
    auto lit = selecter.next_decision();
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
	   << mActivity[lit.varid()] << endl;
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
// @return 矛盾が生じたら矛盾の原因を返す．
//
// 矛盾が生じていなかったら kNullSatReason を返す．
// 矛盾の原因とは実際には充足していない節である．
SatReason
CoreMgr::implication()
{
  int prop_num = 0;
  auto conflict = kNullSatReason;
  while ( mAssignList.has_elem() ) {
    auto l = mAssignList.get_next();
    ++ prop_num;

    if ( debug & debug_implication ) {
      cout << "\tpick up " << l << endl;
    }
    // l の割り当てによって無効化された watcher-list の更新を行う．
    auto nl = ~l;

    auto& wlist = watcher_list(l);
    int n = wlist.num();
    int rpos = 0;
    int wpos = 0;
    while ( rpos < n ) {
      auto w = wlist.elem(rpos);
      wlist.set_elem(wpos, w);
      ++ rpos;
      ++ wpos;
      if ( w.is_literal() ) {
	// 2-リテラル節の場合は相方のリテラルに基づく値の割り当てを行う．
	auto l0{w.literal()};
	auto val0{eval(l0)};
	if ( val0 == SatBool3::X ) {
	  if ( debug & debug_assign ) {
	    cout << "\tassign " << l0 << " @" << decision_level()
		 << " from " << l << endl;
	  }
	  assign(l0, SatReason{nl});
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
	auto c{w.clause()};
	auto l0{c->wl0()};
	if ( l0 == nl ) {
	  // nl を 1番めのリテラルにする．
	  c->xchange_wl();
	  // 新しい wl0 を得る．
	  l0 = c->wl0();
	}
	else { // l1 == nl
	  ASSERT_COND( c->wl1() == nl );
	}

	auto val0{eval(l0)};
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
	for ( int i: Range(2, n) ) {
	  auto l2{c->lit(i)};
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
	    watcher_list(~l2).add(Watcher(w));

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
  mPropagationNum += prop_num;
  mSweep_props -= prop_num;

  return conflict;
}

// @brief level までバックトラックする
// @param[in] level バックトラックするレベル
void
CoreMgr::backtrack(int level)
{
  if ( debug & (debug_assign | debug_decision) ) {
    cout << endl
	 << "backtrack until @" << level << endl;
  }

  if ( level < decision_level() ) {
    mAssignList.backtrack(level);
    while ( mAssignList.has_elem() ) {
      auto p{mAssignList.get_prev()};
      int varid = p.varid();
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

// 変数のアクティビティを増加させる．
void
CoreMgr::bump_var_activity(int varid)
{
  double& act = mActivity[varid];
  act += mVarBump;
  if ( act > 1e+100 ) {
    for ( int i: Range(mVarNum) ) {
      mActivity[i] *= 1e-100;
    }
    mVarBump *= 1e-100;
  }
  int pos = mHeapPos[varid];
  if ( pos > 0 ) {
    move_up(pos);
  }
}

// @brief watcher を削除する．
// @param[in] watch_lit リテラル
// @param[in] reason 理由
void
CoreMgr::del_watcher(SatLiteral watch_lit,
		     SatReason reason)
{
  auto w0{reason};
  auto& wlist{watcher_list(watch_lit)};
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
    auto w{wlist.elem(wpos + 1)};
    wlist.set_elem(wpos, w);
  }
  wlist.erase(n);
}

// @brief 充足された watcher を削除する．
// @param[in] watch_lit リテラル
void
CoreMgr::del_satisfied_watcher(SatLiteral watch_lit)
{
  // watch_lit に関係する watcher リストをスキャンして
  // literal watcher が充足していたらその watcher を削除する．
  // watcher リストを配列で実装しているので
  // あたまからスキャンして該当の要素以降を
  // 1つづつ前に詰める．
  auto& wlist{watcher_list(watch_lit)};
  int n = wlist.num();
  int wpos = 0;
  for ( int rpos: Range(n) ) {
    auto w{wlist.elem(rpos)};
    if ( w.is_literal() ) {
      auto val{ eval(w.literal()) };
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
int
CoreMgr::next_var()
{
  while ( !empty() ) {
    int dvar = pop_top();
    if ( eval(dvar) == SatBool3::X ) {
      return dvar;
    }
  }
  return -1;
}

// 学習節のアクティビティを増加させる．
void
CoreMgr::bump_clause_activity(SatClause* clause)
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
CoreMgr::reset_activity()
{
  for ( int i: Range(mVarSize) ) {
    mActivity[i] = 0.0;
  }
}
#endif

// @brief 与えられた変数のリストからヒープ木を構成する．
void
CoreMgr::build(const vector<int>& var_list)
{
  for ( int i: Range(mVarSize) ) {
    mHeapPos[i] = -1;
  }
  mHeapNum = 0;
  ASSERT_COND( var_list.size() <= mVarSize );

  for ( int i: Range(var_list.size()) ) {
    auto var = var_list[i];
    ++ mHeapNum;
    set(var, i);
  }
  for ( int i = (mHeapNum / 2); i > 0; ) {
    -- i;
    move_down(i);
  }
}

// 引数の位置にある要素を適当な位置まで沈めてゆく
void
CoreMgr::move_down(int pos)
{
  int vindex_p = mHeap[pos];
  double val_p = mActivity[vindex_p];
  for ( ; ; ) {
    // ヒープ木の性質から親から子の位置がわかる
    int pos_l = left(pos);
    int pos_r = pos_l + 1;
    if ( pos_r > mHeapNum ) {
      // 左右の子どもを持たない場合
      break;
    }
    // 左右の子供のうちアクティビティの大きい方を pos_c とする．
    // 同点なら左を選ぶ．
    int pos_c = pos_l;
    int vindex_c = mHeap[pos_c];
    double val_c = mActivity[vindex_c];
    if ( pos_r < mHeapNum ) {
      int vindex_r = mHeap[pos_r];
      double val_r = mActivity[vindex_r];
      if ( val_c < val_r ) {
	pos_c = pos_r;
	vindex_c = vindex_r;
	val_c = val_r;
      }
    }
    // 子供のアクティビティが親を上回らなければ終わり
    if ( val_c <= val_p ) {
      break;
    }
    // 逆転
    set(vindex_p, pos_c);
    set(vindex_c, pos);
    pos = pos_c;
  }
}

// @brief 内容を出力する
void
CoreMgr::dump_heap(ostream& s) const
{
  s << "heap num = " << mHeapNum << endl;
  int j = 0;
  int nc = 1;
  const char* spc = "";
  for ( int i: Range(mHeapNum) ) {
    int vindex = mHeap[i];
    ASSERT_COND( mHeapPos[vindex] == i );
    if ( i > 0 ) {
      int p = parent(i);
      int pindex = mHeap[p];
      ASSERT_COND( mActivity[pindex] >= mActivity[vindex] );
    }
    s << spc << vindex << "("
      << mActivity[vindex]
      << ")";
    ++ j;
    if ( j == nc ) {
      j = 0;
      nc <<= 1;
      s << endl;
      spc = "";
    }
    else {
      spc = " ";
    }
  }
  if ( j > 0 ) {
    s << endl;
  }
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
CoreMgr::get_stats(SatStats& stats) const
{
  stats.mVarNum = variable_num();
  stats.mConstrClauseNum = clause_num();
  stats.mConstrLitNum = literal_num();
  stats.mLearntClauseNum = learnt_clause_num() + learnt_bin_clause_num();
  stats.mLearntLitNum = learnt_literal_num();
  stats.mRestart = mRestartNum;
  stats.mDecisionNum = mDecisionNum;
  stats.mPropagationNum = mPropagationNum;
  stats.mConflictNum = mConflictNum;
  stats.mConflictLimit = conflict_limit();
  stats.mLearntLimit = learnt_limit();
  stats.mTime = mTimer.time();
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
CoreMgr::reg_msg_handler(SatMsgHandler* msg_handler)
{
  mMsgHandlerList.push_back(msg_handler);
}

// @brief ヘッダを出力する．
void
CoreMgr::print_header()
{
  for ( auto handler_p: mMsgHandlerList ) {
    SatMsgHandler& handler = *(handler_p);
    handler.print_header();
  }
}

// @brief 現在の状況を出力する．
void
CoreMgr::print_stats()
{
  SatStats stats;
  get_stats(stats);
  for ( auto handler_p: mMsgHandlerList ) {
    SatMsgHandler& handler = *(handler_p);
    handler.print_message(stats);
  }
}

// @brief フッタを出力する．
void
CoreMgr::print_footer()
{
  SatStats stats;
  get_stats(stats);
  for ( auto handler_p: mMsgHandlerList ) {
    SatMsgHandler& handler = *(handler_p);
    handler.print_footer(stats);
  }
}

END_NAMESPACE_YM_SAT
