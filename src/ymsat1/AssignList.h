﻿#ifndef YMSAT1_ASSIGNLIST_H
#define YMSAT1_ASSIGNLIST_H

/// @file AssignList.h
/// @brief AssignList のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym_sat1.h"
#include "ym/ym_sat.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT1

//////////////////////////////////////////////////////////////////////
/// @class AssignList AssignList.h "AssignList.h"
/// @brief 割り当ての記録を保持するクラス
///
/// 基本的にはスタックとして機能するが，一つ一つもどるのではなく，
/// 直前の決定(他の割り当てからの含意ではない値の割り当て)までを
/// 単位として戻るので，その位置を覚えておく補助的な配列を用意する．
/// ここではそれを marker と呼んでいる．
/// set_marker() を呼ぶと今のスタックの位置を marker として記憶し，
/// 現在の decision_level を一つ増やす．
/// marker(level) で level に対応した(正確には level の割り当てを行う
/// 直前の)スタックの位置を返す．level 以降の割り当てを無効にする場合
/// には marker(level) までの割り当てを取り消せばよい．
/// ただし marker は AssignList の外部からはアクセス不可となっている．
/// 可能な操作としては backtrack(level) で指定したレベルまでスタックの
/// 底を移動させ，get_prev() でスタックの上から順に割り当てを取り出す
/// 処理を has_elem() が false になるまで繰り返すことである．
///
/// 一方，含意操作中には AssignList は未処理の割り当てを入れておくキュー
/// として機能する．そのため，読み出し位置を記録する mListHead という
/// メンバ変数を持っている．ただし，この変数は外部からはアクセス不可で
/// get_next()(次に進める), skip_all()(末尾に進める), backtrack()(指定
/// したレベルまで戻る) の3つの関数でのみ値が更新される．
///
/// 最後に，AssignList の任意の位置にアクセスするための関数として，
/// get(pos) が用意されている．この関数を使うときは事前に size() で
/// アクセス可能なサイズを得ておくこと．
/// AssignList には一つの変数につき高々一回しか積まれることはないので，
/// AssignList の最大サイズは変数の数となる．また，問題が SAT の時，
/// 全ての変数に値が割り当てられるので，その時の AssignList のサイズは
/// 変数の数となる．よって，新しい変数を確保する度に reserve() を呼んで
/// 充分な大きさに拡張しておく．そうしておくことで put() などの処理中
/// に配列のサイズを気にする必要がなくなる．
//////////////////////////////////////////////////////////////////////
class AssignList
{
public:

  /// @brief コンストラクタ
  AssignList();

  /// @brief デストラクタ
  ~AssignList();


public:
  //////////////////////////////////////////////////////////////////////
  // 領域確保に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 必要なサイズを指定する．
  void
  reserve(int req_size);


public:
  //////////////////////////////////////////////////////////////////////
  // 割り当ての追加/取り出しに関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 割り当てを追加する．
  /// @note ここでは領域の拡張を行わない．それ以前に reserve() で適切な
  /// サイズが指定されている必要がある．
  void
  put(SatLiteral lit);

  /// @brief 記録されている割り当ての要素数を得る．
  int
  size() const;

  /// @brief 読み出す要素があるとき true を返す．
  bool
  has_elem() const;

  /// @brief 次の要素を返す．
  /// @return 今の読み出し位置の要素を返す．
  /// @note 読み出し位置は一つ進む
  SatLiteral
  get_next();

  /// @brief 前の要素を返す．
  /// @return 書き込み位置の一つ手前の要素を返す．
  /// @note 書き込み位置は一つもどる．
  SatLiteral
  get_prev();

  /// @brief 全てを読み出したことにする．
  /// @note 読み出し位置を書き込み位置にする．
  void
  skip_all();

  /// @brief pos 番目の要素を得る．
  SatLiteral
  get(int pos) const;


public:
  //////////////////////////////////////////////////////////////////////
  // decision level と marker の操作に関する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 現在のレベルを返す．
  int
  cur_level() const;

  /// @brief 現在の位置をマーカーに登録する．
  void
  set_marker();

  /// @brief level の割り当てを行う直前の状態にもどす．
  /// @param[in] level 対象のレベル
  /// @note 具体的には読み出し位置が level のマーカとなり，現在のレベルが
  /// level となる．
  void
  backtrack(int level);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // リストの実際の容量
  int mSize;

  // 値割り当てを保持するリスト(配列)
  SatLiteral* mList;

  // 書き込み位置
  int mTail;

  // 読み出し位置
  int mHead;

  // 各 decision-level ごとの marker の配列
  int* mMarker;

  // 現在の decision_level
  int mCurLevel;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief 割り当てを追加する．
inline
void
AssignList::put(SatLiteral lit)
{
  ASSERT_COND( mTail < mSize );
  mList[mTail ++] = lit;
}

// @brief 記録されている割り当ての要素数を得る．
inline
int
AssignList::size() const
{
  return mTail;
}

// @brief 読み出す要素があるとき true を返す．
inline
bool
AssignList::has_elem() const
{
  return mHead < mTail;
}

// @brief 次の要素を返す．
inline
SatLiteral
AssignList::get_next()
{
  return get(mHead ++);
}

// @brief 前の要素を返す．
inline
SatLiteral
AssignList::get_prev()
{
  return get(-- mTail);
}

// @brief 全てを読み出したことにする．
inline
void
AssignList::skip_all()
{
  mHead = mTail;
}

// @brief pos 番目の要素を得る．
inline
SatLiteral
AssignList::get(int pos) const
{
  ASSERT_COND( pos < mSize );
  return mList[pos];
}

// @brief 現在のレベルを返す．
inline
int
AssignList::cur_level() const
{
  return mCurLevel;
}

// @brief 現在の位置をマーカーに登録する．
inline
void
AssignList::set_marker()
{
  ASSERT_COND( mCurLevel < mSize );
  mMarker[mCurLevel ++] = mTail;
}

// @brief level の割り当てを行う直前の状態にもどす．
inline
void
AssignList::backtrack(int level)
{
  ASSERT_COND( level < static_cast<int>(mSize) );
  mHead = mMarker[level];
  mCurLevel = level;
}

END_NAMESPACE_YM_SAT1

#endif // LIBYM_YMYMLOGIC_SAT_YMSAT_ASSIGNLIST_H
