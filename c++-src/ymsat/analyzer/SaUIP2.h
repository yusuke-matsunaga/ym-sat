#ifndef SAUIP2_H
#define SAUIP2_H

/// @file SaUIP2.h
/// @brief SaUIP2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "SaBase.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SaUIP2 SaUIP2.h "SaUIP2.h"
/// @brief first UIP ヒューリスティックを用いた解析器
///
/// SaUIP1 との違いは put_lit() の挙動のみ
//////////////////////////////////////////////////////////////////////
class SaUIP2 :
  public SaBase
{
public:

  /// @brief コンストラクタ
  SaUIP2(
    SatCore& core ///< [in] コアマネージャ
  );

  /// @brief デストラクタ
  ~SaUIP2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatAnalyer の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 解析を行う．
  /// @return バックトラックレベルを返す．
  int
  analyze(
    Reason creason,              ///< [in] 矛盾の原因
    vector<Literal>& learnt_lits ///< [out] 学習された節を表すリテラルのベクタ
  ) override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部もしくは継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  void
  capture(
    Reason creason,
    vector<Literal>& learnt
  );

  /// @brief conflict 節のリテラルに対する処理を行う．
  void
  put_lit(
    Literal lit,             ///< [in] リテラル
    vector<Literal>& learnt, ///< [in] 学習節の要素リスト
    int& count		     ///< [inout] ペンディング状態のリテラル数
  )
  {
    auto var = lit.varid();
    int var_level = decision_level(var);
    if ( !get_mark(var) && var_level > 0 ) {
      set_mark(var);
      bump_var_activity(var);
      if ( var_level < decision_level() ) {
	auto cr1 = reason(var);
	if ( cr1.is_literal() ) {
	  learnt.push_back(cr1.literal());
	}
	else {
	  learnt.push_back(lit);
	}
      }
      else {
	++ count;
      }
    }
  }

};

END_NAMESPACE_YM_SAT

#endif // SAUIP1_H
