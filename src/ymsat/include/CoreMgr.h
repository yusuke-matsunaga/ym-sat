#ifndef COREMGR_H
#define COREMGR_H

/// @file CoreMgr.h
/// @brief CoreMgr のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"
#include "ym/SatVarId.h"
#include "ym/SatBool3.h"
#include "ym/FragAlloc.h"
#include "ym/StopWatch.h"

#include "SatClause.h"
#include "SatReason.h"
#include "AssignList.h"
#include "Watcher.h"


BEGIN_NAMESPACE_YM_SAT

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
  ymuint
  variable_num() const;

  /// @brief 制約節の数を得る．
  ymuint
  clause_num() const;

  /// @brief 二項制約節の数を得る．
  ymuint
  bin_clause_num() const;

  /// @brief 制約節のリテラルの総数を得る．
  ymuint
  literal_num() const;

  /// @brief 学習節の数を得る．
  ymuint
  learnt_clause_num() const;

  /// @brief 二項学習節の数を得る．
  ymuint
  learnt_bin_clause_num() const;

  /// @brief 学習節のリテラルの総数を得る．
  ymuint
  learnt_literal_num() const;

  /// @brief 制約節を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < clause_num() )
  /// @return 制約節を返す．
  const SatClause*
  clause(ymuint pos) const;

  /// @brief 二項制約節の内容を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < bin_clause_num() )
  /// @param[out] lit0, lit1 リテラル
  void
  bin_clause(ymuint pos,
	     SatLiteral& lit0,
	     SatLiteral& lit1) const;


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
  SatVarId
  new_variable(bool decision);

  /// @brief 変数に関する領域を実際に確保する．
  void
  alloc_var();


public:
  //////////////////////////////////////////////////////////////////////
  // 節の追加に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 条件リテラルのリストを設定する．
  /// @param[in] lits リテラルのベクタ
  void
  set_conditional_literals(const vector<SatLiteral>& lits);

  /// @brief 条件リテラルのリストを設定する．
  /// @param[in] lit_num リテラル数
  /// @param[in] lits リテラルの配列
  void
  set_conditional_literals(ymuint lit_num,
			   const SatLiteral* lits);

  /// @brief 節を追加する．
  /// @param[in] lits リテラルのベクタ
  void
  add_clause(const vector<SatLiteral>& lits);

  /// @brief 節を追加する．
  /// @param[in] lit_num リテラル数
  /// @param[in] lits リテラルの配列
  void
  add_clause(ymuint lit_num,
	     const SatLiteral* lits);

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
  /// @param[in] var 変数
  SatBool3
  eval(SatVarId var) const;

  /// @brief リテラルの評価を行う．
  /// @param[in] lit リテラル
  ///
  /// 肯定のリテラルの場合は変数の eval() 結果と同じ．
  /// 否定のリテラルの場合は結果を反転する．
  SatBool3
  eval(SatLiteral lit) const;

  /// @brief バックトラック前の値を得る．
  SatBool3
  prev_val(SatVarId var) const;

  /// @brief モデルを得る．
  /// @param[out] model 割り当て結果を格納する配列
  void
  get_model(vector<SatBool3>& model);

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
  ymuint
  last_assign() const;

  /// @brief 割り当てリストの pos 番めの要素を得る．
  /// @param[in] pos 位置番号 ( 0 <= pos < last_assign() )
  SatLiteral
  get_assign(ymuint pos) const;

  /// @brief 変数の decision level を返す．
  /// @param[in] var 変数番号
  int
  decision_level(SatVarId var) const;


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
	vector<SatBool3>& model,
	Controller& controller,
	Analyzer& analyzer,
	Selecter& selecter);

  /// @brief 変数の割り当て理由を返す．
  /// @param[in] var 変数
  SatReason
  reason(SatVarId var) const;

  /// @brief 次に割り当てる変数を取り出す．
  ///
  /// アクティビティが最大で未割り当ての変数を返す．
  SatVarId
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

#if 0 // forget_larnt_clause() で使うかも．
  /// @brief 変数のアクティビティを初期化する．
  void
  reset_activity();
#endif

  /// @brief 与えられた変数のリストからヒープ木を構成する．
  void
  build(const vector<SatVarId>& var_list);

  /// @brief リスタート回数を返す．
  ymuint64
  restart_num() const;

  /// @brief 総矛盾数を返す．
  ymuint64
  conflict_num() const;

  /// @brief 総分岐数を返す．
  ymuint64
  decision_num() const;

  /// @brief 総インプリケーション数を返す．
  ymuint64
  propagation_num() const;

  /// @brief 矛盾回数の制限値を得る．
  ymuint64
  conflict_limit() const;

  /// @brief 学習節の制限値を得る．
  ymuint64
  learnt_limit() const;

  /// @brief conflict_limit の最大値を返す．
  ymuint64
  max_conflict() const;

  /// @brief conflict_limit の最大値を設定する．
  /// @param[in] val 設定する値
  /// @return 以前の設定値を返す．
  ymuint64
  set_max_conflict(ymuint64 val);

  /// @brief 矛盾回数の制限値を設定する．
  /// @param[in] limit 設定する値
  void
  set_conflict_limit(ymuint64 limit);

  /// @brief 学習節の制限値を設定する．
  /// @param[in] limit 設定する値
  void
  set_learnt_limit(ymuint64 limit);

  /// @brief 現在の内部状態を得る．
  /// @param[out] stats 状態を格納する構造体
  void
  get_stats(SatStats& stats) const;

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
  alloc_lits(ymuint lit_num);

  /// @brief add_clause() の下請け関数
  ///
  /// リテラルの実体は mTmpLits[] に入れておく．
  void
  add_clause_sub(ymuint lit_num);

  /// @brief 新しい節を生成する．
  /// @param[in] lit_num リテラル数
  /// @param[in] learnt 学習節のとき true とするフラグ
  ///
  /// リテラルは mTmpLits[] に入れておく．
  SatClause*
  new_clause(ymuint lit_num,
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
  add_var(SatVarId var);

  /// @brief 変数を再びヒープに追加する．
  /// @param[in] var 追加する変数
  void
  push(SatVarId var);

  /// @brief 要素が空の時 true を返す．
  bool
  empty() const;

  /// @brief アクティビティ最大の変数番号を取り出す．
  /// @note 該当の変数はヒープから取り除かれる．
  ymuint
  pop_top();

  /// @brief 引数の位置にある要素を適当な位置まで沈めてゆく
  /// @param[in] pos 対象の要素の位置
  void
  move_down(ymuint pos);

  /// @brief 引数の位置にある要素を適当な位置まで上げてゆく
  /// @param[in] vindex 対象の変数番号
  void
  move_up(ymuint vindex);

  /// @brief 変数を配列にセットする．
  /// @param[in] vindex 対象の変数番号
  /// @param[in] pos 位置
  /// @note mHeap と mHeapPos の一貫性を保つためにはこの関数を使うこと．
  void
  set(ymuint vindex,
      ymuint pos);

  /// @brief 左の子供の位置を計算する
  /// @param[in] pos 親の位置
  static
  ymuint
  left(ymuint pos);

  /// @brief 右の子供の位置を計算する．
  /// @param[in] pos 親の位置
  static
  ymuint
  right(ymuint pos);

  /// @brief 親の位置を計算する．
  /// @param[in] pos 子供の位置
  /// @note 左の子供でも右の子供でも同じ
  static
  ymuint
  parent(ymuint pos);

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

  // SatClause のメモリ領域確保用のアロケータ
  FragAlloc mAlloc;

  // 正常の時に true となっているフラグ
  bool mSane;

  // 条件リテラルのリスト
  vector<SatLiteral> mCondLits;

  // 制約節の配列
  vector<SatClause*> mConstrClauseList;

  // 二項節を表すリテラルのリスト
  vector<BinClause> mConstrBinList;

  // 制約節の総リテラル数 (二項制約節も含む)
  ymuint64 mConstrLitNum;

  // 学習節の配列
  vector<SatClause*> mLearntClauseList;

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

  // 値割り当てを保持するリスト
  AssignList mAssignList;

  // 前回の sweep 時の割り当て数
  ymuint64 mSweep_assigns;

  // 前回の sweep 時のリテラル数
  ymint64 mSweep_props;

  // add_clause で一時的に利用するリテラル配列
  SatLiteral* mTmpLits;

  // mTmpLits のサイズ
  ymuint32 mTmpLitsSize;

  // 矛盾の解析時にテンポラリに使用される節
  SatClause* mTmpBinClause;

  // ヒープ上の位置の配列
  // サイズは mVarSize
  ymint32* mHeapPos;

  // アクティビティの配列
  // サイズは mVarSize
  double* mActivity;

  // ヒープ用の配列
  // サイズは mVarSize
  ymuint32* mHeap;

  // ヒープの要素数
  ymuint32 mHeapNum;

  // 動作フラグ
  bool mGoOn;

  // リスタート回数
  ymuint64 mRestartNum;

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

  // mTimer を使うとき true にするフラグ
  bool mTimerOn;

  // 時間計測器
  StopWatch mTimer;

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
ymuint
CoreMgr::variable_num() const
{
  return mVarNum;
}

// @brief 制約節の数を得る．
inline
ymuint
CoreMgr::clause_num() const
{
  return mConstrClauseList.size();
}

// @brief 二項制約節の数を得る．
inline
ymuint
CoreMgr::bin_clause_num() const
{
  return mConstrBinList.size();
}

// @brief 制約節のリテラルの総数を得る．
inline
ymuint
CoreMgr::literal_num() const
{
  return mConstrLitNum;
}

// @brief 学習節の数を得る．
inline
ymuint
CoreMgr::learnt_clause_num() const
{
  return mLearntClauseList.size();
}

// @brief 二項学習節の数を得る．
inline
ymuint
CoreMgr::learnt_bin_clause_num() const
{
  return mLearntBinNum;
}

// @brief 学習節のリテラルの総数を得る．
inline
ymuint
CoreMgr::learnt_literal_num() const
{
  return mLearntLitNum;
}

// @brief 制約節を得る．
// @param[in] pos 位置番号 ( 0 <= pos < clause_num() )
// @return 制約節を返す．
inline
const SatClause*
CoreMgr::clause(ymuint pos) const
{
  ASSERT_COND( pos < clause_num() );
  return mConstrClauseList[pos];
}

// @brief 二項制約節の内容を得る．
// @param[in] pos 位置番号 ( 0 <= pos < bin_clause_num() )
// @param[out] lit0, lit1 リテラル
inline
void
CoreMgr::bin_clause(ymuint pos,
		    SatLiteral& lit0,
		    SatLiteral& lit1) const
{
  ASSERT_COND( pos < bin_clause_num() );
  lit0 = mConstrBinList[pos].mLit0;
  lit1 = mConstrBinList[pos].mLit1;
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
CoreMgr::add_var(SatVarId var)
{
  ymuint vindex = var.val();
  set(vindex, mHeapNum);
  mActivity[vindex] = 0.0;
  ++ mHeapNum;
}

// watcher list を得る．
inline
WatcherList&
CoreMgr::watcher_list(SatLiteral lit)
{
  ymuint index = lit.index();
  ASSERT_COND( index < mVarNum * 2 );
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
CoreMgr::eval(SatVarId var) const
{
  ymuint vindex = var.val();
  ASSERT_COND( vindex < mVarNum );
  ymuint8 x = mVal[vindex] & 3U;
  return conv_to_Bool3(x);
}

// @brief リテラルの評価を行う．
// @param[in] lit リテラル
inline
SatBool3
CoreMgr::eval(SatLiteral lit) const
{
  ymuint index = lit.index();
  ymuint vindex = index >> 1;
  ASSERT_COND( vindex < mVarNum );
  ymuint x = mVal[vindex] & 3U;
  ymuint inv = index & 1U;
  int d = 1 - (inv * 2);
  return static_cast<SatBool3>((static_cast<int>(x) - 1) * d);
}

// @brief バックトラック前の値を得る．
inline
SatBool3
CoreMgr::prev_val(SatVarId var) const
{
  ymuint vindex = var.val();
  ASSERT_COND( vindex < mVarNum );
  ymuint8 x = ((mVal[vindex] >> 2)) & 3U;
  return conv_to_Bool3(x);
}

// 値の割当てを行う．
inline
void
CoreMgr::assign(SatLiteral lit,
		SatReason reason)
{
  ymuint lindex = lit.index();
  ymuint vindex = lindex / 2;
  ymuint inv = lindex & 1U;
  ymuint8 x = 2 - inv * 2;
  ASSERT_COND( vindex < mVarNum );
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
ymuint
CoreMgr::last_assign() const
{
  return mAssignList.size() - 1;
}

// @brief 割り当てリストの pos 番めの要素を得る．
// @param[in] pos 位置番号
inline
SatLiteral
CoreMgr::get_assign(ymuint pos) const
{
  return mAssignList.get(pos);
}

// @brief 変数の decision level を返す．
// @param[in] var 変数番号
inline
int
CoreMgr::decision_level(SatVarId var) const
{
  ymuint vindex = var.val();
  ASSERT_COND( vindex < mVarNum );
  return mDecisionLevel[vindex];
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
CoreMgr::reason(SatVarId var) const
{
  ymuint vindex = var.val();
  ASSERT_COND( vindex < mVarNum );
  return mReason[vindex];
}

// @brief clase が含意の理由になっているか調べる．
inline
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
CoreMgr::push(SatVarId var)
{
  ymuint vindex = var.val();
  if ( mHeapPos[vindex] == -1 ) {
    ymuint pos = mHeapNum;
    ++ mHeapNum;
    set(vindex, pos);
    move_up(pos);
  }
}

// @brief もっともアクティビティの高い変数を返す．
inline
ymuint
CoreMgr::pop_top()
{
  ASSERT_COND(mHeapNum > 0 );

  ymuint ans = mHeap[0];
  mHeapPos[ans] = -1;
  -- mHeapNum;
  if ( mHeapNum > 0 ) {
    ymuint vindex = mHeap[mHeapNum];
    set(vindex, 0);
    move_down(0);
  }
  return ans;
}

// 引数の位置にある要素を適当な位置まで上げてゆく
inline
void
CoreMgr::move_up(ymuint pos)
{
  ymuint vindex = mHeap[pos];
  double val = mActivity[vindex];
  while ( pos > 0 ) {
    ymuint pos_p = parent(pos);
    ymuint vindex_p = mHeap[pos_p];
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
CoreMgr::set(ymuint vindex,
	     ymuint pos)
{
  mHeap[pos] = vindex;
  mHeapPos[vindex] = pos;
}

// @brief 左の子供の位置を計算する
inline
ymuint
CoreMgr::left(ymuint pos)
{
  return pos + pos + 1;
}

// @brief 右の子供の位置を計算する．
inline
ymuint
CoreMgr::right(ymuint pos)
{
  return pos + pos + 2;
}

// @brief 親の位置を計算する．
inline
ymuint
CoreMgr::parent(ymuint pos)
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
ymuint64
CoreMgr::restart_num() const
{
  return mRestartNum;
}

// @brief 総矛盾数を返す．
inline
ymuint64
CoreMgr::conflict_num() const
{
  return mConflictNum;
}

// @brief 総分岐数を返す．
inline
ymuint64
CoreMgr::decision_num() const
{
  return mDecisionNum;
}

// @brief 総インプリケーション数を返す．
inline
ymuint64
CoreMgr::propagation_num() const
{
  return mPropagationNum;
}

// @brief 矛盾回数の制限値を得る．
inline
ymuint64
CoreMgr::conflict_limit() const
{
  return mConflictLimit;
}

// @brief 学習節の制限値を得る．
inline
ymuint64
CoreMgr::learnt_limit() const
{
  return mLearntLimit;
}

// @brief conflict_limit の最大値を返す．
inline
ymuint64
CoreMgr::max_conflict() const
{
  return mMaxConflict;
}

// @brief conflict_limit の最大値を設定する．
// @param[in] val 設定する値
// @return 以前の設定値を返す．
inline
ymuint64
CoreMgr::set_max_conflict(ymuint64 val)
{
  ymuint64 old_val = mMaxConflict;
  mMaxConflict = val;
  return old_val;
}

// @brief 矛盾回数の制限値を設定する．
// @param[in] limit 設定する値
inline
void
CoreMgr::set_conflict_limit(ymuint64 limit)
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
CoreMgr::set_learnt_limit(ymuint64 limit)
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
    mTimer.stop();
    mTimer.reset();
    mTimer.start();
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
    mTimer.stop();
  }
}

END_NAMESPACE_YM_SAT

#endif // COREMGR_H
