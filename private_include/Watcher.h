#ifndef YMSAT_WATCHER_H
#define YMSAT_WATCHER_H

/// @file Watcher.h
/// @brief Watcher のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "Reason.h"


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
  public Reason
{
public:

  /// @brief コンストラクタ
  Watcher() = default;

  /// @brief コンストラクタ
  explicit
  Watcher(
    Reason src ///< [in] もととなる SatReason
  ) : Reason{src}
  {
  }

};


//////////////////////////////////////////////////////////////////////
/// @class WatcherList Watcher.h "Watcher.h"
/// @brief Watcher のリストを表すクラス
//////////////////////////////////////////////////////////////////////
class WatcherList
{
public:

  /// @brief コンストラクタ
  WatcherList() = default;

  /// @brief デストラクタ
  ~WatcherList()
  {
    delete [] mArray;
  }


public:

  /// @brief クリアする．
  void
  clear()
  {
    mNum = 0;
  }

  /// @brief 要素数を返す．
  SizeType
  num() const
  {
    return mNum;
  }

  /// @brief 要素を追加する．
  void
  add(
    Watcher elem ///< [in] 追加する要素
  )
  {
    if ( mNum >= mSize ) {
      expand(mNum + 1);
    }
    set_elem(mNum, elem);
    ++ mNum;
  }

  /// @brief 要素を削除する．
  ///
  /// 線形走査を行っている．
  void
  del(
    Watcher elem
  )
  {
    SizeType pos = 0;
    for ( ; pos < mNum; ++ pos ) {
      auto w = mArray[pos];
      if ( w == elem ) {
	break;
      }
    }
    -- mNum;
    for ( ; pos < mNum; ++ pos ) {
      mArray[pos] = mArray[pos + 1];
    }
  }

  /// @brief pos 番目の要素を返す．
  const Watcher&
  elem(
    SizeType pos ///< [in] 位置
  ) const
  {
    return mArray[pos];
  }

  /// @brief pos 番目の要素を設定する．
  void
  set_elem(
    SizeType pos, ///< [in] 位置
    const Watcher& elem  ///< [in] 要素
  )
  {
    mArray[pos] = elem;
  }

  /// @brief 要素を移動する．
  void
  move_elem(
    SizeType from_pos, ///< [in] 先頭の位置
    SizeType end_pos,  ///< [in] 末尾の位置
    SizeType to_pos    ///< [in] 移動先
  )
  {
    for ( SizeType pos = from_pos; pos < end_pos; ++ pos, ++ to_pos ) {
      mArray[to_pos] = mArray[pos];
    }
    mNum = to_pos;
  }

  /// @brief 要素を切り詰める．
  void
  erase(
    SizeType num ///< [in] 切り詰めるサイズ
  )
  {
    mNum = num;
  }

  /// @brief from の内容を移す．
  ///
  /// from は空になる．
  void
  move(
    WatcherList& from ///< [in] もとのリスト
  )
  {
    mSize = from.mSize;
    mNum = from.mNum;
    mArray = from.mArray;
    from.mSize = 0;
    from.mNum = 0;
    from.mArray = nullptr;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 配列を拡張する．
  void
  expand(
    SizeType req_size ///< [in] 拡張するサイズ
  )
  {
    if ( mSize < req_size ) {
      auto old_size = mSize;
      auto old_array = mArray;
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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 配列のサイズ
  SizeType mSize{0};

  // 要素数
  SizeType mNum{0};

  // 配列
  Watcher* mArray{nullptr};

};

END_NAMESPACE_YM_SAT

#endif // YMSAT_WATCHER_H
