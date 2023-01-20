
/// @file AssignList.cc
/// @brief AssignList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "AssignList.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

// @brief コンストラクタ
AssignList::AssignList(
) : mSize{1024},
    mList{new Literal[mSize]},
    mMarker(new SizeType[mSize])
{
}

// @brief デストラクタ
AssignList::~AssignList()
{
  delete [] mList;
  delete [] mMarker;
}

// @brief 必要なサイズを指定する．
void
AssignList::reserve(
  SizeType req_size
)
{
  auto new_size = mSize;
  while ( new_size <= req_size ) {
    new_size <<= 1;
  }
  if ( mSize < new_size ) {
    auto old_list = mList;
    mSize = new_size;
    mList = new Literal[mSize];
    for ( int i: Range(mTail) ) {
      mList[i] = old_list[i];
    }
    delete [] old_list;

    // 実際には mMarker の大きさが mList と同じになることは
    // ほとんどないが，このタイミングでしか容量を調節できないので
    // 一緒にしている．
    auto old_marker = mMarker;
    mMarker = new SizeType[mSize];
    for ( SizeType i: Range(mCurLevel) ) {
      mMarker[i] = old_marker[i];
    }
    delete [] old_marker;
  }
}

END_NAMESPACE_YM_SAT
