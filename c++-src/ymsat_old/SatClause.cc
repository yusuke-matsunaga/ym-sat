
/// @file SatClause.cc
/// @brief SatClause の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ymsat_old/SatClause.h"
#include "ymsat_old/SatReason.h"
#include "ymsat_old/Watcher.h"


BEGIN_NAMESPACE_YM_YMSATOLD

//////////////////////////////////////////////////////////////////////
// クラス SatClause
//////////////////////////////////////////////////////////////////////

// @brief SatClause の内容を出力する
ostream&
operator<<(ostream& s,
	   const SatClause& c)
{
  int n = c.lit_num();
  if ( n == 2 ) {
    s << "(" << c.lit(0) << " + " << c.lit(1) << ")";
  }
  else {
    // 一旦 vector に入れてソートする．
    vector<SatLiteral> tmp(n);
    for ( int i = 0; i < n; ++ i ) {
      tmp[i] = c.lit(i);
    }
    //sort(tmp.begin() + 1, tmp.end());

    s << "(";
    const char* plus = "";
    for ( int i = 0; i < n; ++ i ) {
      s << plus << tmp[i];
      plus = " + ";
    }
    s << ")";
  }
  return s;
}


//////////////////////////////////////////////////////////////////////
// クラス SatReason
//////////////////////////////////////////////////////////////////////

// @brief SatReason の内容を出力する
ostream&
operator<<(ostream& s,
	   const SatReason& r)
{
  if ( r.is_literal() ) {
    s << r.literal();
  }
  else {
    s << *(r.clause());
  }
  return s;
}


END_NAMESPACE_YM_YMSATOLD
