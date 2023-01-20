
/// @file Clause.cc
/// @brief Clause の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Clause.h"
#include "Reason.h"
#include "Watcher.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス Clause
//////////////////////////////////////////////////////////////////////

// @brief Clause の内容を出力する
ostream&
operator<<(
  ostream& s,
  const Clause& c
)
{
  SizeType n = c.lit_num();
  if ( n == 2 ) {
    s << "(" << c.lit(0) << " + " << c.lit(1) << ")";
  }
  else {
    // 一旦 vector に入れてソートする．
    vector<Literal> tmp(n);
    for ( SizeType i: Range(n) ) {
      tmp[i] = c.lit(i);
    }
    //sort(tmp.begin() + 1, tmp.end());

    s << "(";
    const char* plus = "";
    for ( SizeType i: Range(n) ) {
      s << plus << tmp[i];
      plus = " + ";
    }
    s << ")";
  }
  return s;
}


//////////////////////////////////////////////////////////////////////
// クラス Reason
// Clause の定義が必要なのでここに置いている
//////////////////////////////////////////////////////////////////////

const Reason Reason::None;

// @relates Reason
ostream&
operator<<(
  ostream& s,
  const Reason& c
)
{
  if ( c.is_literal() ) {
    auto lit = c.literal();
    s << lit;
  }
  else {
    s << *(c.clause());
  }
  return s;
}

END_NAMESPACE_YM_SAT
