#ifndef YMSAT1_VARHEAP_H
#define YMSAT1_VARHEAP_H

/// @file VarHeap.h
/// @brief VarHeap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym_sat1.h"
#include "ym/ym_sat.h"
#include "ym/SatVarId.h"


BEGIN_NAMESPACE_YM_SAT1

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
  /// @param[in] decay 低減率
  void
  set_decay(double decay);

  /// @brief 変数のアクティビティを増加させる．
  /// @param[in] var 変数番号
  void
  bump_var_activity(SatVarId var);

  /// @brief 変数のアクティビティを定率で減少させる．
  void
  decay_var_activity();

  /// @brief 空にする．
  void
  clear();

  /// @brief size 個の要素を格納出来るだけの領域を確保する．
  /// @param[in] size 必要なサイズ
  void
  alloc_var(int size);

  /// @brief 要素が空の時 true を返す．
  bool
  empty() const;

  /// @brief 変数を始めてヒープに追加する．
  /// @param[in] var 追加する変数
  void
  add_var(SatVarId var);

  /// @brief 変数を再びヒープに追加する．
  /// @param[in] var 追加する変数
  void
  push(SatVarId var);

  /// @brief アクティビティ最大の変数番号を取り出す．
  /// @note 該当の変数はヒープから取り除かれる．
  int
  pop_top();

  /// @brief 変数のアクティビティを返す．
  /// @param[in] var 対象の変数
  double
  activity(SatVarId var) const;

  /// @brief 変数のアクティビティを初期化する．
  void
  reset_activity();

  /// @brief 与えられた変数のリストからヒープ木を構成する．
  void
  build(const vector<SatVarId>& var_list);

  /// @brief 内容を出力する
  /// @param[in] s 出力先のストリーム
  void
  dump(ostream& s) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

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


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数のアクティビティの増加量
  double mVarBump;

  // 変数のアクティビティの減衰量
  double mVarDecay;

  // 変数の数
  int mVarNum;

  // 変数関係の配列のサイズ
  int mVarSize;

  // ヒープ上の位置の配列
  // サイズは mVarSize
  int* mHeapPos;

  // アクティビティ
  // サイズは mVarSize
  double* mActivity;

  // ヒープ用の配列
  // サイズは mVarSize
  int* mHeap;

  // ヒープの要素数
  int mHeapNum;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief アクティビティの低減率を設定する．
inline
void
VarHeap::set_decay(double decay)
{
  mVarDecay = decay;
}

// @brief 空にする．
inline
void
VarHeap::clear()
{
  mHeapNum = 0;
}

// @brief 要素が空の時 true を返す．
inline
bool
VarHeap::empty() const
{
  return mHeapNum == 0;
}

// @brief 変数を始めてヒープに追加する．
// @param[in] var 変数番号
inline
void
VarHeap::add_var(SatVarId var)
{
  int vindex = var.val();
  set(vindex, mHeapNum);
  mActivity[vindex] = 0.0;
  ++ mHeapNum;
}

// @brief 変数のアクティビティを返す．
// @param[in] var 変数番号
inline
double
VarHeap::activity(SatVarId var) const
{
  return mActivity[var.val()];
}

// @brief 要素を追加する．
inline
void
VarHeap::push(SatVarId var)
{
  int vindex = var.val();
  if ( mHeapPos[vindex] == -1 ) {
    int pos = mHeapNum;
    ++ mHeapNum;
    set(vindex, pos);
    move_up(pos);
  }
}

// @brief もっともアクティビティの高い変数を返す．
inline
int
VarHeap::pop_top()
{
  ASSERT_COND(mHeapNum > 0 );

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
VarHeap::move_up(int pos)
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
VarHeap::set(int vindex,
	     int pos)
{
  mHeap[pos] = vindex;
  mHeapPos[vindex] = pos;
}

// @brief 左の子供の位置を計算する
inline
int
VarHeap::left(int pos)
{
  return pos + pos + 1;
}

// @brief 右の子供の位置を計算する．
inline
int
VarHeap::right(int pos)
{
  return pos + pos + 2;
}

// @brief 親の位置を計算する．
inline
int
VarHeap::parent(int pos)
{
  return (pos - 1) >> 1;
}

END_NAMESPACE_YM_SAT1

#endif // YMSAT1_VARHEAP_H
