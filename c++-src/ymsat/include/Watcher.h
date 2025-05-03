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

  /// @brief Clause* を指定したコンストラクタ
  explicit
  Watcher(
    Clause* clause ///< [in] 節のポインタ
  ) : Reason(clause)
  {
  }

  /// @brief Literal を指定したコンストラクタ
  explicit
  Watcher(
    Literal lit ///< [in] リテラル
  ) : Reason(lit)
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

  /// @brief ムーブコンストラクタ
  WatcherList(
    WatcherList&& src
  ) : mArray{std::move(src.mArray)}
  {
  }

  /// @brief デストラクタ
  ~WatcherList() = default;


public:

  /// @brief クリアする．
  void
  clear()
  {
    mArray.clear();
  }

  /// @brief 要素数を返す．
  SizeType
  size() const
  {
    return mArray.size();
  }

  /// @brief 要素を追加する．
  void
  add(
    const Watcher& w ///< [in] 追加する要素
  )
  {
    mArray.push_back(w);
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
    SizeType pos,    ///< [in] 位置
    const Watcher& w ///< [in] 要素
  )
  {
    mArray[pos] = w;
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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 配列
  vector<Watcher> mArray;

};

END_NAMESPACE_YM_SAT

#endif // YMSAT_WATCHER_H
