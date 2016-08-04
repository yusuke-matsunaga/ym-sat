#ifndef YM_SAT1_H
#define YM_SAT1_H

/// @file libym_logic/sat/ymsat/YmSat1.h
/// @brief YmSat1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"


/// @brief sat 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_SAT1 \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsSat1)

/// @brief sat 用の名前空間の終了
#define END_NAMESPACE_YM_SAT1 \
END_NAMESPACE(nsSat1) \
END_NAMESPACE_YM

BEGIN_NAMESPACE_YM_SAT
#if 0
class SaFactory;
class SatAnalyzer;
class SatClause;
class SatReason;
class AssignList;
class Watcher;
class WatcherList;
#endif
END_NAMESPACE_YM_SAT

BEGIN_NAMESPACE_YM_SAT1
#if 0
using nsSat::SaFactory;
using nsSat::SatAnalyzer;
using nsSat::SatClause;
using nsSat::SatReason;
using nsSat::AssignList;
using nsSat::Watcher;
using nsSat::WatcherList;
#endif
END_NAMESPACE_YM_SAT1

#endif // YM_SAT1_H
