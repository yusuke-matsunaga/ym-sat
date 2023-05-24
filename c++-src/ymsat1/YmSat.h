#ifndef YMSAT1_YMSAT_H
#define YMSAT1_YMSAT_H

/// @file YmSat.h
/// @brief YmSat のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2016, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym_sat1.h"
#include "SatSolverImpl.h"
#include "Clause.h"
#include "Reason.h"
#include "AssignList.h"
#include "Watcher.h"
#include "VarHeap.h"
#include "conf.h"
#include <chrono>
#include <random>
#include <json/json.h>


BEGIN_NAMESPACE_YM_SAT1

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
  YmSat(
    const Json::Value& js_obj ///< [in] 初期化パラメータ
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
  /// @note 変数番号は 0 から始まる．
  SatVarId
  new_variable(
    bool decision ///< [in] 決定変数の時に true とする．
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

  /// @brief トータルの矛盾回数の制限を設定する．
  /// @return 以前の設定値を返す．
  SizeType
  set_conflict_budget(
    SizeType val ///< [in] 設定する値
  ) override;

  /// @brief トータルの implication 回数の制限を設定する．
  /// @return 以前の設定値を返す．
  SizeType
  set_propagation_budget(
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

  /// @brief 割当てキューに基づいて implication を行う．
  /// @return 矛盾が生じたら矛盾の原因を返す．
  ///
  /// 矛盾が生じていなかったら Reason::None を返す．
  /// 矛盾の原因とは実際には充足していない節である．
  Reason
  implication();

  /// @brief level までバックトラックする
  void
  backtrack(
    int level ///< [in] バックトラックするレベル
  );

  /// @brief 次の割り当てを選ぶ．
  ///
  /// 割り当てられる変数がない場合には Literal::X を返す．
  Literal
  next_decision();

  /// @brief 値の割当てか可能かチェックする．
  /// @return 矛盾が起きたら false を返す．
  ///
  /// すでに同じ値が割り当てられていたらなにもしない．
  /// 割り当てには assign() を呼び出す．
  bool
  check_and_assign(
    Literal lit ///< [in] 割り当てるリテラル
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
  void
  assign(
    Literal lit,                 ///< [in] 割り当てるリテラル
    Reason reason = Reason::None ///< [in] 割り当ての理由
  )
  {
    auto lindex = lit.index();
    int vindex = lindex / 2;
    ASSERT_COND( vindex < mVarNum );
    int inv = lindex & 1U;
    std::uint8_t x = 2 - inv * 2;
    mVal[vindex] = x | ((conv_from_Bool3(SatBool3::X)) << 2);
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

  /// @brief 新しい節を生成する．
  ///
  /// リテラルは mTmpLits に格納されている．
  Clause*
  new_clause(
    SizeType lit_num,   ///< [in] リテラル数
    bool learnt = false ///< [in] 学習節のとき true とするフラグ
  );

  /// @brief mTmpLits を確保する．
  void
  alloc_lits(
    SizeType lit_num ///< [in] リテラル数
  );

  /// @brief 節を削除する．
  void
  delete_clause(
    Clause* clause ///< [in] 削除する節
  );

  /// @brief watcher list を得る．
  WatcherList&
  watcher_list(
    Literal lit ///< [in] リテラル
  )
  {
    auto index = lit.index();
    return mWatcherList[index];
  }

  /// @brief Watcher を追加する．
  void
  add_watcher(
    Literal watch_lit, ///< [in] リテラル
    Reason reason      ///< [in] 理由
  )
  {
    watcher_list(watch_lit).add(Watcher{reason});
  }

  /// @brief watcher を削除する．
  void
  del_watcher(
    Literal watch_lit, ///< [in] リテラル
    Reason reason      ///< [in] 理由
  );

  /// @brief 変数1の評価を行う．
  SatBool3
  eval(
    SatVarId id ///< [in] 変数番号
  ) const
  {
    ASSERT_COND( id >= 0 && id < mVarNum );
    return cur_val(mVal[id]);
  }

  /// @brief literal の評価を行う．
  SatBool3
  eval(
    Literal l ///< [in] リテラル
  ) const
  {
    auto index = l.index();
    ASSERT_COND( (index / 2) < mVarNum );
    int x = mVal[index / 2] & 3U;
    int inv = index & 1U;
    int d = 1 - (inv * 2);
    return static_cast<SatBool3>((static_cast<int>(x) - 1) * d);
  }

  /// @brief 現在の decision level を返す．
  int
  decision_level() const
  {
    return mAssignList.cur_level();
  }

  /// @brief 変数の decision level を返す．
  int
  decision_level(
    SatVarId varid ///< [in] 変数番号
  ) const
  {
    ASSERT_COND( varid >= 0 && varid < mVarNum );
    return mDecisionLevel[varid];
  }

#if YMSAT_USE_LBD
  /// @brief LBD を計算する．
  int
  calc_lbd(
    const Clause* clause ///< [in] 対象の節
  );
#endif

  /// @brief 変数の割り当て理由を返す．
  Reason
  reason(
    SatVarId varid ///< [in] 変数番号
  ) const
  {
    ASSERT_COND( varid >= 0 && varid < mVarNum );
    return mReason[varid];
  }

  /// @brief 学習節が使われているか調べる．
  bool
  is_locked(
    Clause* clause ///< [in] 対象の節
  ) const
  {
    // 直感的には分かりにくいが，節の最初のリテラルは
    // 残りのリテラルによって含意されていることになっている．
    // そこで最初のリテラルの変数の割り当て理由が自分自身か
    // どうかを調べれば clause が割り当て理由として用いられて
    // いるかわかる．
    return reason(clause->wl0().varid()) == Reason{clause};
  }

  /// @brief 変数のアクティビティを増加させる．
  void
  bump_var_activity(
    SatVarId var ///< [in] 変数番号
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
  void
  bump_clause_activity(
    Clause* clause ///< [in] 対象の学習節
  );

  /// @brief 学習節のアクティビティを定率で減少させる．
  void
  decay_clause_activity()
  {
    mClauseBump /= mClauseDecay;
  }

  /// @brief 実際に変数に関するデータ構造を生成する．
  void
  alloc_var()
  {
    if ( mOldVarNum < mVarNum ) {
      if ( mVarSize < mVarNum ) {
	expand_var();
      }
      for ( SatVarId var = mOldVarNum; var < mVarNum; ++ var ) {
	mVal[var] = conv_from_Bool3(SatBool3::X);
	mVarHeap.add_var(var);
      }
      mOldVarNum = mVarNum;
    }
  }

  /// @brief 変数に関する配列を拡張する．
  void
  expand_var();

  static
  SatBool3
  conv_to_Bool3(
    std::uint8_t x
  )
  {
    int tmp = static_cast<int>(x) - 1;
    return static_cast<SatBool3>(tmp);
  }

  static
  std::uint8_t
  conv_from_Bool3(
    SatBool3 b
  )
  {
    int tmp = static_cast<int>(b) + 1;
    return static_cast<std::uint8_t>(tmp);
  }

  static
  SatBool3
  cur_val(
    std::uint8_t x
  )
  {
    return conv_to_Bool3(x & 3U);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 解析器
  Analyzer* mAnalyzer;

  // 正常の時に true となっているフラグ
  bool mSane;

  // 制約節の配列
  vector<Clause*> mConstrClauseList;

  // 制約節の数
  SizeType mConstrClauseNum;

  // 二項制約節の数
  SizeType mConstrBinNum;

  // 制約節の総リテラル数 (二項制約節も含む)
  SizeType mConstrLitNum;

  // 学習節の配列
  vector<Clause*> mLearntClause;

  // 二項学習節の数
  SizeType mLearntBinNum;

  // 学習節の総リテラル数 (二項制約節も含む)
  SizeType mLearntLitNum;

#if YMSAT_USE_DVAR
  // dvar 配列
  vector<bool> mDvarArray;
#endif

  // 変数の数
  SizeType mVarNum;

  // 前回の alloc_var で処理した時の変数の数
  SizeType mOldVarNum;

  // 変数関係の配列のサイズ
  SizeType mVarSize;

  // 値の配列
  // サイズは mVarSize
  std::uint8_t* mVal;

  // 値が割り当てられたときのレベルの配列
  // サイズは mVarSize
  int* mDecisionLevel;

  // 値が割り当てられた理由の配列
  // サイズは mVarSize
  Reason* mReason;

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
  SizeType mLbdTmpSize;
#endif

  // 矛盾の解析時にテンポラリに使用される節
  Clause* mTmpBinClause;

  // search 開始時の decision level
  int mRootLevel;

  // 値割り当てを保持するリスト
  AssignList mAssignList;

  // 学習節のアクティビティの増加量
  double mClauseBump;

  // 学習節のアクティビティの減衰量
  double mClauseDecay;

  // 変数選択用乱数発生器
  std::mt19937 mRandGen;

  // mTimer を使うとき true にするフラグ
  bool mTimerOn;

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
  SizeType mRestart;

  // 総コンフリクト数
  SizeType mConflictNum;

  // 総 decision 数
  SizeType mDecisionNum;

  // 総 implication 数
  SizeType mPropagationNum;

  // コンフリクト数の制限
  SizeType mConflictLimit;

  // 学習節の制限
  SizeType mLearntLimit;

  // トータルのコンフリクト数の制限
  SizeType mMaxConflict;

  // stop() が用いるフラグ
  bool mGoOn;

  // メッセージハンドラのリスト
  list<SatMsgHandler*> mMsgHandlerList;

  // add_clause で一時的に利用するリテラル配列
  Literal* mTmpLits;

  // mTmpLits のサイズ
  SizeType mTmpLitsSize;

  // search() で用いられるリテラル配列
  vector<Literal> mLearntLits;


protected:
  //////////////////////////////////////////////////////////////////////
  // デバッグ用の定数，変数
  //////////////////////////////////////////////////////////////////////

  static
  const int debug_none        = 0x00;

  static
  const int debug_implication = 0x01;

  static
  const int debug_analyze     = 0x02;

  static
  const int debug_assign      = 0x04;

  static
  const int debug_decision    = 0x08;

  static
  const int debug_solve       = 0x10;

  static
  const int debug_all         = 0xffffffff;

  static
  //const int debug = debug_decision | debug_analyze | debug_assign;
  //const int debug = debug_assign;
  //const int debug = debug_assign | debug_implication;
  //const int debug = debug_assign | debug_analyze;
  //const int debug = debug_solve | debug_decision;
  //const int debug = debug_solve | debug_assign;
  //const int debug = debug_all;
  const int debug = debug_none;

};

END_NAMESPACE_YM_SAT1

#endif // YMSAT1_YMSAT_H
