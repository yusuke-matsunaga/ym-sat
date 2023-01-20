#ifndef VARHEAP_H
#define VARHEAP_H

/// @file VarHeap.h
/// @brief VarHeap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018, 203 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class VarHeap VarHeap.h "VarHeap.h"
/// @brief 変数のヒープ木
///
/// mActivity[] の値に基づいて整列させる．
//////////////////////////////////////////////////////////////////////
class VarHeap
{
public:

  /// @brief コンストラクタ
  VarHeap();

  /// @brief デストラクタ
  ~VarHeap();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief アクティビティの低減率を設定する．
  void
  set_decay(
    double decay ///< [in] 低減率
  )
  {
    mVarDecay = decay;
  }

  /// @brief 変数のアクティビティを増加させる．
  void
  bump_var_activity(
    SatVarId var ///< [in] 変数番号
  );

  /// @brief 変数のアクティビティを定率で減少させる．
  void
  decay_var_activity();

  /// @brief 空にする．
  void
  clear()
  {
    mHeapNum = 0;
  }

  /// @brief size 個の要素を格納出来るだけの領域を確保する．
  void
  alloc_var(
    SizeType size ///< [in] 必要なサイズ
  );

  /// @brief 要素が空の時 true を返す．
  bool
  empty() const
  {
    return mHeapNum == 0;
  }

  /// @brief 変数を始めてヒープに追加する．
  void
  add_var(
    SatVarId var ///< [in] 追加する変数
  )
  {
    set(var, mHeapNum);
    mActivity[var] = 0.0;
    ++ mHeapNum;
}

  /// @brief 変数を再びヒープに追加する．
  void
  push(
    SatVarId var ///< [in] 追加する変数
  )
  {
    if ( mHeapPos[var] == -1 ) {
      auto pos = mHeapNum;
      ++ mHeapNum;
      set(var, pos);
      move_up(pos);
    }
  }

  /// @brief アクティビティ最大の変数番号を取り出す．
  ///
  /// 該当の変数はヒープから取り除かれる．
  SatVarId
  pop_top()
  {
    ASSERT_COND(mHeapNum > 0 );

    auto ans = mHeap[0];
    mHeapPos[ans] = -1;
    -- mHeapNum;
    if ( mHeapNum > 0 ) {
      auto vindex = mHeap[mHeapNum];
      set(vindex, 0);
      move_down(0);
    }
    return ans;
  }

  /// @brief 変数のアクティビティを返す．
  double
  activity(
    SatVarId var ///< [in] 対象の変数
  ) const
  {
    return mActivity[var];
  }

  /// @brief 変数のアクティビティを初期化する．
  void
  reset_activity();

  /// @brief 与えられた変数のリストからヒープ木を構成する．
  void
  build(
    const vector<SatVarId>& var_list
  );

  /// @brief 内容を出力する
  void
  dump(
    ostream& s ///< [in] 出力先のストリーム
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 引数の位置にある要素を適当な位置まで沈めてゆく
  void
  move_down(
    SizeType pos ///< [in] 対象の要素の位置
  );

  /// @brief 引数の位置にある要素を適当な位置まで上げてゆく
  void
  move_up(
    SizeType pos ///< [in] 対象の要素の位置
  )
  {
    auto vindex = mHeap[pos];
    double val = mActivity[vindex];
    while ( pos > 0 ) {
      auto pos_p = parent(pos);
      auto vindex_p = mHeap[pos_p];
      double val_p = mActivity[vindex_p];
      if ( val_p >= val ) {
	break;
      }
      set(vindex, pos_p);
      set(vindex_p, pos);
      pos = pos_p;
    }
  }

  /// @brief 変数を配列にセットする．
  ///
  /// mHeap と mHeapPos の一貫性を保つためにはこの関数を使うこと．
  void
  set(
    SatVarId vindex, ///< [in] 対象の変数番号
    SizeType pos     ///< [in] 位置
  )
  {
    mHeap[pos] = vindex;
    mHeapPos[vindex] = pos;
  }

  /// @brief 左の子供の位置を計算する
  static
  SizeType
  left(
    SizeType pos ///< [in] 親の位置
  )
  {
    return pos + pos + 1;
  }

  /// @brief 右の子供の位置を計算する．
  static
  SizeType
  right(
    SizeType pos ///< [in] 親の位置
  )
  {
    return pos + pos + 2;
  }

  /// @brief 親の位置を計算する．
  ///
  /// 左の子供でも右の子供でも同じ
  static
  SizeType
  parent(
    SizeType pos ///< [in] 子供の位置
  )
  {
    return (pos - 1) >> 1;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数のアクティビティの増加量
  double mVarBump{1.0};

  // 変数のアクティビティの減衰量
  double mVarDecay{0.95};

  // 変数の数
  SizeType mVarNum{0};

  // 変数関係の配列のサイズ
  SizeType mVarSize{0};

  // ヒープ上の位置の配列
  // サイズは mVarSize
  SizeType* mHeapPos{nullptr};

  // アクティビティ
  // サイズは mVarSize
  double* mActivity{nullptr};

  // ヒープ用の配列
  // サイズは mVarSize
  SatVarId* mHeap{nullptr};

  // ヒープの要素数
  SizeType mHeapNum{0};

};

END_NAMESPACE_YM_SAT

#endif // VARHEAP_H
