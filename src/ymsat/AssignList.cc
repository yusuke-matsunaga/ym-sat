﻿
/// @file AssignList.cc
/// @brief AssignList の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "AssignList.h"


BEGIN_NAMESPACE_YM_SAT

// @brief コンストラクタ
AssignList::AssignList() :
  mSize(1024),
  mList(new Literal[mSize]),
  mTail(0),
  mHead(0),
  mMarker(new ymuint32[mSize]),
  mCurLevel(0)
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
AssignList::reserve(ymuint req_size)
{
  ymuint new_size = mSize;
  while ( new_size <= req_size ) {
    new_size <<= 1;
  }
  if ( mSize < new_size ) {
    SatLiteral* old_list = mList;
    mSize = new_size;
    mList = new SatLiteral[mSize];
    for (ymuint i = 0; i < mTail; ++ i) {
      mList[i] = old_list[i];
    }
    delete [] old_list;

    // 実際には mMarker の大きさが mList と同じになることは
    // ほとんどないが，このタイミングでしか容量を調節できないので
    // 一緒にしている．
    ymuint32* old_marker = mMarker;
    mMarker = new ymuint32[mSize];
    for (ymuint i = 0; i < mCurLevel; ++ i) {
      mMarker[i] = old_marker[i];
    }
    delete [] old_marker;
  }
}

END_NAMESPACE_YM_SAT
