
/// @file VarHeap.cc
/// @brief VarHeap の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "VarHeap.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// VarHeap
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
VarHeap::VarHeap()
{
}

// @brief デストラクタ
VarHeap::~VarHeap()
{
  delete [] mHeapPos;
  delete [] mActivity;
  delete [] mHeap;
}

// @brief size 個の要素を格納出来るだけの領域を確保する．
void
VarHeap::alloc_var(
  SizeType size
)
{
  auto old_var_num = mVarNum;
  auto old_size = mVarSize;
  auto old_heap_pos = mHeapPos;
  auto old_activity = mActivity;
  auto old_heap = mHeap;
  if ( mVarSize == 0 ) {
    mVarSize = 1024;
  }
  while ( mVarSize < size ) {
    mVarSize <<= 1;
  }
  if ( mVarSize != old_size ) {
    mHeapPos = new SizeType[mVarSize];
    mActivity = new double[mVarSize];
    mHeap = new SatVarId[mVarSize];
    for ( SizeType i = 0; i < old_var_num; ++ i ) {
      mHeapPos[i] = old_heap_pos[i];
      mActivity[i] = old_activity[i];
    }
    for ( SizeType i = 0; i < mHeapNum; ++ i ) {
      mHeap[i] = old_heap[i];
    }
    if ( old_size > 0 ) {
      delete [] old_heap_pos;
      delete [] old_activity;
      delete [] old_heap;
    }
  }
  mVarNum = size;
}

// 変数のアクティビティを増加させる．
void
VarHeap::bump_var_activity(
  SatVarId varid
)
{
  double& act = mActivity[varid];
  act += mVarBump;
  if ( act > 1e+100 ) {
    for ( SizeType var = 0; var < mVarNum; ++ var ) {
      mActivity[var] *= 1e-100;
    }
    mVarBump *= 1e-100;
  }
  // pos != -1 と pos != 0 を同時に行うための hack
  int pos = mHeapPos[varid];
  if ( pos > 0 ) {
    move_up(pos);
  }
}

// 変数のアクティビティを定率で減少させる．
void
VarHeap::decay_var_activity()
{
  mVarBump *= (1 / mVarDecay);
}

// @brief 変数のアクティビティを初期化する．
void
VarHeap::reset_activity()
{
  for ( SizeType i = 0; i < mVarSize; ++ i ) {
    mActivity[i] = 0.0;
  }
}

// @brief 与えられた変数のリストからヒープ木を構成する．
void
VarHeap::build(
  const vector<SatVarId>& var_list
)
{
  for ( SizeType i = 0; i < mVarSize; ++ i ) {
    mHeapPos[i] = -1;
  }
  mHeapNum = 0;
  ASSERT_COND( var_list.size() <= mVarSize );

  for ( SizeType i = 0; i < var_list.size(); ++ i ) {
    auto var = var_list[i];
    ++ mHeapNum;
    set(var, i);
  }
  for ( SizeType i = (mHeapNum / 2); i > 0; ) {
    -- i;
    move_down(i);
  }
}

// 引数の位置にある要素を適当な位置まで沈めてゆく
void
VarHeap::move_down(
  SizeType pos
)
{
  auto vindex_p = mHeap[pos];
  auto val_p = mActivity[vindex_p];
  for ( ; ; ) {
    // ヒープ木の性質から親から子の位置がわかる
    auto pos_l = left(pos);
    auto pos_r = pos_l + 1;
    if ( pos_r > mHeapNum ) {
      // 左右の子どもを持たない場合
      break;
    }
    // 左右の子供のうちアクティビティの大きい方を pos_c とする．
    // 同点なら左を選ぶ．
    auto pos_c = pos_l;
    auto vindex_c = mHeap[pos_c];
    auto val_c = mActivity[vindex_c];
    if ( pos_r < mHeapNum ) {
      auto vindex_r = mHeap[pos_r];
      auto val_r = mActivity[vindex_r];
      if ( val_c < val_r ) {
	pos_c = pos_r;
	vindex_c = vindex_r;
	val_c = val_r;
      }
    }
    // 子供のアクティビティが親を上回らなければ終わり
    if ( val_c <= val_p ) {
      break;
    }
    // 逆転
    set(vindex_p, pos_c);
    set(vindex_c, pos);
    pos = pos_c;
  }
}

// @brief 引数の位置にある要素を適当な位置まで上げてゆく
void
VarHeap::move_up(
  SizeType pos
)
{
  auto vindex = mHeap[pos];
  auto val = mActivity[vindex];
  while ( pos > 0 ) {
    auto pos_p = parent(pos);
    auto vindex_p = mHeap[pos_p];
    auto val_p = mActivity[vindex_p];
    if ( val_p >= val ) {
      break;
    }
    set(vindex, pos_p);
    set(vindex_p, pos);
    pos = pos_p;
  }
}

// @brief 内容を出力する
void
VarHeap::dump(
  ostream& s
) const
{
  s << "heap num = " << mHeapNum << endl;
  SizeType j = 0;
  SizeType nc = 1;
  const char* spc = "";
  for ( SizeType i = 0; i < mHeapNum; ++ i ) {
    auto vindex = mHeap[i];
    ASSERT_COND( mHeapPos[vindex] == i );
    if ( i > 0 ) {
      auto p = parent(i);
      auto pindex = mHeap[p];
      ASSERT_COND( mActivity[pindex] >= mActivity[vindex] );
    }
    s << spc << vindex << "("
      << mActivity[vindex]
      << ")";
    ++ j;
    if ( j == nc ) {
      j = 0;
      nc <<= 1;
      s << endl;
      spc = "";
    }
    else {
      spc = " ";
    }
  }
  if ( j > 0 ) {
    s << endl;
  }
}

END_NAMESPACE_YM_SAT
