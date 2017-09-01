#ifndef YMSAT1_YMSAT_H
#define YMSAT1_YMSAT_H

/// @file YmSat.h
/// @brief YmSat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym_sat1.h"
#include "SatSolverImpl.h"
#include "ym/FragAlloc.h"
#include "ym/RandGen.h"
#include "ym/StopWatch.h"
#include "ymsat1/SatClause.h"
#include "ymsat1/SatReason.h"
#include "ymsat1/AssignList.h"
#include "ymsat1/Watcher.h"
#include "ymsat1/VarHeap.h"
#include "conf.h"


BEGIN_NAMESPACE_YM_SAT1

class SatAnalyzer;

//////////////////////////////////////////////////////////////////////
/// @class YmSat YmSat.h "YmSat.h"
/// @brief SatSolver の実装クラス
//////////////////////////////////////////////////////////////////////
class YmSat :
  public nsSat::SatSolverImpl
{
  friend class SatAnalyzer;

public:
  //////////////////////////////////////////////////////////////////////
  /// @class YmSat::Params YmSat.h "YmSat.h"
  /// @brief YmSat の挙動を制御するパラメータ
  //////////////////////////////////////////////////////////////////////
  struct Params
  {
    /// @brief 変数の decay 値
    double mVarDecay;

    /// @brief 学習節の decay 値
    double mClauseDecay;

#if YMSAT_USE_LBD
    /// @brief LBD ヒューリスティックを使うとき true
    bool mUseLbd;

    /// @brief コンストラクタ
    Params() :
      mVarDecay(1.0),
      mClauseDecay(1.0),
      mUseLbd(false)
    {
    }

    /// @brief 値を指定したコンストラクタ
    Params(double var_decay,
	   double clause_decay,
	   bool use_lbd) :
      mVarDecay(var_decay),
      mClauseDecay(clause_decay),
      mUseLbd(use_lbd)
    {
    }
#else

    /// @brief コンストラクタ
    Params() :
      mVarDecay(1.0),
      mClauseDecay(1.0)
    {
    }

    /// @brief 値を指定したコンストラクタ
    Params(double var_decay,
	   double clause_decay) :
      mVarDecay(var_decay),
      mClauseDecay(clause_decay)
    {
    }
#endif
  };


public:

  /// @brief コンストラクタ
  /// @param[in] option オプション文字列
  YmSat(const string& option = string());

  /// @brief デストラクタ
  virtual
  ~YmSat();


public:
  //////////////////////////////////////////////////////////////////////
  // SatSolver で定義されている仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  virtual
  bool
  sane() const;

  /// @brief 変数を追加する．
  /// @param[in] decision 決定変数の時に true とする．
  /// @return 新しい変数番号を返す．
  /// @note 変数番号は 0 から始まる．
  virtual
  SatVarId
  new_variable(bool decision);

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  virtual
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 節を追加する．
  /// @param[in] lit_num リテラル数
  /// @param[in] lits リテラルの配列
  virtual
  void
  add_clause(ymuint lit_num,
	     const SatLiteral* lits);

  /// @brief SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
  /// @retval kB3True 充足した．
  /// @retval kB3False 充足不能が判明した．
  /// @retval kB3X わからなかった．
  /// @note i 番めの変数の割り当て結果は model[i] に入る．
  virtual
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	vector<SatBool3>& model,
	vector<SatLiteral>& conflicts);

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  virtual
  void
  stop();

  /// @brief 学習節をすべて削除する．
  virtual
  void
  forget_learnt_clause();

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  virtual
  void
  get_stats(SatStats& stats) const;

  /// @brief 変数の数を得る．
  virtual
  ymuint
  variable_num() const;

  /// @brief 制約節の数を得る．
  virtual
  ymuint
  clause_num() const;

  /// @brief 制約節のリテラルの総数を得る．
  virtual
  ymuint
  literal_num() const;

  /// @brief DIMACS 形式で制約節を出力する．
  /// @param[in] s 出力先のストリーム
  virtual
  void
  write_DIMACS(ostream& s) const;

  /// @brief conflict_limit の最大値
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  virtual
  ymuint64
  set_max_conflict(ymuint64 val);

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  /// @param[in] msg_handler 登録するメッセージハンドラ
  virtual
  void
  reg_msg_handler(SatMsgHandler* msg_handler);

  /// @brief 時間計測機能を制御する
  virtual
  void
  timer_on(bool enable);


private:
  //////////////////////////////////////////////////////////////////////
  // 実装用のプライベート関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 探索を行う本体の関数
  /// @retval kB3True 充足した．
  /// @retval kB3False 充足できないことがわかった．
  /// @retval kB3X 矛盾の生起回数が mConflictLimit を超えた．
  ///
  /// 矛盾の結果新たな学習節が追加される場合もあるし，
  /// 内部で reduce_learnt_clause() を呼んでいるので学習節が
  /// 削減される場合もある．
  SatBool3
  search();

  /// @brief 割当てキューに基づいて implication を行う．
  /// @return 矛盾が生じたら矛盾の原因を返す．
  ///
  /// 矛盾が生じていなかったら kNullSatReason を返す．
  /// 矛盾の原因とは実際には充足していない節である．
  SatReason
  implication();

  /// @brief level までバックトラックする
  /// @param[in] level バックトラックするレベル
  void
  backtrack(int level);

  /// @brief 次の割り当てを選ぶ．
  /// @note 割り当てられる変数がない場合には kSatLiteralX を返す．
  SatLiteral
  next_decision();

  /// @brief 値の割当てか可能かチェックする．
  /// @param[in] lit 割り当てるリテラル
  /// @return 矛盾が起きたら false を返す．
  ///
  /// すでに同じ値が割り当てられていたらなにもしない．
  /// 割り当てには assign() を呼び出す．
  bool
  check_and_assign(SatLiteral lit);

  /// @brief 値の割当てを行う．
  /// @param[in] lit 割り当てるリテラル
  /// @param[in] reason 割り当ての理由
  void
  assign(SatLiteral lit,
	 SatReason reason = SatReason());

  /// @brief CNF を簡単化する．
  ///
  /// 具体的には implication() を行って充足している節を取り除く．
  /// もちろん decision_level が 0 の時しか実行できない．
  void
  reduce_CNF();

  /// @brief 学習節の整理を行なう．
  void
  reduce_learnt_clause();

  /// @brief 使われていない学習節を削除する．
  void
  reduceDB();

  /// @brief 学習節を追加する．
  /// @note 追加するリテラルは mLearntLits に入れる．
  void
  add_learnt_clause();

  /// @brief add_clause() の下請け関数
  ///
  /// リテラルの実体は mTmpLits[] に入っている．
  void
  add_clause_sub(ymuint lit_num);

  /// @brief 新しい節を生成する．
  /// @param[in] lit_num リテラル数
  /// @param[in] learnt 学習節のとき true とするフラグ
  /// @note リテラルは mTmpLits に格納されている．
  SatClause*
  new_clause(ymuint lit_num,
	     bool learnt = false);

  /// @brief mTmpLits を確保する．
  void
  alloc_lits(ymuint lit_num);

  /// @brief 節を削除する．
  /// @param[in] clause 削除する節
  void
  delete_clause(SatClause* clause);

  /// @brief watcher list を得る．
  /// @param[in] lit リテラル
  WatcherList&
  watcher_list(SatLiteral lit);

  /// @brief Watcher を追加する．
  /// @param[in] watch_lit リテラル
  /// @param[in] reason 理由
  void
  add_watcher(SatLiteral watch_lit,
	      SatReason reason);

  /// @brief watcher を削除する．
  /// @param[in] watch_lit リテラル
  /// @param[in] reason 理由
  void
  del_watcher(SatLiteral watch_lit,
	      SatReason reason);

  /// @brief 変数1の評価を行う．
  /// @param[in] id 変数番号
  SatBool3
  eval(SatVarId id) const;

  /// @brief literal の評価を行う．
  /// @param[in] l リテラル
  SatBool3
  eval(SatLiteral l) const;

  /// @brief 現在の decision level を返す．
  int
  decision_level() const;

  /// @brief 変数の decision level を返す．
  /// @param[in] varid 変数番号
  int
  decision_level(SatVarId varid) const;

#if YMSAT_USE_LBD
  /// @brief LBD を計算する．
  /// @param[in] clause 対象の節
  ymuint
  calc_lbd(const SatClause* clause);
#endif

  /// @brief 変数の割り当て理由を返す．
  /// @param[in] varid 変数番号
  SatReason
  reason(SatVarId varid) const;

  /// @brief 学習節が使われているか調べる．
  /// @param[in] clause 対象の節
  bool
  is_locked(SatClause* clause) const;

  /// @brief 変数のアクティビティを増加させる．
  /// @param[in] var 変数番号
  void
  bump_var_activity(SatVarId var);

  /// @brief 変数のアクティビティを定率で減少させる．
  void
  decay_var_activity();

  /// @brief 学習節のアクティビティを増加させる．
  /// @param[in] clause 対象の学習節
  void
  bump_clause_activity(SatClause* clause);

  /// @brief 学習節のアクティビティを定率で減少させる．
  void
  decay_clause_activity();

  /// @brief 実際に変数に関するデータ構造を生成する．
  void
  alloc_var();

  /// @brief 変数に関する配列を拡張する．
  void
  expand_var();

  static
  SatBool3
  conv_to_Bool3(ymuint8 x);

  static
  ymuint8
  conv_from_Bool3(SatBool3 b);

  static
  SatBool3
  cur_val(ymuint8 x);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 解析器
  SatAnalyzer* mAnalyzer;

  // 正常の時に true となっているフラグ
  bool mSane;

  // SatClause のメモリ領域確保用のアロケータ
  FragAlloc mAlloc;

  // 制約節の配列
  vector<SatClause*> mConstrClauseList;

  // 二項制約節の数
  ymuint64 mConstrBinNum;

  // 制約節の総リテラル数 (二項制約節も含む)
  ymuint64 mConstrLitNum;

  // 学習節の配列
  vector<SatClause*> mLearntClause;

  // 二項学習節の数
  ymuint64 mLearntBinNum;

  // 学習節の総リテラル数 (二項制約節も含む)
  ymuint64 mLearntLitNum;

#if YMSAT_USE_DVAR
  // dvar 配列
  vector<bool> mDvarArray;
#endif

  // 変数の数
  ymuint32 mVarNum;

  // 前回の alloc_var で処理した時の変数の数
  ymuint32 mOldVarNum;

  // 変数関係の配列のサイズ
  ymuint32 mVarSize;

  // 値の配列
  // サイズは mVarSize
  ymuint8* mVal;

  // 値が割り当てられたときのレベルの配列
  // サイズは mVarSize
  int* mDecisionLevel;

  // 値が割り当てられた理由の配列
  // サイズは mVarSize
  SatReason* mReason;

  // watcher list の配列
  // サイズは mVarSize * 2
  WatcherList* mWatcherList;

#if YMSAT_USE_WEIGHTARRAY
  // 変数の極性ごとの重み
  // サイズは mVarSize * 2
  double* mWeightArray;
#endif

  // 変数ヒープ
  VarHeap mVarHeap;

#if YMSAT_USE_LBD
  // calc_lbd() 用の作業領域
  // サイズは decision_level() + 1
  bool* mLbdTmp;

  // mLbdTmp のサイズ．
  ymuint32 mLbdTmpSize;
#endif

  // 矛盾の解析時にテンポラリに使用される節
  SatClause* mTmpBinClause;

  // search 開始時の decision level
  int mRootLevel;

  // 値割り当てを保持するリスト
  AssignList mAssignList;

  // 学習節のアクティビティの増加量
  double mClauseBump;

  // 学習節のアクティビティの減衰量
  double mClauseDecay;

  // 変数選択用乱数発生器
  RandGen mRandGen;

  // mTimer を使うとき true にするフラグ
  bool mTimerOn;

  // 時間計測器
  StopWatch mTimer;

  // 制御用パラメータ
  Params mParams;

  // restart 数
  ymuint mRestart;

  // 総コンフリクト数
  ymuint64 mConflictNum;

  // 総 decision 数
  ymuint64 mDecisionNum;

  // 総 implication 数
  ymuint64 mPropagationNum;

  // コンフリクト数の制限
  ymuint64 mConflictLimit;

  // 学習節の制限
  ymuint64 mLearntLimit;

  // トータルのコンフリクト数の制限
  ymuint64 mMaxConflict;

  // stop() が用いるフラグ
  bool mGoOn;

  // メッセージハンドラのリスト
  list<SatMsgHandler*> mMsgHandlerList;

  // add_clause で一時的に利用するリテラル配列
  SatLiteral* mTmpLits;

  // mTmpLits のサイズ
  ymuint32 mTmpLitsSize;

  // search() で用いられるリテラル配列
  vector<SatLiteral> mLearntLits;


protected:
  //////////////////////////////////////////////////////////////////////
  // デバッグ用の定数，変数
  //////////////////////////////////////////////////////////////////////

  static
  const ymuint debug_none        = 0x00;

  static
  const ymuint debug_implication = 0x01;

  static
  const ymuint debug_analyze     = 0x02;

  static
  const ymuint debug_assign      = 0x04;

  static
  const ymuint debug_decision    = 0x08;

  static
  const ymuint debug_solve       = 0x10;

  static
  const ymuint debug_all         = 0xffffffff;

  static
  //const ymuint debug = debug_decision | debug_analyze | debug_assign;
  //const ymuint debug = debug_assign;
  //const ymuint debug = debug_assign | debug_implication;
  //const ymuint debug = debug_assign | debug_analyze;
  //const ymuint debug = debug_solve | debug_decision;
  //const ymuint debug = debug_solve | debug_assign;
  //const ymuint debug = debug_all;
  const ymuint debug = debug_none;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 変数の数を得る．
inline
ymuint
YmSat::variable_num() const
{
  return mVarNum;
}

// @brief 制約節の数を得る．
inline
ymuint
YmSat::clause_num() const
{
  return mConstrClauseList.size() + mConstrBinNum;
}

// @brief 制約節のリテラルの総数を得る．
inline
ymuint
YmSat::literal_num() const
{
  return mConstrLitNum;
}

// watcher list を得る．
inline
WatcherList&
YmSat::watcher_list(SatLiteral lit)
{
  ymuint index = lit.index();
  return mWatcherList[index];
}

// Watcher を追加する．
inline
void
YmSat::add_watcher(SatLiteral watch_lit,
		   SatReason reason)
{
  watcher_list(watch_lit).add(Watcher(reason));
}

inline
SatBool3
YmSat::conv_to_Bool3(ymuint8 x)
{
  int tmp = static_cast<int>(x) - 1;
  return static_cast<SatBool3>(tmp);
}

inline
ymuint8
YmSat::conv_from_Bool3(SatBool3 b)
{
  int tmp = static_cast<int>(b) + 1;
  return static_cast<ymuint8>(tmp);
}

inline
SatBool3
YmSat::cur_val(ymuint8 x)
{
  return conv_to_Bool3(x & 3U);
}

// 変数の評価を行う．
inline
SatBool3
YmSat::eval(SatVarId id) const
{
  ASSERT_COND( id.val() < mVarNum );
  return cur_val(mVal[id.val()]);
}

// literal の評価を行う．
inline
SatBool3
YmSat::eval(SatLiteral l) const
{
  ymuint index = l.index();
  ASSERT_COND( (index / 2) < mVarNum );
  ymuint x = mVal[index / 2] & 3U;
  ymuint inv = index & 1U;
  int d = 1 - (inv * 2);
  return static_cast<SatBool3>((static_cast<int>(x) - 1) * d);
}

// 値の割当てか可能かチェックする．
// 矛盾が起きたら false を返す．
inline
bool
YmSat::check_and_assign(SatLiteral lit)
{
  SatBool3 old_val = eval(lit);
  if ( old_val != kB3X ) {
    return old_val == kB3True;
  }
  assign(lit);
  return true;
}

// 値の割当てを行う．
inline
void
YmSat::assign(SatLiteral lit,
	      SatReason reason)
{
  ymuint lindex = lit.index();
  ymuint vindex = lindex / 2;
  ASSERT_COND( vindex < mVarNum );
  ymuint inv = lindex & 1U;
  ymuint8 x = 2 - inv * 2;
  mVal[vindex] = x | ((conv_from_Bool3(kB3X)) << 2);
  mDecisionLevel[vindex] = decision_level();
  mReason[vindex] = reason;

  // mAssignList に格納する．
  mAssignList.put(lit);
}

// 現在の decision level を返す．
inline
int
YmSat::decision_level() const
{
  return mAssignList.cur_level();
}

// 変数の decision level を返す．
inline
int
YmSat::decision_level(SatVarId varid) const
{
  ASSERT_COND( varid.val() < mVarNum );
  return mDecisionLevel[varid.val()];
}

// 変数の割り当て理由を返す．
inline
SatReason
YmSat::reason(SatVarId varid) const
{
  ASSERT_COND( varid.val() < mVarNum );
  return mReason[varid.val()];
}

// @brief clase が含意の理由になっているか調べる．
inline
bool
YmSat::is_locked(SatClause* clause) const
{
  // 直感的には分かりにくいが，節の最初のリテラルは
  // 残りのリテラルによって含意されていることになっている．
  // そこで最初のリテラルの変数の割り当て理由が自分自身か
  // どうかを調べれば clause が割り当て理由として用いられて
  // いるかわかる．
  return reason(clause->wl0().varid()) == SatReason(clause);
}

// 実際に変数に関するデータ構造を生成する．
inline
void
YmSat::alloc_var()
{
  if ( mOldVarNum < mVarNum ) {
    if ( mVarSize < mVarNum ) {
      expand_var();
    }
    for (ymuint i = mOldVarNum; i < mVarNum; ++ i) {
      mVal[i] = conv_from_Bool3(kB3X);
      mVarHeap.add_var(SatVarId(i));
    }
    mOldVarNum = mVarNum;
  }
}

// 変数のアクティビティを増加させる．
inline
void
YmSat::bump_var_activity(SatVarId var)
{
  mVarHeap.bump_var_activity(var);
}

// 変数のアクティビティを定率で減少させる．
inline
void
YmSat::decay_var_activity()
{
  mVarHeap.decay_var_activity();
}

// 学習節のアクティビティを定率で減少させる．
inline
void
YmSat::decay_clause_activity()
{
  mClauseBump /= mClauseDecay;
}

END_NAMESPACE_YM_SAT1

#endif // YMSAT1_YMSAT_H
