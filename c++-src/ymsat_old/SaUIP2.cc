﻿
/// @file SaUIP2.cc
/// @brief SaUIP2 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SaUIP2.h"


BEGIN_NAMESPACE_YM_SAT1

// @brief コンストラクタ
SaUIP2::SaUIP2(
  YmSat* solver
) : SaBase{solver}
{
}

// @brief デストラクタ
SaUIP2::~SaUIP2()
{
}

// conflict を解析する．
int
SaUIP2::analyze(
  Reason creason,
  vector<Literal>& learnt
)
{
  capture(creason, learnt);
  make_minimal(learnt);
  clear_marks();
  return reorder(learnt);
}

// creason の矛盾の原因になっている割り当てのうち，
// - もっとも近い unique identification point
// - 現在のレベルよりも低いレベルの割り当て
// からなるセパレータ集合を learnt に入れる．
void
SaUIP2::capture(
  Reason creason,
  vector<Literal>& learnt
)
{
  learnt.clear();
  learnt.push_back(Literal::X); // place holder

  bool first = true;
  int count = 0;
  SizeType last = last_assign();
  for ( ; ; ) {
    if ( creason.is_clause() ) {
      auto cclause = creason.clause();

      // cclause が学習節なら activity をあげる．
      if ( cclause->is_learnt() ) {
	bump_clause_activity(cclause);
      }

      // cclause 節に含まれるリテラルが以前の decision level
      // で割り当てられていたら学習節に加える．
      // 現在の decision level なら count を増やすだけ．
      // あとで mAssignList をたどれば該当のリテラルは捜し出せる．
      SizeType n = cclause->lit_num();
      // 最初の節は全てのリテラルを対象にするが，
      // 二番目以降の節の最初のリテラルは割り当て結果なので除外する．
      for ( SizeType i = 0; i < n; ++ i ) {
	auto q = cclause->lit(i);
	if ( !first && q == cclause->wl0() ) continue;
	auto var = q.varid();
	int var_level = decision_level(var);
	if ( !get_mark(var) && var_level > 0 ) {
	  set_mark_and_putq(var);
	  bump_var_activity(var);
	  if ( var_level < decision_level() ) {
	    auto cr1 = reason(q.varid());
	    if ( cr1.is_literal() ) {
	      learnt.push_back(cr1.literal());
	    }
	    else {
	      learnt.push_back(q);
	    }
	  }
	  else {
	    ++ count;
	  }
	}
      }
    }
    else {
      ASSERT_COND( !first );
      auto q = creason.literal();
      auto var = q.varid();
      int var_level = decision_level(var);
      if ( !get_mark(var) && var_level > 0 ) {
	set_mark_and_putq(var);
	bump_var_activity(var);
	if ( var_level < decision_level() ) {
	  auto cr1 = reason(q.varid());
	  if ( cr1.is_literal() ) {
	    learnt.push_back(cr1.literal());
	  }
	  else {
	    learnt.push_back(q);
	  }
	}
	else {
	  ++ count;
	}
      }
    }

    first = false;

    // mAssignList に入っている最近の変数で mark の付いたものを探す．
    // つまり conflict clause に含まれていた変数ということ．
    for ( ; ; -- last) {
      auto q = get_assign(last);
      auto var = q.varid();
      if ( get_mark(var) ) {
	set_mark(var, false);
	// それを最初のリテラルにする．
	learnt[0] = ~q;
	creason = reason(q.varid());
	-- last;
	-- count;
	break;
      }
#if defined(DEBUG)
      // ここは重いのでコメントアウトしておく
      ASSERT_COND(last > 0 );
#endif
    }
    if ( count == 0 ) {
      // q は first UIP だった．
      break;
    }
  }
}

END_NAMESPACE_YM_SAT1
