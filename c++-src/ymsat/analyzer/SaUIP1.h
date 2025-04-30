#ifndef SAUIP1_H
#define SAUIP1_H

/// @file SaUIP1.h
/// @brief SaUIP1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.

#include "SaBase.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SaUIP1 SaUIP1.h "SaUIP1.h"
/// @brief first UIP ヒューリスティックを用いた解析器
//////////////////////////////////////////////////////////////////////
class SaUIP1 :
  public SaBase
{
public:

  /// @brief コンストラクタ
  SaUIP1(
    SatCore& core ///< [in] コアマネージャ
  );

  /// @brief デストラクタ
  ~SaUIP1();


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

  /// creason の矛盾の原因になっている割り当てのうち，
  /// - もっとも近い unique identification point
  /// - 現在のレベルよりも低いレベルの割り当て
  /// からなるセパレータ集合を返す．
  void
  capture(
    Reason creason,         ///< [in] 矛盾を起こした原因
    vector<Literal>& learnt ///< [out] 結果を格納する変数
  );

  /// @brief conflict 節のリテラルに対する処理を行う．
  void
  put_lit(
    Literal lit,             ///< [in] リテラル
    vector<Literal>& learnt, ///< [in] 学習節の要素リスト
    int& count               ///< [inout] ペンディング状態のリテラル数
  )
  {
    auto var = lit.varid();
    int var_level = decision_level(var);
    if ( !get_mark(var) && var_level > 0 ) {
      set_mark(var);
      bump_var_activity(var);
      if ( var_level < decision_level() ) {
	learnt.push_back(lit);
      }
      else {
	++ count;
      }
    }
  }

};

END_NAMESPACE_YM_SAT

#endif // SAUIP1_H
