#ifndef YMSAT1OLD_SAUIP1_H
#define YMSAT1OLD_SAUIP1_H

/// @file SaUIP1.h
/// @brief SaUIP1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SaBase.h"


BEGIN_NAMESPACE_YM_YMSATOLD

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
    YmSat* solver ///< [in] SATソルバ
  );

  /// @brief デストラクタ
  ~SaUIP1();


public:
  //////////////////////////////////////////////////////////////////////
  // SatAnalyer の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 解析を行う．
  /// @return バックトラックレベル
  int
  analyze(
    Reason creason,         ///< [in] 矛盾を起こした原因
    vector<Literal>& learnt ///< [in] 学習された節を表すリテラルのベクタ
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
    int& count               ///< [inout] ペンディング状態のリテラル数
  );

};

END_NAMESPACE_YM_YMSATOLD

#endif // YMSAT1OLD_SAUIP1_H
