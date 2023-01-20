#ifndef SAUIP2_H
#define SAUIP2_H

/// @file SaUIP2.h
/// @brief SaUIP2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SaBase.h"


BEGIN_NAMESPACE_YM_SAT1

//////////////////////////////////////////////////////////////////////
/// @class SaUIP2 SaUIP2.h "SaUIP2.h"
/// @brief first UIP ヒューリスティックを用いた解析器
//////////////////////////////////////////////////////////////////////
class SaUIP2 :
  public SaBase
{
public:

  /// @brief コンストラクタ
  SaUIP2(
    YmSat* solver ///< [in] SATソルバ
  );

  /// @brief デストラクタ
  ~SaUIP2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatAnalyer の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 解析を行う．
  /// @return バックトラックレベル
  int
  analyze(
    Reason creason,         ///< [in] 矛盾を起こした原因
    vector<Literal>& learnt ///< [out] 学習された節を表すリテラルのベクタ
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

};

END_NAMESPACE_YM_SAT1

#endif // SAUIP1_H
