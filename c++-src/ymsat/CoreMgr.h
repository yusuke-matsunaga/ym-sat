#ifndef COREMGR_H
#define COREMGR_H

/// @file CoreMgr.h
/// @brief CoreMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"
#include "ym/SatBool3.h"

#include "SatReason.h"
#include "AssignList.h"
#include "Watcher.h"

#include <chrono>

BEGIN_NAMESPACE_YM_SAT

class SatClause;
class Controller;
class Analyzer;
class Selecter;

//////////////////////////////////////////////////////////////////////
/// @class CoreMgr CoreMgr.h "CoreMgr.h"
/// @brief 変数関係のデータ構造を管理するクラス
///
//////////////////////////////////////////////////////////////////////
class CoreMgr
{
public:

  /// @brief コンストラクタ
  CoreMgr();

  /// @brief デストラクタ
  ~CoreMgr();


public:
  //////////////////////////////////////////////////////////////////////
  // 情報を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 正しい状態のときに true を返す．
  bool
  sane() const;

  /// @brief 変数の数を得る．
  int
  variable_num() const;

  /// @brief 制約節の数を得る．
  int
  clause_num() const;

  /// @brief 二項制約節の数を得る．
  int
  bin_clause_num() const;

  /// @brief 単項節の数を得る．
  int
  unit_clause_num() const;

  /// @brief 制約節のリテラルの総数を得る．
  int
  literal_num() const;

  /// @brief 学習節の数を得る．
  int
  learnt_clause_num() const;

  /// @brief 二項学習節の数を得る．
  int
  learnt_bin_clause_num() const;

  /// @brief 学習節のリテラルの総数を得る．
  int
  learnt_literal_num() const;

  /// @brief 制約節を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < clause_num() )
  /// @return 制約節を返す．
  const SatClause*
  clause(int pos) const;

  /// @brief 二項制約節の内容を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < bin_clause_num() )
  /// @param[out] lit0, lit1 リテラル
  void
  bin_clause(int pos,
	     SatLiteral& lit0,
	     SatLiteral& lit1) const;

  /// @brief 単項節の内容を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < unit_clause_num() )
  SatLiteral
  unit_clause(int pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 変数の追加に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数を追加する．
  /// @param[in] decision 決定変数の時に true とする．
  /// @return 新しい変数番号を返す．
  ///
  /// 実際には変数番号を割り当てるだけで alloc_var()
  /// を呼ばれてはじめて実際の領域を確保する．
  int
  new_variable(bool decision);

  /// @brief 変数に関する領域を実際に確保する．
  void
  alloc_var();


public:
  //////////////////////////////////////////////////////////////////////
  // 節の追加に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 学習節を追加する．
  /// @param[in] lits 追加するリテラルのリスト
  void
  add_learnt_clause(const vector<SatLiteral>& lits);


public:
  //////////////////////////////////////////////////////////////////////
  // watcher list にアクセスする関数
  //////////////////////////////////////////////////////////////////////

  /// @brief watcher list を得る．
  /// @param[in] lit リテラル
  WatcherList&
  watcher_list(SatLiteral lit);

  /// @brief Watcher を追加する．
  /// @param[in] lit リテラル
  /// @param[in] reason 理由
  void
  add_watcher(SatLiteral lit,
	      SatReason reason);

  /// @brief watcher を削除する．
  /// @param[in] lit リテラル
  /// @param[in] reason 理由
  void
  del_watcher(SatLiteral lit,
	      SatReason reason);

  /// @brief 充足された watcher を削除する．
  /// @param[in] watch_lit リテラル
  void
  del_satisfied_watcher(SatLiteral watch_lit);


public:
  //////////////////////////////////////////////////////////////////////
  // 変数の値割り当てに関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数の評価を行う．
  /// @param[in] var 変数番号
  SatBool3
  eval(int var) const;

  /// @brief リテラルの評価を行う．
  /// @param[in] lit リテラル
  ///
  /// 肯定のリテラルの場合は変数の eval() 結果と同じ．
  /// 否定のリテラルの場合は結果を反転する．
  SatBool3
  eval(SatLiteral lit) const;

  /// @brief バックトラック前の値を得る．
  /// @param[in] var 変数番号
  SatBool3
  prev_val(int var) const;

  /// @brief 値の割当てを行う．
  /// @param[in] lit 割り当てるリテラル
  /// @param[in] reason 割り当ての理由
  void
  assign(SatLiteral lit,
	 SatReason reason = SatReason());

  /// @brief 値の割当てが可能かチェックする．
  /// @param[in] lit 割り当てるリテラル
  /// @return 矛盾が起きたら false を返す．
  ///
  /// すでに同じ値が割り当てられていたらなにもしない．
  /// 割り当てには assign() を呼び出す．
  bool
  check_and_assign(SatLiteral lit);

  /// @brief 現在の decision level を返す．
  int
  decision_level() const;

  /// @brief 割り当てリストの末尾の位置を得る．
  int
  last_assign() const;

  /// @brief 割り当てリストの pos 番めの要素を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < last_assign() )
  SatLiteral
  get_assign(int pos) const;

  /// @brief 変数の decision level を返す．
  /// @param[in] var 変数番号
  int
  decision_level(int var) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 探索に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief SAT 問題を解く．
  /// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
  /// @param[out] model 充足するときの値の割り当てを格納する配列．
  /// @param[in] controller コントローラー
  /// @param[in] analyzer 解析器
  /// @param[in] selecter リテラル選択器
  /// @retval SatBool3::True 充足した．
  /// @retval SatBool3::False 充足不能が判明した．
  /// @retval SatBool3::X わからなかった．
  /// @note i 番めの変数の割り当て結果は model[i] に入る．
  SatBool3
  solve(const vector<SatLiteral>& assumptions,
	SatModel& model,
	Controller& controller,
	Analyzer& analyzer,
	Selecter& selecter);

  /// @brief 変数の割り当て理由を返す．
  /// @param[in] var 変数番号
  SatReason
  reason(int var) const;

  /// @brief 次に割り当てる変数を取り出す．
  ///
  /// アクティビティが最大で未割り当ての変数を返す．
  int
  next_var();

  /// @brief 停止する．
  void
  stop();

  /// @brief 時間計測機能を制御する
  void
  timer_on(bool enable);

  /// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
  /// @param[in] msg_handler 登録するメッセージハンドラ
  void
  reg_msg_handler(SatMsgHandler* msg_handler);


public:
  //////////////////////////////////////////////////////////////////////
  // 変数と節のアクティビティパラメータに関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief アクティビティの低減率を設定する．
  /// @param[in] var_decay 変数の低減率
  /// @param[in] clause_decay 節の低減率
  void
  set_decay(double var_decay,
	    double clause_decay);

  /// @brief 変数のアクティビティを増加させる．
  /// @param[in] var 変数番号
  void
  bump_var_activity(int var);

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

#if 0 // forget_larnt_clause() で使うかも．
  /// @brief 変数のアクティビティを初期化する．
  void
  reset_activity();
#endif

  /// @brief 与えられた変数のリストからヒープ木を構成する．
  void
  build(const vector<int>& var_list);

  /// @brief リスタート回数を返す．
  int
  restart_num() const;

  /// @brief 総矛盾数を返す．
  int
  conflict_num() const;

  /// @brief 総分岐数を返す．
  int
  decision_num() const;

  /// @brief 総インプリケーション数を返す．
  int
  propagation_num() const;

  /// @brief 矛盾回数の制限値を得る．
  int
  conflict_limit() const;

  /// @brief 学習節の制限値を得る．
  int
  learnt_limit() const;

  /// @brief conflict_limit の最大値を返す．
  int
  max_conflict() const;

  /// @brief conflict_limit の最大値を設定する．
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  int
  set_max_conflict(int val);

  /// @brief 矛盾回数の制限値を設定する．
  /// @param[in] limit 設定する値
  void
  set_conflict_limit(int limit);

  /// @brief 学習節の制限値を設定する．
  /// @param[in] limit 設定する値
  void
  set_learnt_limit(int limit);

  /// @brief 現在の内部状態を得る．
  SatStats
  get_stats() const;

  /// @brief ヒープの内容を出力する
  /// @param[in] s 出力先のストリーム
  void
  dump_heap(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 探索を行う本体の関数
  /// @param[in] controller Controller オブジェクト
  /// @param[in] analyzer Analyzer オブジェクト
  /// @param[in] selecter Selecter オブジェクト
  /// @retval SatBool3::True 充足した．
  /// @retval SatBool3::False 充足できないことがわかった．
  /// @retval SatBool3::X 矛盾の生起回数が mConflictLimit を超えた．
  ///
  /// 矛盾の結果新たな学習節が追加される場合もあるし，
  /// 内部で reduce_learnt_clause() を呼んでいるので学習節が
  /// 削減される場合もある．
  SatBool3
  search(Controller& controller,
	 Analyzer& analyzer,
	 Selecter& selecter);

  /// @brief バックトラック用のマーカーをセットする．
  void
  set_marker();

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

  /// @brief 動作フラグを得る．
  bool
  go_on() const;

  /// @brief mTmpLits を確保する．
  /// @param[in] lit_num 必要なサイズ
  void
  alloc_lits(int lit_num);

  /// @brief 新しい節を生成する．
  /// @param[in] lit_num リテラル数
  /// @param[in] learnt 学習節のとき true とするフラグ
  ///
  /// リテラルは mTmpLits[] に入れておく．
  SatClause*
  new_clause(int lit_num,
	     bool learnt = false);

  /// @brief 節を削除する．
  /// @param[in] clause 削除する節
  void
  delete_clause(SatClause* clause);

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
  sweep_clause(vector<SatClause*>& clause_list);

  /// @brief 学習節の整理を行なう．
  void
  reduce_learnt_clause();

  /// @brief 学習節が使われているか調べる．
  /// @param[in] clause 対象の節
  bool
  is_locked(SatClause* clause) const;

  /// @brief 変数に関する配列を拡張する．
  void
  expand_var();

  /// @brief ヒープを空にする．
  void
  clear();

  /// @brief 変数をヒープに追加する．
  /// @param[in] var 追加する変数
  void
  add_var(int var);

  /// @brief 変数を再びヒープに追加する．
  /// @param[in] var 追加する変数
  void
  push(int var);

  /// @brief 要素が空の時 true を返す．
  bool
  empty() const;

  /// @brief アクティビティ最大の変数番号を取り出す．
  /// @note 該当の変数はヒープから取り除かれる．
  int
  pop_top();

  /// @brief 引数の位置にある要素を適当な位置まで沈めてゆく
  /// @param[in] pos 対象の要素の位置
  void
  move_down(int pos);

  /// @brief 引数の位置にある要素を適当な位置まで上げてゆく
  /// @param[in] vindex 対象の変数番号
  void
  move_up(int vindex);

  /// @brief 変数を配列にセットする．
  /// @param[in] vindex 対象の変数番号
  /// @param[in] pos 位置
  /// @note mHeap と mHeapPos の一貫性を保つためにはこの関数を使うこと．
  void
  set(int vindex,
      int pos);

  /// @brief 左の子供の位置を計算する
  /// @param[in] pos 親の位置
  static
  int
  left(int pos);

  /// @brief 右の子供の位置を計算する．
  /// @param[in] pos 親の位置
  static
  int
  right(int pos);

  /// @brief 親の位置を計算する．
  /// @param[in] pos 子供の位置
  /// @note 左の子供でも右の子供でも同じ
  static
  int
  parent(int pos);

  /// @brief mVal[] で用いているエンコーディングを SatBool3 に変換する．
  static
  SatBool3
  conv_to_Bool3(ymuint8 x);

  /// @brief SatBool3 を mVal[] 用のエンコーディングに変換する．
  static
  ymuint8
  conv_from_Bool3(SatBool3 b);

  /// @brief 時間計測を開始する．
  ///
  /// ただし mTimerOn が false の時はなにもしない．
  void
  start_timer();

  /// @brief 時間計測を終了する．
  ///
  /// ただし mTimerOn が false の時はなにもしない．
  void
  stop_timer();

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
    BinClause(SatLiteral l0,
	      SatLiteral l1)
    {
      mLit0 = l0;
      mLit1 = l1;
    }

    SatLiteral mLit0;
    SatLiteral mLit1;
  };


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数のアクティビティの増加量
  double mVarBump;

  // 変数のアクティビティの減衰量
  double mVarDecay;

  // 学習節のアクティビティの増加量
  double mClauseBump;

  // 学習節のアクティビティの減衰量
  double mClauseDecay;

  // 正常の時に true となっているフラグ
  bool mSane;

  // 制約節の配列
  vector<SatClause*> mConstrClauseList;

  // 二項節を表すリテラルのリスト
  vector<BinClause> mConstrBinList;

  // 単項節のリスト(リテラルのリスト)
  vector<SatLiteral> mConstrUnitList;

  // 制約節の総リテラル数 (二項制約節も含む)
  int mConstrLitNum;

  // 学習節の配列
  vector<SatClause*> mLearntClauseList;

  // 二項学習節の数
  int mLearntBinNum;

  // 学習節の総リテラル数 (二項制約節も含む)
  int mLearntLitNum;

#if YMSAT_USE_DVAR
  // dvar 配列
  vector<bool> mDvarArray;
#endif

  // 変数の数
  int mVarNum;

  // 前回の alloc_var で処理した時の変数の数
  int mOldVarNum;

  // 変数関係の配列のサイズ
  int mVarSize;

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

  // 値割り当てを保持するリスト
  AssignList mAssignList;

  // 前回の sweep 時の割り当て数
  int mSweep_assigns;

  // 前回の sweep 時のリテラル数
  int mSweep_props;

  // add_clause で一時的に利用するリテラル配列
  SatLiteral* mTmpLits;

  // mTmpLits のサイズ
  int mTmpLitsSize;

  // 矛盾の解析時にテンポラリに使用される節
  SatClause* mTmpBinClause;

  // ヒープ上の位置の配列
  // サイズは mVarSize
  int* mHeapPos;

  // アクティビティの配列
  // サイズは mVarSize
  double* mActivity;

  // ヒープ用の配列
  // サイズは mVarSize
  int* mHeap;

  // ヒープの要素数
  int mHeapNum;

  // 動作フラグ
  bool mGoOn;

  // リスタート回数
  int mRestartNum;

  // 総コンフリクト数
  int mConflictNum;

  // 総 decision 数
  int mDecisionNum;

  // 総 implication 数
  int mPropagationNum;

  // コンフリクト数の制限
  int mConflictLimit;

  // 学習節の制限
  int mLearntLimit;

  // トータルのコンフリクト数の制限
  int mMaxConflict;

  // mTimer を使うとき true にするフラグ
  bool mTimerOn;

  using Clock = std::chrono::system_clock;
  using TimePoint = std::chrono::time_point<Clock>;
  using Duration = std::chrono::milliseconds;

  // 直前の開始時刻
  TimePoint mStartTime;

  // 累積時間
  Duration mAccTime;

  // メッセージハンドラのリスト
  list<SatMsgHandler*> mMsgHandlerList;


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


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 正しい状態のときに true を返す．
inline
bool
CoreMgr::sane() const
{
  return mSane;
}

// @brief 変数の数を得る．
inline
int
CoreMgr::variable_num() const
{
  return mVarNum;
}

// @brief 制約節の数を得る．
inline
int
CoreMgr::clause_num() const
{
  return mConstrClauseList.size();
}

// @brief 二項制約節の数を得る．
inline
int
CoreMgr::bin_clause_num() const
{
  return mConstrBinList.size();
}

// @brief 単項節の数を得る．
inline
int
CoreMgr::unit_clause_num() const
{
  return mConstrUnitList.size();
}

// @brief 制約節のリテラルの総数を得る．
inline
int
CoreMgr::literal_num() const
{
  return mConstrLitNum;
}

// @brief 学習節の数を得る．
inline
int
CoreMgr::learnt_clause_num() const
{
  return mLearntClauseList.size();
}

// @brief 二項学習節の数を得る．
inline
int
CoreMgr::learnt_bin_clause_num() const
{
  return mLearntBinNum;
}

// @brief 学習節のリテラルの総数を得る．
inline
int
CoreMgr::learnt_literal_num() const
{
  return mLearntLitNum;
}

// @brief 制約節を得る．
// @param[in] pos 位置番号 ( 0 <= pos < clause_num() )
// @return 制約節を返す．
inline
const SatClause*
CoreMgr::clause(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < clause_num() );
  return mConstrClauseList[pos];
}

// @brief 二項制約節の内容を得る．
// @param[in] pos 位置番号 ( 0 <= pos < bin_clause_num() )
// @param[out] lit0, lit1 リテラル
inline
void
CoreMgr::bin_clause(int pos,
		    SatLiteral& lit0,
		    SatLiteral& lit1) const
{
  ASSERT_COND( pos >= 0 && pos < bin_clause_num() );
  lit0 = mConstrBinList[pos].mLit0;
  lit1 = mConstrBinList[pos].mLit1;
}

// @brief 単項節の内容を得る．
// @param[in] pos 位置番号 ( 0 <= pos < unit_clause_num() )
inline
SatLiteral
CoreMgr::unit_clause(int pos) const
{
  ASSERT_COND( pos >= 0 && pos < unit_clause_num() );
  return mConstrUnitList[pos];
}

// @brief アクティビティの低減率を設定する．
// @param[in] var_decay 変数の低減率
// @param[in] clause_decay 節の低減率
inline
void
CoreMgr::set_decay(double var_decay,
		   double clause_decay)
{
  mVarDecay = var_decay;
  mClauseDecay = clause_decay;
}

// @brief 要素が空の時 true を返す．
inline
bool
CoreMgr::empty() const
{
  return mHeapNum == 0;
}

// @brief 変数を始めてヒープに追加する．
// @param[in] var 変数番号
inline
void
CoreMgr::add_var(int var)
{
  set(var, mHeapNum);
  mActivity[var] = 0.0;
  ++ mHeapNum;
}

// watcher list を得る．
inline
WatcherList&
CoreMgr::watcher_list(SatLiteral lit)
{
  int index = lit.index();
  ASSERT_COND( index >= 0 && index < mVarNum * 2 );
  return mWatcherList[index];
}

// @brief Watcher を追加する．
// @param[in] watch_lit リテラル
// @param[in] reason 理由
inline
void
CoreMgr::add_watcher(SatLiteral watch_lit,
		     SatReason reason)
{
  watcher_list(watch_lit).add(Watcher(reason));
}

// @brief mVal[] で用いているエンコーディングを SatBool3 に変換する．
inline
SatBool3
CoreMgr::conv_to_Bool3(ymuint8 x)
{
  int tmp = static_cast<int>(x) - 1;
  return static_cast<SatBool3>(tmp);
}

// @brief SatBool3 を mVal[] 用のエンコーディングに変換する．
inline
ymuint8
CoreMgr::conv_from_Bool3(SatBool3 b)
{
  int tmp = static_cast<int>(b) + 1;
  return static_cast<ymuint8>(tmp);
}

// @brief 変数の評価を行う．
// @param[in] var 変数
inline
SatBool3
CoreMgr::eval(int var) const
{
  ASSERT_COND( var >= 0 && var < mVarNum );
  ymuint8 x = mVal[var] & 3U;
  return conv_to_Bool3(x);
}

// @brief リテラルの評価を行う．
// @param[in] lit リテラル
inline
SatBool3
CoreMgr::eval(SatLiteral lit) const
{
  int index = lit.index();
  int vindex = index >> 1;
  ASSERT_COND( vindex >= 0 && vindex < mVarNum );
  int x = mVal[vindex] & 3U;
  int inv = index & 1U;
  int d = 1 - (inv * 2);
  return static_cast<SatBool3>((static_cast<int>(x) - 1) * d);
}

// @brief バックトラック前の値を得る．
inline
SatBool3
CoreMgr::prev_val(int var) const
{
  ASSERT_COND( var >= 0 && var < mVarNum );
  ymuint8 x = ((mVal[var] >> 2)) & 3U;
  return conv_to_Bool3(x);
}

// 値の割当てを行う．
inline
void
CoreMgr::assign(SatLiteral lit,
		SatReason reason)
{
  int lindex = lit.index();
  int vindex = lindex / 2;
  int inv = lindex & 1U;
  ymuint8 x = 2 - inv * 2;
  ASSERT_COND( vindex >= 0 && vindex < mVarNum );
  mVal[vindex] = x | (conv_from_Bool3(SatBool3::X) << 2);
  mDecisionLevel[vindex] = decision_level();
  mReason[vindex] = reason;

  // mAssignList に格納する．
  mAssignList.put(lit);
}

// @brief 値の割当てか可能かチェックする．
// @param[in] lit 割り当てるリテラル
// @return 矛盾が起きたら false を返す．
//
// すでに同じ値が割り当てられていたらなにもしない．
// 割り当てには assign() を呼び出す．
inline
bool
CoreMgr::check_and_assign(SatLiteral lit)
{
  SatBool3 old_val = eval(lit);
  if ( old_val != SatBool3::X ) {
    return old_val == SatBool3::True;
  }
  assign(lit);
  return true;
}

// @brief 現在の decision level を返す．
inline
int
CoreMgr::decision_level() const
{
  return mAssignList.cur_level();
}

// @brief 割り当てリストの末尾の位置を得る．
inline
int
CoreMgr::last_assign() const
{
  return mAssignList.size() - 1;
}

// @brief 割り当てリストの pos 番めの要素を得る．
// @param[in] pos 位置番号
inline
SatLiteral
CoreMgr::get_assign(int pos) const
{
  return mAssignList.get(pos);
}

// @brief 変数の decision level を返す．
// @param[in] var 変数番号
inline
int
CoreMgr::decision_level(int var) const
{
  ASSERT_COND( var >= 0 && var < mVarNum );
  return mDecisionLevel[var];
}

// @brief バックトラック用のマーカーをセットする．
inline
void
CoreMgr::set_marker()
{
  mAssignList.set_marker();
}

// @brief 変数の割り当て理由を返す．
// @param[in] var 変数
inline
SatReason
CoreMgr::reason(int var) const
{
  ASSERT_COND( var >= 0 && var < mVarNum );
  return mReason[var];
}

// @brief 停止する．
inline
void
CoreMgr::stop()
{
  mGoOn = false;
}

// @brief 動作フラグを得る．
inline
bool
CoreMgr::go_on() const
{
  return mGoOn;
}

// @brief 要素を追加する．
inline
void
CoreMgr::push(int var)
{
  if ( mHeapPos[var] == -1 ) {
    int pos = mHeapNum;
    ++ mHeapNum;
    set(var, pos);
    move_up(pos);
  }
}

// @brief もっともアクティビティの高い変数を返す．
inline
int
CoreMgr::pop_top()
{
  ASSERT_COND( mHeapNum > 0 );

  int ans = mHeap[0];
  mHeapPos[ans] = -1;
  -- mHeapNum;
  if ( mHeapNum > 0 ) {
    int vindex = mHeap[mHeapNum];
    set(vindex, 0);
    move_down(0);
  }
  return ans;
}

// 引数の位置にある要素を適当な位置まで上げてゆく
inline
void
CoreMgr::move_up(int pos)
{
  int vindex = mHeap[pos];
  double val = mActivity[vindex];
  while ( pos > 0 ) {
    int pos_p = parent(pos);
    int vindex_p = mHeap[pos_p];
    double val_p = mActivity[vindex_p];
    if ( val_p >= val ) {
      break;
    }
    set(vindex, pos_p);
    set(vindex_p, pos);
    pos = pos_p;
  }
}

// 変数を配列にセットする．
inline
void
CoreMgr::set(int vindex,
	     int pos)
{
  mHeap[pos] = vindex;
  mHeapPos[vindex] = pos;
}

// @brief 左の子供の位置を計算する
inline
int
CoreMgr::left(int pos)
{
  return pos + pos + 1;
}

// @brief 右の子供の位置を計算する．
inline
int
CoreMgr::right(int pos)
{
  return pos + pos + 2;
}

// @brief 親の位置を計算する．
inline
int
CoreMgr::parent(int pos)
{
  return (pos - 1) >> 1;
}

// 変数のアクティビティを定率で減少させる．
inline
void
CoreMgr::decay_var_activity()
{
  mVarBump *= (1.0 / mVarDecay);
}

// 学習節のアクティビティを定率で減少させる．
inline
void
CoreMgr::decay_clause_activity()
{
  mClauseBump *= (1.0 / mClauseDecay);
}

// @brief リスタート回数を返す．
inline
int
CoreMgr::restart_num() const
{
  return mRestartNum;
}

// @brief 総矛盾数を返す．
inline
int
CoreMgr::conflict_num() const
{
  return mConflictNum;
}

// @brief 総分岐数を返す．
inline
int
CoreMgr::decision_num() const
{
  return mDecisionNum;
}

// @brief 総インプリケーション数を返す．
inline
int
CoreMgr::propagation_num() const
{
  return mPropagationNum;
}

// @brief 矛盾回数の制限値を得る．
inline
int
CoreMgr::conflict_limit() const
{
  return mConflictLimit;
}

// @brief 学習節の制限値を得る．
inline
int
CoreMgr::learnt_limit() const
{
  return mLearntLimit;
}

// @brief conflict_limit の最大値を返す．
inline
int
CoreMgr::max_conflict() const
{
  return mMaxConflict;
}

// @brief conflict_limit の最大値を設定する．
// @param[in] val 設定する値
// @return 以前の設定値を返す．
inline
int
CoreMgr::set_max_conflict(int val)
{
  int old_val = mMaxConflict;
  mMaxConflict = val;
  return old_val;
}

// @brief 矛盾回数の制限値を設定する．
// @param[in] limit 設定する値
inline
void
CoreMgr::set_conflict_limit(int limit)
{
  mConflictLimit = limit;
  if ( mConflictLimit > mMaxConflict ) {
    mConflictLimit = mMaxConflict;
  }
}

// @brief 学習節の制限値を設定する．
// @param[in] limit 設定する値
inline
void
CoreMgr::set_learnt_limit(int limit)
{
  mLearntLimit = limit;
}

// @brief 時間計測機能を制御する
inline
void
CoreMgr::timer_on(bool enable)
{
  mTimerOn = enable;
}

// @brief 時間計測を開始する．
//
// ただし mTimerOn が false の時はなにもしない．
inline
void
CoreMgr::start_timer()
{
  if ( mTimerOn ) {
    mStartTime = Clock::now();
  }
}

// @brief 時間計測を終了する．
//
// ただし mTimerOn が false の時はなにもしない．
inline
void
CoreMgr::stop_timer()
{
  if ( mTimerOn ) {
    auto t = Clock::now();
    auto d = std::chrono::duration_cast<Duration>(t - mStartTime);
    mAccTime += d;
  }
}

END_NAMESPACE_YM_SAT

#endif // COREMGR_H
