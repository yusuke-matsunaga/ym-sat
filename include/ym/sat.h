﻿#ifndef YM_SAT_H
#define YM_SAT_H

/// @file ym/sat.h
/// @brief SAT Solver 用の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.

/// @defgroup SatGroup SAT ソルバ
///
/// 中身はフルスクラッチで API もオリジナルだが，実装のアイデアの多くは
/// MiniSat-1.14 を参考にしている．ちなみに MiniSat そのものを ymsat の
/// API で使うこともできる．


//////////////////////////////////////////////////////////////////////
// ym-common のコンフィギュレーションファイル
//////////////////////////////////////////////////////////////////////

#include "ym_config.h"


//////////////////////////////////////////////////////////////////////
// sat 用の名前空間の定義用マクロ
//////////////////////////////////////////////////////////////////////

/// @brief sat 用の名前空間の開始
#define BEGIN_NAMESPACE_YM_SAT \
BEGIN_NAMESPACE_YM \
BEGIN_NAMESPACE(nsSat)

/// @brief sat 用の名前空間の終了
#define END_NAMESPACE_YM_SAT \
END_NAMESPACE(nsSat) \
END_NAMESPACE_YM

/// @namespace nsYm::nsSat
/// @brief SAT ソルバ関係のクラスが属する名前空間
///
/// ただし，主要なものは nsYm にインポートされている．
/// 実装用のローカルな要素だけがこの名前空間に存在する．

BEGIN_NAMESPACE_YM_SAT

class SatBinaryNum;
class SatBvEnc;
class SatCountEnc;
class SatTseitinEnc;
class SatDimacs;
class SatModel;
class SatOrderedSet;
class SatSolver;
class SatSolverType;
class SatStats;
class SatMsgHandler;
class SatMsgHandlerS;

END_NAMESPACE_YM_SAT

BEGIN_NAMESPACE_YM

class SatLiteral;

using nsSat::SatBinaryNum;
using nsSat::SatBvEnc;
using nsSat::SatCountEnc;
using nsSat::SatTseitinEnc;
using nsSat::SatDimacs;
using nsSat::SatModel;
using nsSat::SatOrderedSet;
using nsSat::SatSolver;
using nsSat::SatSolverType;
using nsSat::SatStats;
using nsSat::SatMsgHandler;
using nsSat::SatMsgHandlerS;

END_NAMESPACE_YM

#endif // YM_SAT_H
