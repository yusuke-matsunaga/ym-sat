
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

  mVarNum = size;
  if ( mVarSize == 0 ) {
    mVarSize = 1024;
  }
  while ( mVarSize < mVarNum ) {
    mVarSize <<= 1;
  }
  if ( mVarSize != old_size ) {
    mHeapPos = new int[mVarSize];
    mActivity = new double[mVarSize];
    mHeap = new SatVarId[mVarSize];
    for ( SizeType i = 0; i < old_var_num; ++ i ) {
      mHeapPos[i] = old_heap_pos[i];
      mActivity[i] = old_activity[i];
    }
    for ( SizeType i = old_var_num; i < mVarNum; ++ i ) {
      mHeapPos[i] = -1;
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
}

// @brief 変数のアクティビティを初期化する．
void
VarHeap::reset_activity()
{
  if ( debug_varheap ) {
    DOUT << "VarHeap::reset_activity()" << endl;
  }
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
  if ( debug_varheap ) {
    DOUT << "VarHeap::build(";
    for ( auto var: var_list ) {
      DOUT << " " << var;
    }
    DOUT << ")" << endl;
  }
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
