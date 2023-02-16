#ifndef SATCORE_H
#define SATCORE_H

/// @file SatCore.h
/// @brief SatCore のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "SatSolverImpl.h"
#include "ym/SatBool3.h"
#include "Clause.h"
#include "Reason.h"
#include "AssignList.h"
#include "Watcher.h"
#include "VarHeap.h"
#include <chrono>

BEGIN_NAMESPACE_YM_SAT

class Controller;
class Analyzer;
class Selecter;

//////////////////////////////////////////////////////////////////////
/// @class SatCore SatCore.h "SatCore.h"
/// @brief
//////////////////////////////////////////////////////////////////////
class SatCore :
  public SatSolverImpl
{
public:

  /// @brief コンストラクタ
  SatCore(
    const string& controller_type, ///< [in] Controller のタイプを表す文字列
    const string& analyzer_type, ///< [in] Analyzer のタイプを表す文字列
    const string& selecter_type, ///< [in] Selecter のタイプを表す文字列
    const unordered_map<string, string>& selopt_dic ///< [in] オプション文字列を格納した辞書
  );

  /// @brief デストラクタ
  ~SatCore();


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const override
  {
    return mSane;
  }

  /// @brief 変数の数を得る．
  SizeType
  variable_num() const
  {
    return mVarNum;
  }

  /// @brief 制約節の数を得る．
  SizeType
  clause_num() const
  {
    return mConstrClauseNum;
  }

  /// @brief 二項制約節の数を得る．
  SizeType
  bin_clause_num() const
  {
    return mConstrBinList.size();
  }

  /// @brief 単項節の数を得る．
  SizeType
  unit_clause_num() const
  {
    return mConstrUnitList.size();
  }

  /// @brief 制約節のリテラルの総数を得る．
  SizeType
  literal_num() const
  {
    return mConstrLitNum;
  }

  /// @brief 学習節の数を得る．
  SizeType
  learnt_clause_num() const
  {
    return mLearntClauseList.size();
  }

  /// @brief 二項学習節の数を得る．
  SizeType
  learnt_bin_clause_num() const
  {
    return mLearntBinNum;
  }

  /// @brief 学習節のリテラルの総数を得る．
  SizeType
  learnt_literal_num() const
  {
    return mLearntLitNum;
  }

  /// @brief 制約節を得る．
  /// @return 制約節を返す．
  const Clause*
  clause(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < clause_num() )
  ) const
  {
    return mConstrClauseList[pos];
  }

  /// @brief 二項制約節の内容を得る．
  void
  bin_clause(
    SizeType pos,  ///< [in] 位置番号 ( 0 <= pos < bin_clause_num() )
    Literal& lit0, ///< [out] リテラル0
    Literal& lit1  ///< [out] リテラル1
  ) const
  {
    lit0 = mConstrBinList[pos].mLit0;
    lit1 = mConstrBinList[pos].mLit1;
  }

  /// @brief 単項節の内容を得る．
  Literal
  unit_clause(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < unit_clause_num() )
  ) const
  {
    return mConstrUnitList[pos];
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 変数の追加に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を追加する．
  /// @return 新しい変数番号を返す．
  ///
  /// 実際には変数番号を割り当てるだけで alloc_var()
  /// を呼ばれてはじめて実際の領域を確保する．
  SatVarId
  new_variable(
    bool decision ///< [in] 決定変数の時に true とする．
  ) override;

  /// @brief 変数に関する領域を実際に確保する．
  void
  alloc_var();


public:
  //////////////////////////////////////////////////////////////////////
  // 節の追加に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 節を追加する．
  void
  add_clause(
    const vector<SatLiteral>& lits ///< [in] リテラルのベクタ
  ) override;

  /// @brief 学習節を追加する．
  void
  add_learnt_clause(
    const vector<Literal>& lits ///< [in] 追加するリテラルのリスト
  );


public:
  //////////////////////////////////////////////////////////////////////
  // watcher list にアクセスする関数
  //////////////////////////////////////////////////////////////////////

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
    Literal lit,  ///< [in] リテラル
    Reason reason ///< [in] 理由
  )
  {
    auto w0 = Watcher{reason};
    auto& wlist = watcher_list(lit);
    wlist.add(w0);
  }

  /// @brief watcher を削除する．
  void
  del_watcher(
    Literal lit,  ///< [in] リテラル
    Reason reason ///< [in] 理由
  )
  {
    auto w0 = Watcher{reason};
    auto& wlist = watcher_list(lit);
    wlist.del(w0);
  }

  /// @brief 充足された watcher を削除する．
  void
  del_satisfied_watcher(
    Literal watch_lit ///< [in] リテラル
  );


public:
  //////////////////////////////////////////////////////////////////////
  // 変数の値割り当てに関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数の評価を行う．
  SatBool3
  eval(
    int var ///< [in] 変数番号
  ) const
  {
    ymuint8 x = mVal[var] & 3U;
    return conv_to_Bool3(x);
  }

  /// @brief リテラルの評価を行う．
  ///
  /// 肯定のリテラルの場合は変数の eval() 結果と同じ．
  /// 否定のリテラルの場合は結果を反転する．
  SatBool3
  eval(
    Literal lit ///< [in] リテラル
  ) const
  {
    auto index = lit.index();
    int vindex = index >> 1;
    int x = mVal[vindex] & 3U;
    int inv = index & 1U;
    int d = 1 - (inv * 2);
    return static_cast<SatBool3>((static_cast<int>(x) - 1) * d);
  }

  /// @brief バックトラック前の値を得る．
  SatBool3
  prev_val(
    SatVarId var ///< [in] 変数番号
  ) const
  {
    ymuint8 x = ((mVal[var] >> 2)) & 3U;
    return conv_to_Bool3(x);
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
    int inv = lindex & 1U;
    ymuint8 x = 2 - inv * 2;
    mVal[vindex] = x | (conv_from_Bool3(SatBool3::X) << 2);
    mDecisionLevel[vindex] = decision_level();
    mReason[vindex] = reason;

    // mAssignList に格納する．
    mAssignList.put(lit);
  }

  /// @brief 値の割当てが可能かチェックする．
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

  /// @brief 現在の decision level を返す．
  int
  decision_level() const
  {
    return mAssignList.cur_level();
  }

  /// @brief 割り当てリストの末尾の位置を得る．
  SizeType
  last_assign() const
  {
    return mAssignList.size() - 1;
  }

  /// @brief 割り当てリストの pos 番めの要素を得る．
  Literal
  get_assign(
    SizeType pos ///< [in] 位置番号 ( 0 <= pos < last_assign() )
  ) const
  {
    return mAssignList.get(pos);
  }

  /// @brief 変数の decision level を返す．
  int
  decision_level(
    SatVarId var ///< [in] 変数番号
  ) const
  {
    return mDecisionLevel[var];
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 探索に関する関数
  //////////////////////////////////////////////////////////////////////

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

  /// @brief 変数の割り当て理由を返す．
  Reason
  reason(
    SatVarId var ///< [in] 変数番号
  ) const
  {
    return mReason[var];
  }

  /// @brief 停止する．
  void
  stop() override
  {
    mGoOn = false;
  }

  /// @brief 時間計測機能を制御する
  void
  timer_on(
    bool enable
  ) override
  {
    mTimerOn = enable;
  }

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  void
  reg_msg_handler(
    SatMsgHandler* msg_handler ///< [in] 登録するメッセージハンドラ
  ) override;


public:
  //////////////////////////////////////////////////////////////////////
  // 変数と節のアクティビティパラメータに関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数ヒープを取り出す．
  VarHeap&
  var_heap()
  {
    return mVarHeap;
  }

  /// @brief アクティビティの低減率を設定する．
  void
  set_decay(
    double var_decay,   ///< [in] 変数の低減率
    double clause_decay ///< [in] 節の低減率
  )
  {
    mVarHeap.set_decay(var_decay);
    mClauseDecay = clause_decay;
  }

  /// @brief 変数のアクティビティを増加させる．
  void
  bump_var_activity(
    int var ///< [in] 変数番号
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
    mClauseBump *= (1.0 / mClauseDecay);
  }

#if 0 // forget_larnt_clause() で使うかも．
  /// @brief 変数のアクティビティを初期化する．
  void
  reset_activity();
#endif

  /// @brief 与えられた変数のリストからヒープ木を構成する．
  void
  build(
    const vector<SatVarId>& var_list ///< [in] 変数番号のリスト
  )
  {
    mVarHeap.build(var_list);
  }

  /// @brief リスタート回数を返す．
  SizeType
  restart_num() const
  {
    return mRestartNum;
  }

  /// @brief 総矛盾数を返す．
  SizeType
  conflict_num() const
  {
    return mConflictNum;
  }

  /// @brief 総分岐数を返す．
  SizeType
  decision_num() const
  {
    return mDecisionNum;
  }

  /// @brief 総インプリケーション数を返す．
  SizeType
  propagation_num() const
  {
    return mPropagationNum;
  }

  /// @brief 矛盾回数の制限値を得る．
  SizeType
  conflict_limit() const
  {
    return mConflictLimit;
  }

  /// @brief 学習節の制限値を得る．
  SizeType
  learnt_limit() const
  {
    return mLearntLimit;
  }

  /// @brief トータルの矛盾回数の制限値を得る．
  SizeType
  conflict_budget() const
  {
    return mConflictBudget;
  }

  /// @brief conflict_budget を設定する．
  /// @return 以前の設定値を返す．
  SizeType
  set_conflict_budget(
    SizeType val ///< [in] 設定する値
  ) override
  {
    auto old_val = mConflictBudget;
    mConflictBudget = val;
    return old_val;
  }

  /// @brief トータルの implication 数の制限値を得る．
  SizeType
  propagation_budget() const
  {
    return mPropagationBudget;
  }

  /// @brief propagation_budget を設定する．
  /// @return 以前の設定値を返す．
  SizeType
  set_propagation_budget(
    SizeType val ///< [in] 設定する値
  ) override
  {
    auto old_val = mPropagationBudget;
    mPropagationBudget = val;
    return old_val;
  }

  /// @brief 矛盾回数の制限値を設定する．
  void
  set_conflict_limit(
    SizeType limit ///< [in] 設定する値
  )
  {
    if ( mConflictBudget > 0 ) {
      limit = std::min(limit, mConflictBudget);
    }
    mConflictLimit = limit;
  }

  /// @brief 学習節の制限値を設定する．
  void
  set_learnt_limit(
    SizeType limit ///< [in] 設定する値
  )
  {
    mLearntLimit = limit;
  }

  /// @brief 現在の内部状態を得る．
  SatStats
  get_stats() const override;

  /// @brief ヒープの内容を出力する
  /// @param[in] s 出力先のストリーム
  void
  dump_heap(
    ostream& s
  ) const
  {
    mVarHeap.dump(s);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
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

  /// @brief 矛盾の原因を求める．
  ///
  /// 結果は mConflicts に格納する．
  void
  analyze_final(
    Literal p ///< [in] 直接の原因となっているリテラル
  );

  /// @brief バックトラック用のマーカーをセットする．
  void
  set_marker()
  {
    mAssignList.set_marker();
  }

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

  /// @brief mTmpLits を確保する．
  void
  alloc_lits(
    SizeType lit_num ///< [in] 必要なサイズ
  );

  /// @brief 新しい節を生成する．
  ///
  /// リテラルは mTmpLits[] に入れておく．
  Clause*
  new_clause(
    SizeType lit_num,   ///< [in] リテラル数
    bool learnt = false ///< [in] 学習節のとき true とするフラグ
  )
  {
    SizeType size = sizeof(Clause) + sizeof(Literal) * (lit_num - 1);
    auto p = new char[size];
    auto clause = new (p) Clause{lit_num, mTmpLits, learnt};

    return clause;
  }

  /// @brief 節を削除する．
  void
  delete_clause(
    Clause* clause ///< [in] 削除する節
  );

  /// @brief CNF を簡単化する．
  ///
  /// 具体的には implication() を行って充足している節を取り除く．
  /// もちろん decision_level が 0 の時しか実行できない．
  void
  reduce_CNF();

  /// @brief 充足している節を取り除く
  ///
  /// reduce_CNF() の中で用いられる．
  void
  sweep_clause(
    vector<Clause*>& clause_list ///< [in] 節のリスト
  );

  /// @brief 学習節の整理を行なう．
  void
  reduce_learnt_clause();

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

  /// @brief 変数に関する配列を拡張する．
  void
  expand_var();

  /// @brief ヒープを空にする．
  void
  clear()
  {
    mVarHeap.clear();
  }

  /// @brief 変数を再びヒープに追加する．
  void
  push(
    SatVarId var ///< [in] 追加する変数
  )
  {
    mVarHeap.push(var);
  }

  /// @brief 要素が空の時 true を返す．
  bool
  empty() const
  {
    return mVarHeap.empty();
  }

  /// @brief アクティビティ最大の変数番号を取り出す．
  ///
  /// 該当の変数はヒープから取り除かれる．
  SatVarId
  pop_top()
  {
    return mVarHeap.pop_top();
  }

  /// @brief decision variable の時 true を返す．
  bool
  is_decision_variable(
    SatVarId var ///< [in] 変数番号
  ) const
  {
    return mDvarArray[var];
  }

  /// @brief 制限値のチェックを行う．
  /// @retval true 制限を満たしている．
  /// @retval false 制限を違反している．
  bool
  check_budget()
  {
    return mGoOn &&
      (mConflictBudget == 0 || mConflictNum < mConflictBudget) &&
      (mPropagationBudget == 0 || mPropagationNum < mPropagationBudget);
  }

  /// @brief mVal[] で用いているエンコーディングを SatBool3 に変換する．
  static
  SatBool3
  conv_to_Bool3(
    ymuint8 x
  )
  {
    int tmp = static_cast<int>(x) - 1;
    return static_cast<SatBool3>(tmp);
  }

  /// @brief SatBool3 を mVal[] 用のエンコーディングに変換する．
  static
  ymuint8
  conv_from_Bool3(
    SatBool3 b
  )
  {
    int tmp = static_cast<int>(b) + 1;
    return static_cast<ymuint8>(tmp);
  }

  /// @brief 時間計測を開始する．
  ///
  /// ただし mTimerOn が false の時はなにもしない．
  void
  start_timer()
  {
    if ( mTimerOn ) {
      mStartTime = Clock::now();
    }
  }

  /// @brief 時間計測を終了する．
  ///
  /// ただし mTimerOn が false の時はなにもしない．
  void
  stop_timer()
  {
    if ( mTimerOn ) {
      auto t = Clock::now();
      auto d = std::chrono::duration_cast<Duration>(t - mStartTime);
      mAccTime += d;
    }
  }

  /// @brief ヘッダを出力する．
  void
  print_header();

  /// @brief 現在の状況を出力する．
  void
  print_stats();

  /// @brief フッタを出力する．
  void
  print_footer();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられるデータ構造
  //////////////////////////////////////////////////////////////////////

  /// @brief 二項節を表す構造体
  struct BinClause
  {
    /// @brief 空のコンストラクタ
    BinClause()
    {
    }

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

  // コントローラー
  unique_ptr<Controller> mController;

  // 解析器
  unique_ptr<Analyzer> mAnalyzer;

  // 変数選択器
  unique_ptr<Selecter> mSelecter;

  // 学習節のアクティビティの増加量
  double mClauseBump{1.0};

  // 学習節のアクティビティの減衰量
  double mClauseDecay{1.0};

  // 正常の時に true となっているフラグ
  bool mSane{true};

  // assumption の配列
  vector<Literal> mAssumptions;

  // 制約節の配列
  vector<Clause*> mConstrClauseList;

  // 二項節を表すリテラルのリスト
  vector<BinClause> mConstrBinList;

  // 単項節のリスト(リテラルのリスト)
  vector<Literal> mConstrUnitList;

  // 制約節の数(二項制約節も含む)
  SizeType mConstrClauseNum{0};

  // 制約節の総リテラル数 (二項制約節も含む)
  SizeType mConstrLitNum{0};

  // 学習節の配列
  vector<Clause*> mLearntClauseList;

  // 二項学習節の数
  SizeType mLearntBinNum{0};

  // 学習節の総リテラル数 (二項制約節も含む)
  SizeType mLearntLitNum{0};

  // decision variable の時 true となる配列
  vector<bool> mDvarArray;

  // decision variable の数
  SizeType mDvarNum{0};

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

  // 値割り当てを保持するリスト
  AssignList mAssignList;

  // 前回の sweep 時の割り当て数
  int mSweep_assigns;

  // 前回の sweep 時のリテラル数
  int mSweep_props;

  // add_clause で一時的に利用するリテラル配列
  Literal* mTmpLits{nullptr};

  // mTmpLits のサイズ
  SizeType mTmpLitsSize{0};

  // 矛盾の解析時にテンポラリに使用される節
  Clause* mTmpBinClause{nullptr};

  // 変数のヒープ木
  VarHeap mVarHeap;

  // 動作フラグ
  bool mGoOn{false};

  // 矛盾の原因となっているリテラルのリスト
  vector<Literal> mConflicts;

  // リスタート回数
  SizeType mRestartNum{0};

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
  SizeType mConflictBudget{0};

  // トータルの implication 数の制限
  SizeType mPropagationBudget{0};

  // mTimer を使うとき true にするフラグ
  bool mTimerOn{false};

  using Clock = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<Clock>;
  using Duration = std::chrono::milliseconds;

  // 直前の開始時刻
  TimePoint mStartTime;

  // 累積時間
  Duration mAccTime;

  // メッセージハンドラのリスト
  vector<SatMsgHandler*> mMsgHandlerList;


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
  //const ymuint debug = debug_solve | debug_decision | debug_implication;
  //const ymuint debug = debug_solve | debug_decision;
  //const ymuint debug = debug_solve | debug_assign;
  //const ymuint debug = debug_all;
  const ymuint debug = debug_none;

};

END_NAMESPACE_YM_SAT

#endif // COREMGR_H
