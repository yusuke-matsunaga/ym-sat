#ifndef YMSATOLD_YMSAT_H
#define YMSATOLD_YMSAT_H

/// @file YmSat.h
/// @brief YmSat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatSolverImpl.h"
#include "ymsat_old.h"
#include "Clause.h"
#include "Reason.h"
#include "AssignList.h"
#include "Watcher.h"
#include "VarHeap.h"
#include "conf.h"
#include <chrono>
#include <random>


BEGIN_NAMESPACE_YM_YMSATOLD

class Analyzer;

//////////////////////////////////////////////////////////////////////
/// @class YmSat YmSat.h "YmSat.h"
/// @brief SatSolver の実装クラス
//////////////////////////////////////////////////////////////////////
class YmSat :
  public nsSat::SatSolverImpl
{
  friend class Analyzer;

public:
  //////////////////////////////////////////////////////////////////////
  /// @class YmSat::Params YmSat.h "YmSat.h"
  /// @brief YmSat の挙動を制御するパラメータ
  //////////////////////////////////////////////////////////////////////
  struct Params
  {
    /// @brief 変数の decay 値
    double mVarDecay{1.0};

    /// @brief 学習節の decay 値
    double mClauseDecay{1.0};

#if YMSAT_USE_LBD
    /// @brief LBD ヒューリスティックを使うとき true
    bool mUseLbd{false};
#endif

    /// @brief コンストラクタ
    Params() = default;

#if YMSAT_USE_LBD
    /// @brief 値を指定したコンストラクタ
    Params(
      double var_decay,
      double clause_decay,
      bool use_lbd
    ) : mVarDecay{var_decay},
	mClauseDecay{clause_decay},
	mUseLbd{use_lbd}
    {
    }
#else
    /// @brief 値を指定したコンストラクタ
    Params(
      double var_decay,
      double clause_decay
    ) : mVarDecay{var_decay},
	mClauseDecay{clause_decay}
    {
    }
#endif
  };


public:

  /// @brief コンストラクタ
  YmSat(
    const string& option = string{} ///< [in] オプション文字列
  );

  /// @brief デストラクタ
  ~YmSat();


public:
  //////////////////////////////////////////////////////////////////////
  // SatSolver で定義されている仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const override;

  /// @brief 変数を追加する．
  /// @return 新しい変数番号を返す．
  int
  new_variable(
    bool decision ///< [in] 決定変数の時に true とする．
  ) override;

  /// @brief リテラルを 'フリーズ' する．
  ///
  /// lingeling 以外は無効
  void
  freeze_literal(
    SatLiteral lit
  ) override;

  /// @brief 節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] リテラルのベクタ
  ) override;

  /// @brief SAT 問題を解く．
  /// @retval SatBool3::True 充足した．
  /// @retval SatBool3::False 充足不能が判明した．
  /// @retval SatBool3::X わからなかった．
  SatBool3
  solve(
    const vector<SatLiteral>& assumptions, ///< [in] あらかじめ仮定する変数の値割り当てリスト
    SatModel& model,                       ///< [out] 充足するときの値の割り当てを格納する配列．
    vector<SatLiteral>& conflicts          ///< [out] 充足不能の場合に原因となっている仮定を入れる配列．
  ) override;

  /// @brief 探索を中止する．
  ///
  /// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
  void
  stop() override;

  /// @brief 学習節をすべて削除する．
  void
  forget_learnt_clause();

  /// @brief 現在の内部状態を得る．
  SatStats
  get_stats() const override;

  /// @brief conflict_limit の最大値
  /// @return 以前の設定値を返す．
  SizeType
  set_max_conflict(
    SizeType val ///< [in] 設定する値
  ) override;

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  void
  reg_msg_handler(
    SatMsgHandler* msg_handler ///< [in] 登録するメッセージハンドラ
  ) override;

  /// @brief 時間計測機能を制御する
  void
  timer_on(
    bool enable
  ) override;


private:
  //////////////////////////////////////////////////////////////////////
  // 実装用のプライベート関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 探索を行う本体の関数
  /// @retval SatBool3::True 充足した．
  /// @retval SatBool3::False 充足できないことがわかった．
  /// @retval SatBool3::X 矛盾の生起回数が mConflictLimit を超えた．
  ///
  /// 矛盾の結果新たな学習節が追加される場合もあるし，
  /// 内部で reduce_learnt_clause() を呼んでいるので学習節が
  /// 削減される場合もある．
  SatBool3
  search();

  /// @brief search() 用の条件パラメータの初期化を行う．
  virtual
  void
  init_control_parameters() = 0;

  /// @brief リスタート時に制御パラメータの更新を行う．
  /// @param[in] restart リスタート回数
  virtual
  void
  update_on_restart(
    SizeType restart
  ) = 0;

  /// @brief コンフリクト時に制御パラメータの更新を行う．
  virtual
  void
  update_on_conflict() = 0;

  /// @brief 次の割り当てを選ぶ．
  /// @note 割り当てられる変数がない場合には Literal::X を返す．
  virtual
  Literal
  next_decision() = 0;

  /// @brief 学習節の整理を行なう．
  ///
  /// 実際には条件が成り立った時のみ整理する．
  virtual
  void
  reduce_learnt_clause() = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 継承クラスから用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 矛盾回数の制限値を設定する．
  /// @param[in] limit 設定する値
  void
  set_conflict_limit(
    SizeType limit
  )
  {
    mConflictLimit = limit;
  }

  /// @brief 学習節の制限値を設定する．
  /// @param[in] limit 設定する値
  void
  set_learnt_limit(
    SizeType limit
  )
  {
    mLearntLimit = limit;
  }

  /// @brief 変数ヒープを得る．
  VarHeap&
  var_heap()
  {
    return mVarHeap;
  }

  /// @brief 変数の数を得る．
  SizeType
  variable_num()
  {
    return mVarNum;
  }

  /// @brief 制約節数を得る．
  SizeType
  clause_num()
  {
    return mConstrClauseNum;
  }

  /// @brief 学習節のリストを返す．
  vector<Clause*>&
  learnt_clause_list()
  {
    return mLearntClauseList;
  }

  /// @brief 節を削除する．
  /// @param[in] clause 削除する節
  void
  delete_clause(
    Clause* clause
  );

  /// @brief 学習節のアクティビティ増加量を返す．
  double
  clause_bump() const
  {
    return mClauseBump;
  }

  /// @brief watcher list を得る．
  /// @param[in] lit リテラル
  WatcherList&
  watcher_list(
    Literal lit
  )
  {
    auto index = lit.index();
    ASSERT_COND( index < (mVarNum * 2) );
    return mWatcherList[index];
  }

  /// @brief 変数1の評価を行う．
  /// @param[in] id 変数番号
  SatBool3
  eval(
    int id
  ) const
  {
    ASSERT_COND( id >= 0 && id < mVarNum );

    return cur_val(mVal[id]);
  }

  /// @brief literal の評価を行う．
  /// @param[in] l リテラル
  SatBool3
  eval(
    Literal l
  ) const
  {
    auto index = l.index();
    ASSERT_COND( (index / 2) < mVarNum );
    int x = mVal[index / 2] & 3U;
    int inv = index & 1U;
    int d = 1 - (inv * 2);
    return static_cast<SatBool3>((static_cast<int>(x) - 1) * d);
  }

  /// @brief 変数の直前の値を返す．
  SatBool3
  old_val(
    int id
  ) const
  {
    ASSERT_COND( id >= 0 && id < mVarNum );

    return conv_to_Bool3((mVal[id] >> 2) & 3U);
  }

  /// @brief 現在の decision level を返す．
  int
  decision_level() const
  {
    return mAssignList.cur_level();
  }

  /// @brief 変数の decision level を返す．
  /// @param[in] varid 変数番号
  int
  decision_level(
    int varid
  ) const
  {
    ASSERT_COND( varid >= 0 && varid < mVarNum );

    return mDecisionLevel[varid];
  }

#if YMSAT_USE_LBD
  /// @brief LBD を計算する．
  /// @param[in] clause 対象の節
  int
  calc_lbd(
    const Clause* clause
  );
#endif

  /// @brief 変数の割り当て理由を返す．
  /// @param[in] varid 変数番号
  Reason
  reason(
    int varid
  ) const
  {
    ASSERT_COND( varid >= 0 && varid < mVarNum );

    return mReason[varid];
  }

  /// @brief 学習節が使われているか調べる．
  /// @param[in] clause 対象の節
  bool
  is_locked(
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


private:
  //////////////////////////////////////////////////////////////////////
  // 低レベルの下請け関数
  //
  // 実装により実行効率に差は出るが，結果には影響がないもの
  //////////////////////////////////////////////////////////////////////

  /// @brief 割当てキューに基づいて implication を行う．
  /// @return 矛盾が生じたら矛盾の原因を返す．
  ///
  /// 矛盾が生じていなかったら Reason::None を返す．
  /// 矛盾の原因とは実際には充足していない節である．
  Reason
  implication();

  /// @brief バックトラックする
  /// @param[in] level バックトラックするレベル
  void
  backtrack(
    int level
  );

  /// @brief 値の割当てか可能かチェックする．
  /// @param[in] lit 割り当てるリテラル
  /// @return 矛盾が起きたら false を返す．
  ///
  /// すでに同じ値が割り当てられていたらなにもしない．
  /// 割り当てには assign() を呼び出す．
  bool
  check_and_assign(
    Literal lit
  )
  {
    auto old_val = eval(lit);
    if ( old_val != SatBool3::X ) {
      return old_val == SatBool3::True;
    }
    assign(lit);
    return true;
  }

  /// @brief 値の割当てを行う．
  /// @param[in] lit 割り当てるリテラル
  /// @param[in] reason 割り当ての理由
  void
  assign(
    Literal lit,
    Reason reason = Reason::None
  )
  {
    auto lindex = lit.index();
    int vindex = lindex / 2;
    int inv = lindex & 1U;
    ymuint8 x = 2 - inv * 2;
    ASSERT_COND( vindex < mVarNum );
    mVal[vindex] = x;
    mDecisionLevel[vindex] = decision_level();
    mReason[vindex] = reason;

    // mAssignList に格納する．
    mAssignList.put(lit);
  }

  /// @brief CNF を簡単化する．
  ///
  /// 具体的には implication() を行って充足している節を取り除く．
  /// もちろん decision_level が 0 の時しか実行できない．
  void
  reduce_CNF();

  /// @brief 充足している節を取り除く
  /// @param[in] clause_list 節のリスト
  ///
  /// reduce_CNF() の中で用いられる．
  void
  sweep_clause(
    vector<Clause*>& clause_list
  );

  /// @brief 学習節を追加する．
  /// @param[in] learnt_lits 追加する節のもととなるリテラルのリスト
  void
  add_learnt_clause(
    const vector<Literal>& learnt_lits
  );

  /// @brief mTmpLits を確保する．
  /// @param[in] lit_num リテラル数
  void
  alloc_lits(
    SizeType lit_num
  );

  /// @brief 新しい節を生成する．
  /// @param[in] lit_num リテラル数
  /// @param[in] learnt 学習節のとき true とするフラグ
  /// @param[in] lbd 学習節のときの literal block distance
  /// @note リテラルは mTmpLits に格納されている．
  Clause*
  new_clause(
    SizeType lit_num,
    bool learnt = false
  );

  /// @brief Watcher を追加する．
  /// @param[in] watch_lit リテラル
  /// @param[in] reason 理由
  void
  add_watcher(
    Literal watch_lit,
    Reason reason
  )
  {
    watcher_list(watch_lit).add(Watcher{reason});
  }

  /// @brief watcher を削除する．
  /// @param[in] watch_lit リテラル
  /// @param[in] reason 理由
  void
  del_watcher(
    Literal watch_lit,
    Reason reason
  );

  /// @brief 充足された watcher を削除する．
  /// @param[in] watch_lit リテラル
  void
  del_satisfied_watcher(
    Literal watch_lit
  );

  /// @brief 変数のアクティビティを増加させる．
  /// @param[in] var 変数番号
  void
  bump_var_activity(
    int var
  )
  {
    mVarHeap.bump_var_activity(var);
  }

  /// @brief 変数のアクティビティを定率で減少させる．
  void
  decay_var_activity()
  {
    mVarHeap.decay_var_activity();
  }

  /// @brief 学習節のアクティビティを増加させる．
  /// @param[in] clause 対象の学習節
  void
  bump_clause_activity(
    Clause* clause
  );

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
  conv_to_Bool3(
    ymuint8 x
  )
  {
    int tmp = static_cast<int>(x) - 1;
    return static_cast<SatBool3>(tmp);
  }

  static
  SatBool3
  cur_val(
    ymuint8 x
  )
  {
    return conv_to_Bool3(x & 3U);
  }

  static
  ymuint8
  conv_from_Bool3(
    SatBool3 b
  )
  {
    int tmp = static_cast<int>(b) + 1;
    return static_cast<ymuint8>(tmp);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 二項節を表す構造体
  struct BinClause
  {
    /// @brief 空のコンストラクタ
    BinClause() = default;

    /// @brief コンストラクタ
    BinClause(
      Literal l0,
      Literal l1
    ) : mLit0{l0},
	mLit1{l1}
    {
    }

    Literal mLit0;
    Literal mLit1;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 解析器
  Analyzer* mAnalyzer;

  // 正常の時に true となっているフラグ
  bool mSane{true};

  // 制約節のリスト
  // ただし二項節は含まない．
  vector<Clause*> mConstrClauseList;

  // 二項節を表すリテラルのリスト
  vector<BinClause> mConstrBinList;

  // 制約節の数(二項節も含む)
  SizeType mConstrClauseNum{0};

  // 制約節の総リテラル数 (二項制約節も含む)
  SizeType mConstrLitNum{0};

  // 学習節のリスト
  vector<Clause*> mLearntClauseList;

  // 二項学習節の数
  SizeType mLearntBinNum{0};

  // 学習節の総リテラル数 (二項制約節も含む)
  SizeType mLearntLitNum{0};

#if YMSAT_USE_DVAR
  // dvar 配列
  vector<bool> mDvarArray;
#endif

  // 変数の数
  SizeType mVarNum{0};

  // 前回の alloc_var で処理した時の変数の数
  SizeType mOldVarNum{0};

  // 変数関係の配列のサイズ
  SizeType mVarSize{0};

  // 値の配列
  // サイズは mVarSize
  ymuint8* mVal{nullptr};

  // 値が割り当てられたときのレベルの配列
  // サイズは mVarSize
  int* mDecisionLevel{nullptr};

  // 値が割り当てられた理由の配列
  // サイズは mVarSize
  Reason* mReason{nullptr};

  // watcher list の配列
  // サイズは mVarSize * 2
  WatcherList* mWatcherList{nullptr};

#if YMSAT_USE_WEIGHTARRAY
  // 変数の極性ごとの重み
  // サイズは mVarSize * 2
  double* mWeightArray{nullptr};
#endif

  // 変数のヒープ木
  VarHeap mVarHeap;

#if YMSAT_USE_LBD
  // calc_lbd() 用の作業領域
  // サイズは decision_level() + 1
  bool* mLbdTmp{nullptr};

  // mLbdTmp のサイズ．
  SizeType mLbdTmpSize{0};
#endif

  // 矛盾の解析時にテンポラリに使用される節
  Clause* mTmpBinClause{nullptr};

  // search 開始時の decision level
  int mRootLevel{0};

  // 値割り当てを保持するリスト
  AssignList mAssignList;

  // 前回の sweep 時の割り当て数
  int mSweep_assigns;

  // 前回の sweep 時のリテラル数
  int mSweep_props;

  // 学習節のアクティビティの増加量
  double mClauseBump;

  // 学習節のアクティビティの減衰量
  double mClauseDecay;

  // mTimer を使うとき true にするフラグ
  bool mTimerOn{false};

  using Clock = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<Clock>;
  using Duration = std::chrono::milliseconds;

  // 開始時刻
  TimePoint mStartTime;

  // 累積時間
  Duration mAccTime;

  // 制御用パラメータ
  Params mParams;

  // restart 数
  SizeType mRestart{0};

  // 総コンフリクト数
  SizeType mConflictNum{0};

  // 総 decision 数
  SizeType mDecisionNum{0};

  // 総 implication 数
  SizeType mPropagationNum{0};

  // コンフリクト数の制限
  SizeType mConflictLimit{0};

  // 学習節の制限
  SizeType mLearntLimit{0};

  // トータルのコンフリクト数の制限
  SizeType mMaxConflict{0};

  // stop() が用いるフラグ
  bool mGoOn{false};

  // メッセージハンドラのリスト
  vector<SatMsgHandler*> mMsgHandlerList;

  // add_clause で一時的に利用するリテラル配列
  Literal* mTmpLits{nullptr};

  // mTmpLits のサイズ
  SizeType mTmpLitsSize{0};


private:
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

#if 0
//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 矛盾回数の制限値を設定する．
// @param[in] limit 設定する値
inline
void
YmSat::set_conflict_limit(int limit)
{
  mConflictLimit = limit;
}

// @brief 学習節の制限値を設定する．
// @param[in] limit 設定する値
inline
void
YmSat::set_learnt_limit(int limit)
{
  mLearntLimit = limit;
}

// @brief 変数ヒープを得る．
inline
VarHeap&
YmSat::var_heap()
{
  return mVarHeap;
}

// @brief 変数の数を得る．
inline
int
YmSat::variable_num()
{
  return mVarNum;
}

// @brief 制約節数を得る．
inline
int
YmSat::clause_num()
{
  return mConstrClauseNum;
}

// @brief 学習節のリストを返す．
inline
vector<Clause*>&
YmSat::learnt_clause_list()
{
  return mLearntClauseList;
}

// @brief 学習節のアクティビティ増加量を返す．
inline
double
YmSat::clause_bump() const
{
  return mClauseBump;
}

// watcher list を得る．
inline
WatcherList&
YmSat::watcher_list(Literal lit)
{
  int index = lit.index();
  ASSERT_COND( index < (mVarNum * 2) );
  return mWatcherList[index];
}

// Watcher を追加する．
inline
void
YmSat::add_watcher(Literal watch_lit,
		   Reason reason)
{
  watcher_list(watch_lit).add(Watcher(reason));
}

BEGIN_NONAMESPACE

inline
SatBool3
conv_to_Bool3(ymuint8 x)
{
  int tmp = static_cast<int>(x) - 1;
  return static_cast<SatBool3>(tmp);
}

inline
SatBool3
cur_val(ymuint8 x)
{
  return conv_to_Bool3(x & 3U);
}

END_NONAMESPACE

// 変数の評価を行う．
inline
SatBool3
YmSat::eval(int id) const
{
  ASSERT_COND( id >= 0 && id < mVarNum );

  return cur_val(mVal[id]);
}

// literal の評価を行う．
inline
SatBool3
YmSat::eval(Literal l) const
{
  int index = l.index();
  ASSERT_COND( (index / 2) < mVarNum );
  int x = mVal[index / 2] & 3U;
  int inv = index & 1U;
  int d = 1 - (inv * 2);
  return static_cast<SatBool3>((static_cast<int>(x) - 1) * d);
}

// @brief 変数の直前の値を返す．
inline
SatBool3
YmSat::old_val(int id) const
{
  ASSERT_COND( id >= 0 && id < mVarNum );

  return conv_to_Bool3((mVal[id] >> 2) & 3U);
}

// 値の割当てか可能かチェックする．
// 矛盾が起きたら false を返す．
inline
bool
YmSat::check_and_assign(Literal lit)
{
  auto old_val = eval(lit);
  if ( old_val != SatBool3::X ) {
    return old_val == SatBool3::True;
  }
  assign(lit);
  return true;
}

BEGIN_NONAMESPACE

inline
ymuint8
conv_from_Bool3(SatBool3 b)
{
  int tmp = static_cast<int>(b) + 1;
  return static_cast<ymuint8>(tmp);
}

END_NONAMESPACE

// 値の割当てを行う．
inline
void
YmSat::assign(Literal lit,
	      Reason reason)
{
  int lindex = lit.index();
  int vindex = lindex / 2;
  int inv = lindex & 1U;
  ymuint8 x = 2 - inv * 2;
  ASSERT_COND( vindex < mVarNum );
  mVal[vindex] = x;
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
YmSat::decision_level(int varid) const
{
  ASSERT_COND( varid >= 0 && varid < mVarNum );

  return mDecisionLevel[varid];
}

// 変数の割り当て理由を返す．
inline
Reason
YmSat::reason(int varid) const
{
  ASSERT_COND( varid >= 0 && varid < mVarNum );

  return mReason[varid];
}

// @brief clase が含意の理由になっているか調べる．
inline
bool
YmSat::is_locked(Clause* clause) const
{
  // 直感的には分かりにくいが，節の最初のリテラルは
  // 残りのリテラルによって含意されていることになっている．
  // そこで最初のリテラルの変数の割り当て理由が自分自身か
  // どうかを調べれば clause が割り当て理由として用いられて
  // いるかわかる．
  return reason(clause->wl0().varid()) == Reason{clause};
}

// @brief 変数のアクティビティを増加させる．
// @param[in] var 変数番号
inline
void
YmSat::bump_var_activity(int var)
{
  mVarHeap.bump_var_activity(var);
}

// @brief 変数のアクティビティを定率で減少させる．
inline
void
YmSat::decay_var_activity()
{
  mVarHeap.decay_var_activity();
}
#endif

END_NAMESPACE_YM_YMSATOLD

#endif // YMSATOLD_YMSAT_H
