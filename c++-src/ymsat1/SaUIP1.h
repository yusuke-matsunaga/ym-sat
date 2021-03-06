﻿#ifndef SAUIP1_H
#define SAUIP1_H

/// @file SaUIP1.h
/// @brief SaUIP1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "SaBase.h"


BEGIN_NAMESPACE_YM_SAT1

//////////////////////////////////////////////////////////////////////
/// @class SaUIP1 SaUIP1.h "SaUIP1.h"
/// @brief first UIP ヒューリスティックを用いた解析器
//////////////////////////////////////////////////////////////////////
class SaUIP1 :
  public SaBase
{
public:

  /// @brief コンストラクタ
  /// @param[in] solver SATソルバ
  SaUIP1(YmSat* solver);

  /// @brief デストラクタ
  ~SaUIP1();


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

  /// @brief conflict 節のリテラルに対する処理を行う．
  /// @param[in] lit リテラル
  /// @param[in] learnt 学習節の要素リスト
  /// @param[inout] count ペンディング状態のリテラル数
  void
  put_lit(SatLiteral lit,
	  vector<SatLiteral>& learnt,
	  int& count);

};

END_NAMESPACE_YM_SAT1

#endif // SAUIP1_H
