﻿#ifndef SAUIP2_H
#define SAUIP2_H

/// @file SaUIP2.h
/// @brief SaUIP2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
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
  /// @param[in] solver SATソルバ
  SaUIP2(YmSat* solver);

  /// @brief デストラクタ
  ~SaUIP2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatAnalyer の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 解析を行う．
  /// @param[in] creason 矛盾を起こした原因
  /// @param[out] learnt 学習された節を表すリテラルのベクタ
  /// @return バックトラックレベル
  int
  analyze(SatReason creason,
	  vector<SatLiteral>& learnt) override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部もしくは継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  void
  capture(SatReason creason,
	  vector<SatLiteral>& learnt);

};

END_NAMESPACE_YM_SAT1

#endif // SAUIP1_H
