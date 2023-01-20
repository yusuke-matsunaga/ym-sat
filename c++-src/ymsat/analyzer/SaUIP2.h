﻿#ifndef SAUIP2_H
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
//////////////////////////////////////////////////////////////////////
class SaUIP2 :
  public SaBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] mgr コアマネージャ
  SaUIP2(CoreMgr& mgr);

  /// @brief デストラクタ
  ~SaUIP2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatAnalyer の仮想関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 解析を行う．
  /// @param[in] creason 矛盾を起こした原因
  /// @return バックトラックレベルと学習された節を表すリテラルのベクタを返す．
  tuple<int, vector<Literal>>
  analyze(Reason creason) override;


protected:
  //////////////////////////////////////////////////////////////////////
  // 内部もしくは継承クラスで用いられる関数
  //////////////////////////////////////////////////////////////////////

  vector<Literal>
  capture(Reason creason);

};

END_NAMESPACE_YM_SAT

#endif // SAUIP1_H
