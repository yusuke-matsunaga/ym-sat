﻿#ifndef YMSATMS2_H
#define YMSATMS2_H

/// @file YmSatMS2.h
/// @brief YmSatMS2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "YmSat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class YmSatMS2 YmSatMS2.h "YmSatMS2.h"
/// @brief MiniSat2 もどきの YmSat
//////////////////////////////////////////////////////////////////////
class YmSatMS2 :
  public YmSat
{
public:

  /// @brief コンストラクタ
  /// @param[in] option オプション文字列
  YmSatMS2(const string& option = string());

  /// @brief デストラクタ
  ~YmSatMS2();


public:
  //////////////////////////////////////////////////////////////////////
  // SatSolver で定義されている仮想関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // 実装用のプライベート関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

};

END_NAMESPACE_YM_SAT

#endif // YMSATMS2_H
