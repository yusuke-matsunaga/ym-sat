#ifndef WATCHER_H
#define WATCHER_H

/// @file Watcher.h
/// @brief Watcher のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"
#include "ym/Alloc.h"
#include "SatReason.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class Watcher ImplInk.h "Watcher.h"
/// @brief watch literal に関連付けられた節のリストを表すクラス
///
/// (a1 + a2 + a3 + ... an) という節があるとする．
/// この時， a1 と a2 が watch literal ならば，~a1 または ~a2
/// の割り当てが起こったときに，この節の watch literal の更新を行う
/// 必要がある．
/// そのような節のリストを作るためのクラス
//////////////////////////////////////////////////////////////////////
class Watcher :
  public SatReason
{
public:

  /// @brief コンストラクタ
  Watcher();

  /// @brief コンストラクタ
  /// @param[in] src もととなる SatReason
  explicit
  Watcher(SatReason src);

};


//////////////////////////////////////////////////////////////////////
/// @class WatcherList Watcher.h "Watcher.h"
/// @brief Watcher のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class WatcherList
{
public:

  /// @brief コンストラクタ
  WatcherList();

  /// @brief デストラクタ
  ~WatcherList();


public:

  /// @brief クリアする．
  void
  clear();

  /// @brief 要素数を返す．
  int
  num() const;

  /// @brief 要素を追加する．
  /// @param[in] elem 追加する要素
  void
  add(Watcher elem);

  /// @brief pos 番目の要素を返す．
  /// @param[in] pos 位置
  Watcher
  elem(int pos) const;

  /// @brief pos 番目の要素を設定する．
  /// @param[in] pos 位置
  /// @param[in] elem 要素
  void
  set_elem(int pos,
	   Watcher elem);

  /// @brief 要素を切り詰める．
  /// @param[in] num 切り詰めるサイズ
  void
  erase(int num);

  /// @brief from の内容を移す．
  /// @param[in] from もとのリスト
  /// @note from は空になる．
  void
  move(WatcherList& from);

  /// @brief このオブジェクトが使っているメモリを回収する．
  /// @note かなり危険な関数．
  /// @note この関数を呼んだらもうこのオブジェクトは使えない．
  void
  finish();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 配列を拡張する．
  /// @param[in] req_size 拡張するサイズ
  void
  expand(int req_size);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 配列のサイズ
  int mSize;

  // 要素数
  int mNum;

  // 配列
  Watcher* mArray;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
inline
Watcher::Watcher()
{
}

// @brief コンストラクタ
// @param[in] src もととなる SatReason
inline
Watcher::Watcher(SatReason src) :
  SatReason(src)
{
}

// @brief コンストラクタ
inline
WatcherList::WatcherList() :
  mSize(0),
  mNum(0),
  mArray(nullptr)
{
}

// @brief デストラクタ
inline
WatcherList::~WatcherList()
{
}

// @brief クリアする．
inline
void
WatcherList::clear()
{
  mNum = 0;
}

inline
void
WatcherList::add(Watcher elem)
{
  if ( mNum >= mSize ) {
    expand(mNum + 1);
  }
  set_elem(mNum, elem);
  ++ mNum;
}

// @brief 要素数を返す．
inline
int
WatcherList::num() const
{
  return mNum;
}

// @brief pos 番目の要素を返す．
inline
Watcher
WatcherList::elem(int pos) const
{
  ASSERT_COND( pos < mNum );
  return mArray[pos];
}

// @brief pos 番目の要素を設定する．
inline
void
WatcherList::set_elem(int pos,
		      Watcher elem)
{
  ASSERT_COND( pos < mSize );
  mArray[pos] = elem;
}

// @brief 要素を切り詰める．
inline
void
WatcherList::erase(int num)
{
  mNum = num;
}

// @brief from の内容を移す．
// @note from は空になる．
inline
void
WatcherList::move(WatcherList& from)
{
  mSize = from.mSize;
  mNum = from.mNum;
  mArray = from.mArray;
  from.mSize = 0;
  from.mNum = 0;
  from.mArray = nullptr;
}

// @brief 配列を拡張する．
inline
void
WatcherList::expand(int req_size)
{
  if ( mSize < req_size ) {
    int old_size = mSize;
    Watcher* old_array = mArray;
    if ( mSize == 0 ) {
      mSize = 4;
    }
    while ( mSize < req_size ) {
      mSize <<= 1;
    }
    mArray = new Watcher[mSize];

    if ( old_size > 0 ) {
      for ( int i = 0; i < old_size; ++ i ) {
	mArray[i] = old_array[i];
      }
      delete [] old_array;
    }
  }
}

// @brief このオブジェクトが使っているメモリを回収する．
// @note かなり危険な関数．
// @note この関数を呼んだらもうこのオブジェクトは使えない．
inline
void
WatcherList::finish()
{
  delete [] mArray;
}

END_NAMESPACE_YM_SAT

#endif // SATREASON_H
