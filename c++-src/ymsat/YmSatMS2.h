#ifndef YMSATMS2_H
#define YMSATMS2_H

/// @file YmSatMS2.h
/// @brief YmSatMS2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2015, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "SatCore.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class YmSatMS2 YmSatMS2.h "YmSatMS2.h"
/// @brief MiniSat2 もどきの YmSat
//////////////////////////////////////////////////////////////////////
class YmSatMS2 :
  public SatCore
{
public:

  /// @brief コンストラクタ
  /// @param[in] option オプション文字列
  YmSatMS2(
    const string& option = string{}
  );

  /// @brief デストラクタ
  ~YmSatMS2();

};

END_NAMESPACE_YM_SAT

#endif // YMSATMS2_H
