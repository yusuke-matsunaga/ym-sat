
/// @file SaBase.cc
/// @brief SaBase の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SaBase.h"


BEGIN_NAMESPACE_YM_SAT1

// @brief コンストラクタ
SaBase::SaBase(
  YmSat* solver
) : Analyzer{solver}
{
}

// @brief デストラクタ
SaBase::~SaBase()
{
}

// @brief 新しい変数が追加されたときに呼ばれる仮想関数
void
SaBase::alloc_var(
  SizeType size
)
{
  mMark.resize(size, false);
}

// 再帰的なチェックを行う簡単化
// lit_list に含まれるリテラルのうち，その理由となっている割り当て，
// もしくはそのまた理由となっている割り当てを再帰的にたどり，
// そのなかに一つでもマークの付いていない決定割り当て(理由をもたない割り当て)
// があればそのリテラルを残す．
// 要するに，矛盾の起きた割り当てに対する極小セパレーター集合を求めている．
void
SaBase::make_minimal(
  vector<Literal>& lit_list
)
{
  SizeType nl = lit_list.size();

  // lit_list に含まれているリテラルのレベルのビットマップ
  // ただし 64 のモジュロをとっている．
  ymuint64 lmask = 0ULL;
  for ( SizeType i = 0; i < nl; ++ i ) {
    auto p = lit_list[i];
    int level = decision_level(p.varid());
    lmask |= (1ULL << (level & 63));
  }

  SizeType wpos = 0;
  for ( SizeType i = 0; i < nl; ++ i ) {
    auto p = lit_list[i];
    auto var = p.varid();
    SizeType top = mClearQueue.size();
    if ( check_recur(var, lmask) ) {
      if ( wpos != i ) {
	lit_list[wpos] = p;
      }
      ++ wpos;
    }
    for ( SizeType j = top; j < mClearQueue.size(); ++ j ) {
      set_mark(mClearQueue[j], false);
    }
    mClearQueue.erase(mClearQueue.begin() + top, mClearQueue.end());
  }
  if ( wpos < nl ) {
    lit_list.erase(lit_list.begin() + wpos, lit_list.end());
  }
}

// make_minimal のサブルーティン
// varid が極小セパレータの要素であるとき true を返す．
// 探索でたどったノードにはマークを付け，mClearQueue2 に積む．
// マークの付いたノードは2度と探索する必要はない．
// 以前の探索の結果が true ならその場で再帰関数は終わるので2度と
// たどることはないし，以前の結果が false ならそのままでよい．
bool
SaBase::check_recur(
  int varid,
  ymuint64 lmask
)
{
  mVarStack.clear();
  mVarStack.push_back(varid);

  while ( !mVarStack.empty() ) {
    auto varid = mVarStack.back();
    mVarStack.pop_back();
    auto r{reason(varid)};
    if ( r == Reason::None ) {
      // varid は決定ノードだった．
      return true;
    }

    if ( ((1ULL << (decision_level(varid) & 63)) & lmask) == 0ULL ) {
      // varid の割り当てレベルと同じレベルの割り当ては lit_list に含まれていない．
      // ということはこのまま再帰を続けても，lit_list の他のリテラルには
      // 到達不可能であることがわかる．
      return true;
    }

    if ( r.is_clause() ) {
      auto clause = r.clause();
      SizeType n = clause->lit_num();
      auto p = clause->wl0();
      for ( SizeType i = 0; i < n; ++ i ) {
	auto q = clause->lit(i);
	if ( q != p ) {
	  put_var(q);
	}
      }
    }
    else {
      auto q = r.literal();
      put_var(q);
    }
  }
  return false;
}

// decision level の高いリテラルを 2番め (lit_list[1] の位置) に持ってくる
// 2番めのリテラルのレベルを返す．
int
SaBase::reorder(
  vector<Literal>& lit_list
)
{
  SizeType n = lit_list.size();
  if ( n < 2 ) {
    return 0;
  }
  auto lit1 = lit_list[1];
  int level = decision_level(lit1.varid());
  SizeType pos = 1;
  for ( SizeType i = 2; i < n; ++ i ) {
    auto lit2 = lit_list[i];
    int level2 = decision_level(lit2.varid());
    if ( level < level2 ) {
      level = level2;
      pos = i;
    }
  }
  if ( pos != 1 ) {
    lit_list[1] = lit_list[pos];
    lit_list[pos] = lit1;
  }
  return level;
}

// var->mMark を設定してキューに積む
void
SaBase::set_mark_and_putq(
  int var
)
{
  set_mark(var, true);
  mClearQueue.push_back(var);
}

// mClearQueue につまれた変数のマークを消す．
void
SaBase::clear_marks()
{
  // var->mMark をクリアする．
  for ( auto var: mClearQueue ) {
    set_mark(var, false);
  }
  mClearQueue.clear();
}

END_NAMESPACE_YM_SAT1
