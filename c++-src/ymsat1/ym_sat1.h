#ifndef YM_SAT1_H
#define YM_SAT1_H

/// @file libym_logic/sat/ymsat/YmSat1.h
/// @brief YmSat1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"


/// @brief sat 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_SAT1 \
BEGIN_NAMESPACE_YM_SAT \
BEGIN_NAMESPACE(nsSat1)

/// @brief sat 用の名前空間の終了
#define END_NAMESPACE_YM_SAT1 \
END_NAMESPACE(nsSat1) \
END_NAMESPACE_YM_SAT

#endif // YM_SAT1_H
