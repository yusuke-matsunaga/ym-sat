
/// @file YmSat_base.cc
/// @brief YmSat の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "YmSat.h"
#include "ym/SatStats.h"
#include "ym/SatMsgHandler.h"
#include "SatAnalyzer.h"
#include "SatClause.h"


BEGIN_NAMESPACE_YM_YMSATOLD

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
  mTmpLits = new SatLiteral[mTmpLitsSize];

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
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
int
YmSat::new_variable(bool decision)
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
  // add_clause_sub() 中でリテラルの並び替えを行うので
  // 一旦 mTmpLits にコピーする．
  int lit_num = lits.size();
  int n2 = mCondLits.size();
  alloc_lits(lit_num + n2);
  for ( int i = 0; i < lit_num; ++ i ) {
    mTmpLits[i] = lits[i];
  }
  for ( int i = 0; i < n2; ++ i ) {
    auto l{mCondLits[i]};
    // 極性が反転することに注意
    mTmpLits[lit_num + i] = ~l;
  }

  // 節を追加する本体
  add_clause_sub(lit_num + n2);
}

// @brief 節を追加する．
// @param[in] lit_num リテラル数
// @param[in] lits リテラルの配列
void
YmSat::add_clause(int lit_num,
		  const SatLiteral* lits)
{
  // add_clause_sub() 中でリテラルの並び替えを行うので
  // 一旦 mTmpLits にコピーする．
  int n2 = mCondLits.size();
  alloc_lits(lit_num + n2);
  for ( int i = 0; i < lit_num; ++ i ) {
    mTmpLits[i] = lits[i];
  }
  for ( int i = 0; i < n2; ++ i ) {
    auto l{mCondLits[i]};
    // 極性が反転することに注意
    mTmpLits[lit_num + i] = ~l;
  }

  // 節を追加する本体
  add_clause_sub(lit_num + n2);
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
  stats.mLearntClauseNum = mLearntClauseList.size() + mLearntBinNum;
  stats.mLearntLitNum = mLearntLitNum;
  stats.mConflictNum = mConflictNum;
  stats.mDecisionNum = mDecisionNum;
  stats.mPropagationNum = mPropagationNum;
  stats.mConflictLimit = mConflictLimit;
  stats.mLearntLimit = mLearntLimit;
  stats.mTime = mAccTime;
}

// @brief 変数の数を得る．
int
YmSat::variable_num() const
{
  return mVarNum;
}

// @brief 制約節の数を得る．
int
YmSat::clause_num() const
{
  return mConstrClauseNum;
}

// @brief 制約節のリテラルの総数を得る．
int
YmSat::literal_num() const
{
  return mConstrLitNum;
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

// @brief add_clause() の下請け関数
void
YmSat::add_clause_sub(int lit_num)
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

  // 変数領域の確保を行う．
  alloc_var();

  // mTmpLits をソートする．
  // たぶん要素数が少ないので挿入ソートが速いはず．
  for ( int i = 1; i < lit_num; ++ i ) {
    // この時点で [0 : i - 1] までは整列している．
    auto l{mTmpLits[i]};
    if ( mTmpLits[i - 1].index() <= l.index() ) {
      // このままで [0 : i] まで整列していることになる．
      continue;
    }

    // l の挿入位置を探す．
    int j = i;
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
  int wpos = 0;
  for ( int rpos = 0; rpos < lit_num; ++ rpos ) {
    auto l{mTmpLits[rpos]};
    if ( wpos != 0 ) {
      auto l1{mTmpLits[wpos - 1]};
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

    auto v{eval(l)};
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

  auto l0{mTmpLits[0]};
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
      auto conflict{implication()};
      if ( conflict != kNullSatReason ) {
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
  for ( int i = 0; i < lit_num; ++ i ) {
    auto l{mTmpLits[i]};
    int index = l.index();
    mWeightArray[index] += 1.0 / static_cast<double>(lit_num);
  }
#endif

  ++ mConstrClauseNum;

  auto l1{mTmpLits[1]};

  if ( lit_num == 2 ) {
    // 二項節の watcher は相方のリテラルなので clause は使われない

    if ( debug & debug_assign ) {
      cout << "add_clause: " << l0 << " + " << l1 << endl;
    }

    mConstrBinList.push_back(BinClause(l0, l1));

    // watcher-list の設定
    add_watcher(~l0, SatReason(l1));
    add_watcher(~l1, SatReason(l0));
  }
  else {
    // 節の生成
    auto clause{new_clause(lit_num)};

    if ( debug & debug_assign ) {
      cout << "add_clause: " << *clause << endl;
    }

    mConstrClauseList.push_back(clause);

    // watcher-list の設定
    add_watcher(~l0, SatReason(clause));
    add_watcher(~l1, SatReason(clause));
  }
}

// 学習節を追加する．
// @param[in] learnt_lits 追加する節のもととなるリテラルのリスト
void
YmSat::add_learnt_clause(const vector<SatLiteral>& learnt_lits)
{
  // learnt_lits の 0 番目のリテラルに値の割り当てが生じる．
  // 残りのリテラルがその割り当ての原因となる．

  int n = learnt_lits.size();
  mLearntLitNum += n;

  if ( n == 0 ) {
    // empty clause があったら unsat
    mSane = false;
    return;
  }

  auto l0{learnt_lits[0]};
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
  auto l1{learnt_lits[1]};
  if ( n == 2 ) {
    // binary-clause の場合

    if ( debug & debug_assign ) {
      cout << "add_learnt_clause: "
	   << "(" << l0 << " + " << l1 << ")" << endl
	   << "\tassign " << l0 << " @" << decision_level()
	   << " from (" << l0 << " + " << l1 << ")" << endl;
    }

    // watcher-list の設定
    add_watcher(~l0, SatReason(l1));
    add_watcher(~l1, SatReason(l0));

    reason = SatReason(l1);

    ++ mLearntBinNum;
  }
  else {
    // 節の生成
    alloc_lits(n);
    for ( int i = 0; i < n; ++ i ) {
      mTmpLits[i] = learnt_lits[i];
    }
    auto clause{new_clause(n, true)};

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

    reason = SatReason(clause);

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

// @brief 新しい節を生成する．
// @param[in] lit_num リテラル数
// @param[in] learnt 学習節のとき true とするフラグ
// @param[in] lbd 学習節のときの literal block distance
// @note リテラルは mTmpLits に格納されている．
SatClause*
YmSat::new_clause(int lit_num,
		  bool learnt)
{
  int size = sizeof(SatClause) + sizeof(SatLiteral) * (lit_num - 1);

  auto p{new char[size]};
  auto clause{new (p) SatClause(lit_num, mTmpLits, learnt)};

  return clause;
}

// @brief 節を削除する．
// @param[in] clause 削除する節
void
YmSat::delete_clause(SatClause* clause)
{
  if ( debug & debug_assign ) {
    cout << " delete_clause: " << (*clause) << endl;
  }

  // watch list を更新
  del_watcher(~clause->wl0(), SatReason(clause));
  del_watcher(~clause->wl1(), SatReason(clause));

  if ( clause->is_learnt() ) {
    mLearntLitNum -= clause->lit_num();
  }
  else {
    mConstrLitNum -= clause->lit_num();
  }

  auto p{reinterpret_cast<char*>(clause)};
  delete [] p;
}

// @brief watcher を削除する．
// @param[in] watch_lit リテラル
// @param[in] reason 理由
void
YmSat::del_watcher(SatLiteral watch_lit,
		   SatReason reason)
{
  // watch_lit に関係する watcher リストから
  // reason を探して削除する．
  // watcher リストを配列で実装しているので
  // あたまからスキャンして該当の要素以降を
  // 1つづつ前に詰める．
  Watcher w0{reason};
  auto& wlist{watcher_list(watch_lit)};
  int n = wlist.num();
  int wpos = 0;
  for ( ; wpos < n; ++ wpos) {
    auto w{wlist.elem(wpos)};
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
YmSat::del_satisfied_watcher(SatLiteral watch_lit)
{
  // watch_lit に関係する watcher リストをスキャンして
  // literal watcher が充足していたらその watcher を削除する．
  // watcher リストを配列で実装しているので
  // あたまからスキャンして該当の要素以降を
  // 1つづつ前に詰める．
  auto& wlist{watcher_list(watch_lit)};
  int n = wlist.num();
  int wpos = 0;
  for ( int rpos = 0; rpos < n; ++ rpos ) {
    auto w{wlist.elem(rpos)};
    if ( w.is_literal() ) {
      auto l{w.literal()};
      auto val{eval(l)};
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
    for ( int i = mOldVarNum; i < mVarNum; ++ i ) {
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
  int old_size = mVarSize;
  ymuint8* old_val = mVal;
  int* old_decision_level = mDecisionLevel;
  SatReason* old_reason = mReason;
  WatcherList* old_watcher_list = mWatcherList;
#if YMSAT_USE_WEIGHTARRAY
  double* old_weight_array = mWeightArray;
#endif
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
#if YMSAT_USE_WEIGHTAARAY
  mWeightArray = new double[mVarSize * 2];
#endif

  for ( int i = 0; i < mOldVarNum; ++ i ) {
    mVal[i] = old_val[i];
    mDecisionLevel[i] = old_decision_level[i];
    mReason[i] = old_reason[i];
  }
  int n2 = mOldVarNum * 2;
  for ( int i = 0; i < n2; ++ i ) {
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

BEGIN_NONAMESPACE

// DIMACS 形式でリテラルを出力する．
void
write_lit(ostream& s,
	  SatLiteral lit)
{
  auto var = lit.varid();
  int idx = var + 1;
  if ( lit.is_negative() ) {
    s << " -";
      }
  else {
    s << " ";
  }
  s << idx;
}

END_NONAMESPACE

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
YmSat::write_DIMACS(ostream& s) const
{
  s << "p cnf " << variable_num() << " " << clause_num() << endl;
  for ( int i = 0; i < mConstrBinList.size(); ++ i ) {
    auto& clause{mConstrBinList[i]};
    write_lit(s, clause.mLit0);
    write_lit(s, clause.mLit1);
  }
  for ( int i = 0; i < mConstrClauseList.size(); ++ i ) {
    auto clause{mConstrClauseList[i]};
    int nl = clause->lit_num();
    for ( int j = 0; j < nl; ++ j ) {
      auto lit{clause->lit(j)};
      write_lit(s, lit);
    }
    s << " 0" << endl;
  }
}

END_NAMESPACE_YM_YMSATOLD
