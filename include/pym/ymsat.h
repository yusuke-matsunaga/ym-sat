#ifndef PYM_YMSAT_H
#define PYM_YMSAT_H

/// @file ymsat.h
/// @brief ymsat の初期化関数のエクスポート
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym_config.h"


BEGIN_NAMESPACE_YM

/// @brief ymsat モジュールの初期化関数
extern "C" PyObject* PyInit_ymsat();

END_NAMESPACE_YM

#endif // PYM_YMSAT_H
