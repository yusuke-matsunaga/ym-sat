
/// @file CoreMgr.cc
/// @brief CoreMgr の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "CoreMgr.h"
#include "YmSat.h"
#include "SatAnalyzer.h"
#include "Selecter.h"
#include "ym/SatStats.h"
#include "ym/SatMsgHandler.h"


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
  mTmpLitsSize = 1024;
  mTmpLits = new SatLiteral[mTmpLitsSize];

  mTmpBinClause = new_clause(2);
}

// @brief デストラクタ
CoreMgr::~CoreMgr()
{
  for (ymuint i = 0; i < mVarSize * 2; ++ i) {
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
SatVarId
CoreMgr::new_var(bool decision)
{
  if ( decision_level() != 0 ) {
    // エラー
    cout << "Error!: decision_level() != 0" << endl;
    ASSERT_NOT_REACHED;
    return kSatVarIdIllegal;
  }

#if YMSAT_USE_DVAR
  mDvarArray.push_back(decision);
#endif

  // ここではカウンタを増やすだけ
  // 実際の処理は alloc_var() でまとめて行う．
  ymuint n = mVarNum;
  ++ mVarNum;
  return SatVarId(n);
}

// 実際に変数に関するデータ構造を生成する．
void
CoreMgr::alloc_var()
{
  if ( mOldVarNum < mVarNum ) {
    if ( mVarSize < mVarNum ) {
      expand_var();
    }
    for (ymuint i = mOldVarNum; i < mVarNum; ++ i) {
      mVal[i] = conv_from_Bool3(kB3X) | (conv_from_Bool3(kB3X) << 2);
      add_var(SatVarId(i));
    }
    mOldVarNum = mVarNum;
  }
}

// 変数に関する配列を拡張する．
void
CoreMgr::expand_var()
{
  // 古い配列を保存しておく．
  ymuint old_size = mVarSize;
  ymuint8* old_val = mVal;
  int* old_decision_level = mDecisionLevel;
  SatReason* old_reason = mReason;
  WatcherList* old_watcher_list = mWatcherList;
  ymint32* old_heap_pos = mHeapPos;
  double* old_activity = mActivity;
  ymuint32* old_heap = mHeap;

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
  mHeapPos = new ymint32[mVarSize];
  mActivity = new double[mVarSize];
  mHeap = new ymuint32[mVarSize];

  // 古い配列から新しい配列へ内容をコピーする．
  for (ymuint i = 0; i < mOldVarNum; ++ i) {
    mVal[i] = old_val[i];
    mDecisionLevel[i] = old_decision_level[i];
    mReason[i] = old_reason[i];
    mHeapPos[i] = old_heap_pos[i];
    mActivity[i] = old_activity[i];
  }
  ymuint n2 = mOldVarNum * 2;
  for (ymuint i = 0; i < n2; ++ i) {
    mWatcherList[i].move(old_watcher_list[i]);
  }
  for (ymuint i = 0; i < mHeapNum; ++ i) {
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
  mAssignList.reserve(mVarSize);
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
CoreMgr::add_clause(const vector<SatLiteral>& lits)
{
  ymuint n = lits.size();
  alloc_lits(n);
  for (ymuint i = 0; i < n; ++ i) {
    mTmpLits[i] = lits[i];
  }
  add_clause_sub(n);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
CoreMgr::add_clause(ymuint lit_num,
		   const SatLiteral* lits)
{
  alloc_lits(lit_num);
  for (ymuint i = 0; i < lit_num; ++ i) {
    mTmpLits[i] = lits[i];
  }
  add_clause_sub(lit_num);
}

// @brief 1項の節(リテラル)を追加する．
void
CoreMgr::add_clause(SatLiteral lit1)
{
  alloc_lits(1);
  mTmpLits[0] = lit1;

  // 節を追加する本体
  add_clause_sub(1);
}

// @brief 2項の節を追加する．
void
CoreMgr::add_clause(SatLiteral lit1,
		   SatLiteral lit2)
{
  alloc_lits(2);
  mTmpLits[0] = lit1;
  mTmpLits[1] = lit2;

  // 節を追加する本体
  add_clause_sub(2);
}

// @brief 3項の節を追加する．
void
CoreMgr::add_clause(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3)
{
  alloc_lits(3);
  mTmpLits[0] = lit1;
  mTmpLits[1] = lit2;
  mTmpLits[2] = lit3;

  // 節を追加する本体
  add_clause_sub(3);
}

// @brief 4項の節を追加する．
void
CoreMgr::add_clause(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4)
{
  alloc_lits(4);
  mTmpLits[0] = lit1;
  mTmpLits[1] = lit2;
  mTmpLits[2] = lit3;
  mTmpLits[3] = lit4;

  // 節を追加する本体
  add_clause_sub(4);
}

// @brief 5項の節を追加する．
void
CoreMgr::add_clause(SatLiteral lit1,
		   SatLiteral lit2,
		   SatLiteral lit3,
		   SatLiteral lit4,
		   SatLiteral lit5)
{
  alloc_lits(5);
  mTmpLits[0] = lit1;
  mTmpLits[1] = lit2;
  mTmpLits[2] = lit3;
  mTmpLits[3] = lit4;
  mTmpLits[4] = lit5;

  // 節を追加する本体
  add_clause_sub(5);
}

// @brief add_clause() の下請け関数
//
// リテラルの実体は mTmpLits[] に入っている．
void
CoreMgr::add_clause_sub(ymuint lit_num)
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
  ymuint wpos = 0;
  for (ymuint rpos = 0; rpos < lit_num; ++ rpos) {
    SatLiteral l = mTmpLits[rpos];
    if ( wpos != 0 && mTmpLits[wpos - 1] == l ) {
      // 重複している．
      continue;
    }
    SatBool3 v = eval(l);
    if ( v == kB3False ) {
      // false literal は追加しない．
      continue;
    }
    if ( v == kB3True ) {
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

    mConstrBinList.push_back(BinClause(l0, l1));
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

// @brief 学習節を追加する．
// @param[in] lits 追加するリテラルのリスト
void
CoreMgr::add_learnt_clause(const vector<SatLiteral>& lits)
{
  ymuint n = lits.size();
  mLearntLitNum += n;

  if ( n == 0 ) {
    // empty clause があったら unsat
    mSane = false;
    return;
  }

  SatLiteral l0 = lits[0];
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
  SatLiteral l1 = lits[1];
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
    for (ymuint i = 0; i < n; ++ i) {
      mTmpLits[i] = lits[i];
    }
    SatClause* clause = new_clause(n, true);
    mLearntClause.push_back(clause);

    reason = SatReason(clause);

    // watcher-list の設定
    add_watcher(~l0, reason);
    add_watcher(~l1, reason);
  }

  // learnt clause の場合には必ず unit clause になっているはず．
  ASSERT_COND( eval(l0) != kB3False );
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
CoreMgr::new_clause(ymuint lit_num,
		   bool learnt)
{
  ymuint size = sizeof(SatClause) + sizeof(SatLiteral) * (lit_num - 1);
  void* p = mAlloc.get_memory(size);
  SatClause* clause = new (p) SatClause(lit_num, mTmpLits, learnt);

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
CoreMgr::reduce_learnt_clause()
{
  ymuint n = mLearntClause.size();
  ymuint n2 = n / 2;

  // 足切りのための制限値
  double abs_limit = mClauseBump / n;

  sort(mLearntClause.begin(), mLearntClause.end(), SatClauseLess());

  vector<SatClause*>::iterator wpos = mLearntClause.begin();
  for (ymuint i = 0; i < n2; ++ i) {
    SatClause* clause = mLearntClause[i];
    if ( clause->lit_num() > 2 && !is_locked(clause) ) {
      delete_clause(clause);
    }
    else {
      *wpos = clause;
      ++ wpos;
    }
  }
  for (ymuint i = n2; i < n; ++ i) {
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

// @brief mTmpLits を確保する．
void
CoreMgr::alloc_lits(ymuint lit_num)
{
  ymuint old_size = mTmpLitsSize;
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
  del_watcher(~clause->wl0(), SatReason(clause));
  del_watcher(~clause->wl1(), SatReason(clause));

  mLearntLitNum -= clause->lit_num();

  ymuint size = sizeof(SatClause) + sizeof(SatLiteral) * (clause->lit_num() - 1);
  mAlloc.put_memory(size, static_cast<void*>(clause));
}

// @brief モデルを得る．
// @param[out] model 割り当て結果を格納する配列
void
CoreMgr::get_model(vector<SatBool3>& model)
{
  model.resize(mVarNum);
  for (ymuint i = 0; i < mVarNum; ++ i) {
    SatBool3 val = eval(SatVarId(i));
    ASSERT_COND( val == kB3True || val == kB3False );
    model[i] = val;
  }
}

// @brief パラメータの初期化を行う．
void
CoreMgr::init_param()
{
  mGoOn = true;
  mRestartNum = 0;
  mConflictNum = 0;
  mDecisionNum = 0;
  mPropagationNum = 0;
}

// @brief 仮定の割り当てを行う．
// @param[in] assumptions 割り当てる仮定のリスト
SatBool3
CoreMgr::assign_assumptions(const vector<SatLiteral>& assumptions)
{
  SatBool3 sat_stat = kB3True;
  for (vector<SatLiteral>::const_iterator p = assumptions.begin();
       p != assumptions.end(); ++ p) {
    SatLiteral lit = *p;

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
      SatReason reason = implication();
      if ( reason != kNullSatReason ) {
	// 矛盾が起こった．
	backtrack(0);
	sat_stat = kB3False;
	break;
      }
    }
  }

  return sat_stat;
}

// @brief SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @retval kB3True 充足した．
// @retval kB3False 充足不能が判明した．
// @retval kB3X わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
CoreMgr::solve(const vector<SatLiteral>& assumptions,
	       vector<SatBool3>& model,
	       YmSat& ymsat,
	       SatAnalyzer& analyzer,
	       Selecter& selecter)
{
  if ( debug & debug_solve ) {
    cout << "YmSat::solve starts" << endl;
    cout << " Assumptions: ";
    const char* and_str = "";
    for (vector<SatLiteral>::const_iterator p = assumptions.begin();
	 p != assumptions.end(); ++ p) {
      cout << and_str << *p;
      and_str = " & ";
    }
    cout << endl;
    cout << " Clauses:" << endl;
    for (ymuint i = 0; i < mConstrClauseList.size(); ++ i) {
      const SatClause* clause = mConstrClauseList[i];
      cout << "  " << *clause << endl;
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
  ymsat._solve_init();

  init_param();

  // 最終的な結果を納める変数
  SatBool3 sat_stat = kB3X;

  ASSERT_COND( decision_level() == 0 );

  // 自明な簡単化を行う．
  reduce_CNF();
  if ( !sane() ) {
    // その時点で充足不可能なら終わる．
    sat_stat = kB3False;
    goto end;
  }

  // assumption の割り当てを行う．
  sat_stat = assign_assumptions(assumptions);
  if ( sat_stat == kB3False ) {
    goto end;
  }

  // 以降，現在のレベルが基底レベルとなる．
  {
    ymuint root_level = decision_level();
    if ( debug & (debug_assign | debug_decision) ) {
      cout << "RootLevel = " << root_level << endl;
    }
  }

  // 探索の本体
  for ( ; ; ) {
    // 探索の本体
    sat_stat = search(ymsat, analyzer, selecter);

    // メッセージ出力を行う．
    print_stats();

    if ( sat_stat != kB3X ) {
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

    ymsat._update_on_restart(restart_num());
  }

  if ( sat_stat == kB3True ) {
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
    case kB3True:  cout << "SAT" << endl; break;
    case kB3False: cout << "UNSAT" << endl; break;
    case kB3X:     cout << "UNKNOWN" << endl; break;
    default: ASSERT_NOT_REACHED;
    }
  }

  return sat_stat;
}

// @brief 探索を行う本体の関数
// @param[in] ymsat YmSat オブジェクト
// @param[in] analyzer SatAnalyzer オブジェクト
// @param[in] selecter Selecter オブジェクト
// @retval kB3True 充足した．
// @retval kB3False 充足できないことがわかった．
// @retval kB3X 矛盾の生起回数が mConflictLimit を超えた．
//
// 矛盾の結果新たな学習節が追加される場合もあるし，
// 内部で reduce_learnt_clause() を呼んでいるので学習節が
// 削減される場合もある．
SatBool3
CoreMgr::search(YmSat& ymsat,
		SatAnalyzer& analyzer,
		Selecter& selecter)
{
  ymuint root_level = decision_level();

  ++ mRestartNum;

  ymuint cur_confl_num = 0;
  for ( ; ; ) {
    // キューにつまれている割り当てから含意される値の割り当てを行う．
    SatReason conflict = implication();
    if ( conflict != kNullSatReason ) {
      // 矛盾が生じた．
      ++ mConflictNum;
      ++ cur_confl_num;
      if ( decision_level() == root_level ) {
	// トップレベルで矛盾が起きたら充足不可能
	return kB3False;
      }

      // 今の矛盾の解消に必要な条件を「学習」する．
      vector<SatLiteral> learnt_lits;
      int bt_level = analyzer.analyze(conflict, learnt_lits);

      if ( debug & debug_analyze ) {
	cout << endl
	     << "analyze for " << conflict << endl
	     << endl
	     << "learnt clause is ";
	const char* plus = "";
	for (ymuint i = 0; i < learnt_lits.size(); ++ i) {
	  SatLiteral l = learnt_lits[i];
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
      ymsat._update_on_conflict();

      continue;
    }

    if ( !mGoOn || (cur_confl_num >= mConflictLimit) ) {
      // 矛盾の回数が制限値を越えた．
      backtrack(root_level);
      return kB3X;
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
    SatLiteral lit = selecter.next_decision(*this);
    if ( lit == kSatLiteralX ) {
      // すべての変数を割り当てた．
      // ということは充足しているはず．
      return kB3True;
    }
    ++ mDecisionNum;

    // バックトラックポイントを記録
    set_marker();

    if ( debug & (debug_assign | debug_decision) ) {
      cout << endl
	   << "choose " << lit << " :"
	   << mActivity[lit.varid().val()] << endl;
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
    ymuint n = wlist.num();
    ymuint rpos = 0;
    ymuint wpos = 0;
    while ( rpos < n ) {
      Watcher w = wlist.elem(rpos);
      wlist.set_elem(wpos, w);
      ++ rpos;
      ++ wpos;
      if ( w.is_literal() ) {
	// 2-リテラル節の場合は相方のリテラルに基づく値の割り当てを行う．
	SatLiteral l0 = w.literal();
	SatBool3 val0 = eval(l0);
	if ( val0 == kB3X ) {
	  if ( debug & debug_assign ) {
	    cout << "\tassign " << l0 << " @" << decision_level()
		 << " from " << l << endl;
	  }
	  assign(l0, SatReason(nl));
	}
	else if ( val0 == kB3False ) {
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
	if ( val0 == kB3True ) {
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
	ymuint n = c->lit_num();
	for (ymuint i = 2; i < n; ++ i) {
	  SatLiteral l2 = c->lit(i);
	  if ( eval(l2) != kB3False ) {
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
	if ( val0 == kB3X ) {
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
      SatLiteral p = mAssignList.get_prev();
      SatVarId varid = p.varid();
      ymuint vindex = varid.val();
      mVal[vindex] = (mVal[vindex] << 2) | conv_from_Bool3(kB3X);
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
CoreMgr::bump_var_activity(SatVarId varid)
{
  ymuint vindex = varid.val();
  double& act = mActivity[vindex];
  act += mVarBump;
  if ( act > 1e+100 ) {
    for (ymuint i = 0; i < mVarNum; ++ i) {
      mActivity[i] *= 1e-100;
    }
    mVarBump *= 1e-100;
  }
  ymint pos = mHeapPos[vindex];
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
  Watcher w0(reason);
  WatcherList& wlist = watcher_list(watch_lit);
  ymuint n = wlist.num();
  ymuint wpos = 0;
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

// @brief 次に割り当てる変数を取り出す．
//
// アクティビティが最大で未割り当ての変数を返す．
SatVarId
CoreMgr::next_var()
{
  while ( !empty() ) {
    ymuint vindex = pop_top();
    SatVarId dvar(vindex);
    if ( eval(dvar) == kB3X ) {
      return dvar;
    }
  }
  return kSatVarIdIllegal;
}

// 学習節のアクティビティを増加させる．
void
CoreMgr::bump_clause_activity(SatClause* clause)
{
  clause->increase_activity(mClauseBump);
  if ( clause->activity() > 1e+100 ) {
    for (vector<SatClause*>::iterator p = mLearntClause.begin();
	 p != mLearntClause.end(); ++ p) {
      SatClause* clause1 = *p;
      clause1->factor_activity(1e-100);
    }
    mClauseBump *= 1e-100;
  }
}

#if 0 // forget_larnt_clause() で使うかも．
// @brief 変数のアクティビティを初期化する．
void
CoreMgr::reset_activity()
{
  for (ymuint i = 0; i < mVarSize; ++ i) {
    mActivity[i] = 0.0;
  }
}

// @brief 与えられた変数のリストからヒープ木を構成する．
void
CoreMgr::build(const vector<SatVarId>& var_list)
{
  for (ymuint i = 0; i < mVarSize; ++ i) {
    mHeapPos[i] = -1;
  }
  mHeapNum = 0;
  ASSERT_COND( var_list.size() <= mVarSize );

  for (ymuint i = 0; i < var_list.size(); ++ i) {
    SatVarId var = var_list[i];
    ymuint vindex = var.val();
    ++ mHeapNum;
    set(vindex, i);
  }
  for (ymuint i = (mHeapNum / 2); i > 0; ) {
    -- i;
    move_down(i);
  }
}
#endif

// 引数の位置にある要素を適当な位置まで沈めてゆく
void
CoreMgr::move_down(ymuint pos)
{
  ymuint vindex_p = mHeap[pos];
  double val_p = mActivity[vindex_p];
  for ( ; ; ) {
    // ヒープ木の性質から親から子の位置がわかる
    ymuint pos_l = left(pos);
    ymuint pos_r = pos_l + 1;
    if ( pos_r > mHeapNum ) {
      // 左右の子どもを持たない場合
      break;
    }
    // 左右の子供のうちアクティビティの大きい方を pos_c とする．
    // 同点なら左を選ぶ．
    ymuint pos_c = pos_l;
    ymuint vindex_c = mHeap[pos_c];
    double val_c = mActivity[vindex_c];
    if ( pos_r < mHeapNum ) {
      ymuint vindex_r = mHeap[pos_r];
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
  ymuint j = 0;
  ymuint nc = 1;
  const char* spc = "";
  for (ymuint i = 0; i < mHeapNum; ++ i) {
    ymuint vindex = mHeap[i];
    ASSERT_COND( mHeapPos[vindex] == static_cast<ymint>(i) );
    if ( i > 0 ) {
      ymint p = parent(i);
      ymuint pindex = mHeap[p];
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
  stats.mRestart = restart_num();
  stats.mDecisionNum = decision_num();
  stats.mPropagationNum = propagation_num();
  stats.mConflictNum = conflict_num();
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
  for (list<SatMsgHandler*>::iterator p = mMsgHandlerList.begin();
       p != mMsgHandlerList.end(); ++ p) {
    SatMsgHandler& handler = *(*p);
    handler.print_header();
  }
}

// @brief 現在の状況を出力する．
void
CoreMgr::print_stats()
{
  SatStats stats;
  get_stats(stats);
  for (list<SatMsgHandler*>::iterator p = mMsgHandlerList.begin();
       p != mMsgHandlerList.end(); ++ p) {
    SatMsgHandler& handler = *(*p);
    handler.print_message(stats);
  }
}

// @brief フッタを出力する．
void
CoreMgr::print_footer()
{
  SatStats stats;
  get_stats(stats);
  for (list<SatMsgHandler*>::iterator p = mMsgHandlerList.begin();
       p != mMsgHandlerList.end(); ++ p) {
    SatMsgHandler& handler = *(*p);
    handler.print_footer(stats);
  }
}

END_NAMESPACE_YM_SAT
