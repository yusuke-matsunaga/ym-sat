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
///
/// 効率的な処理を行うために WatcherList の要素として２重のリンクポインタ
/// を持つ．
//////////////////////////////////////////////////////////////////////
class Watcher :
  public Reason
{
  friend class WatcherList;

public:

  /// @brief コンストラクタ
  Watcher()
  {
    mPrev = mNext = this;
  }

  /// @brief コンストラクタ
  explicit
  Watcher(
    Reason src ///< [in] もととなる SatReason
  ) : Reason{src}
  {
    mPrev = mNext = this;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の要素を得る．
  Watcher*
  next() const
  {
    return mNext;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 前の要素を刺すリンクポインタ
  Watcher* mPrev;

  // 次の要素を指すリンクポインタ
  Watcher* mNext;

};


//////////////////////////////////////////////////////////////////////
/// @class WatcherList Watcher.h "Watcher.h"
/// @brief Watcher のリストを表すクラス
///
/// 実体は先頭/末尾を表すダミーの Watcher を持つ．
//////////////////////////////////////////////////////////////////////
class WatcherList
{
public:

  /// @brief コンストラクタ
  WatcherList() = default;

  /// @brief デストラクタ
  ~WatcherList() = default;


public:

  /// @brief クリアする．
  void
  clear()
  {
    mSize = 0;
    mDummy.mNext = mDummy.mPrev = &mDummy;
  }

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mSize;
  }

  /// @brief 要素を追加する．
  void
  add(
    Watcher* elem ///< [in] 追加する要素
  )
  {
    ++ mSize;
    auto last = mDummy.mPrev;
    last->mNext = elem;
    elem->mPrev = last;
    elem->mNext = &mDummy;
    mDummy.mPrev = elem;
  }

  /// @brief 要素を削除する．
  void
  del(
    Watcher* elem
  )
  {
    -- mSize;
    auto prev = elem->mPrev;
    auto next = elem->mNext;
    prev->mNext = next;
    next->mPrev = prev;
    elem->mPrev = elem->mNext = elem;
  }

  /// @brief 先頭の要素を返す．
  Watcher*
  begin() const
  {
    return mDummy.mNext;
  }

  /// @brief 末尾の要素を返す．
  Watcher*
  end()
  {
    return &mDummy;
  }

#if 0
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
    auto rpos = mArray.begin() + from_pos;
    auto epos = mArray.begin() + end_pos;
    auto wpos = mArray.begin() + to_pos;
    for ( ; rpos != epos; ++ rpos, ++ wpos ) {
      *wpos = *rpos;
    }
    mArray.erase(wpos, mArray.end());
  }

  /// @brief 要素を切り詰める．
  void
  erase(
    SizeType num ///< [in] 切り詰めるサイズ
  )
  {
    auto pos = mArray.begin() + num;
    mArray.erase(pos, mArray.end());
  }

  /// @brief from の内容を移す．
  ///
  /// from は空になる．
  void
  move(
    WatcherList& from ///< [in] もとのリスト
  )
  {
    std::swap(mArray, from.mArray);
  }
#endif


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

#if 0
  // 配列
  vector<Watcher> mArray;
#else
  // 要素数
  SizeType mSize{0};

  // 先頭/末尾のダミー
  Watcher mDummy;
#endif

};

END_NAMESPACE_YM_SAT

#endif // YMSAT_WATCHER_H
