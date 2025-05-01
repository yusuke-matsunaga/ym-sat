
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
VarHeap::VarHeap(
) : mHeapPos(1024),
    mActivity(1024),
    mHeap(1024)
{
}

// @brief デストラクタ
VarHeap::~VarHeap()
{
}

// @brief req_size 個の要素を格納出来るだけの領域を確保する．
void
VarHeap::alloc_var(
  SizeType req_size
)
{
  SizeType size = mActivity.size();
  if ( size < req_size ) {
    while ( size < req_size ) {
      size <<= 1;
    }
    mHeapPos.resize(size, -1);
    mActivity.resize(size);
    mHeap.resize(size);
  }
}

// @brief 変数のアクティビティを初期化する．
void
VarHeap::reset_activity()
{
  if ( debug_varheap ) {
    DOUT << "VarHeap::reset_activity()" << endl;
  }
  for ( SizeType i = 0; i < mActivity.size(); ++ i ) {
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

  if ( var_list.size() > mHeapPos.size() ) {
    throw std::invalid_argument{"var_list is too large"};
  }

  for ( SizeType i = 0; i < mHeapPos.size(); ++ i ) {
    mHeapPos[i] = -1;
  }
  mHeapNum = 0;

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
