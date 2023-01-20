
/// @file SaUIP1.cc
/// @brief SaUIP1 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "SaUIP1.h"
#include "Clause.h"


BEGIN_NAMESPACE_YM_SAT

// @brief コンストラクタ
SaUIP1::SaUIP1(
  CoreMgr& mgr
) : SaBase{mgr}
{
}

// @brief デストラクタ
SaUIP1::~SaUIP1()
{
}

// conflict を解析する．
tuple<int, vector<Literal>>
SaUIP1::analyze(
  Reason creason
)
{
  auto learnt = capture(creason);
  make_minimal(learnt);
  clear_marks();
  int lv = reorder(learnt);
  return make_tuple(lv, learnt);
}

vector<Literal>
SaUIP1::capture(
  Reason creason
)
{
  vector<Literal> learnt;
  learnt.push_back(Literal::X); // place holder

  bool first = true;
  int count = 0;
  int last = last_assign();
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
      int n = cclause->lit_num();
      // 最初の節は全てのリテラルを対象にするが，
      // 二番目以降の節の最初のリテラルは割り当て結果なので除外する．
      for ( SizeType i = 0; i < n; ++ i ) {
	auto q = cclause->lit(i);
	if ( !first && q == cclause->wl0() ) continue;
	put_lit(q, learnt, count);
      }
    }
    else {
      ASSERT_COND( !first );
      auto q = creason.literal();
      put_lit(q, learnt, count);
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
      ASSERT_COND(last > 0 );
    }
    if ( count == 0 ) {
      // q は first UIP だった．
      break;
    }
  }

  return learnt;
}

// @brief conflict 節のリテラルに対する処理を行う．
void
SaUIP1::put_lit(
  Literal lit,
  vector<Literal>& learnt,
  int& count
)
{
  auto var = lit.varid();
  int var_level = decision_level(var);
  if ( !get_mark(var) && var_level > 0 ) {
    set_mark_and_putq(var);
    bump_var_activity(var);
    if ( var_level < decision_level() ) {
      learnt.push_back(lit);
    }
    else {
      ++ count;
    }
  }
}

END_NAMESPACE_YM_SAT
