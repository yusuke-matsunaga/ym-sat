#ifndef YMSAT_OLD_H
#define YMSAT_OLD_H

/// @file ymsat_old
/// @brief ymsat_old のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"


/// @brief sat 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_YMSATOLD \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsSatold)

/// @brief sat 用の名前空間の終了
#define END_NAMESPACE_YM_YMSATOLD \
END_NAMESPACE(nsSatold) \
END_NAMESPACE_YM

#endif // YMSAT_OLD_H
