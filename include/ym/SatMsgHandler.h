﻿#ifndef YM_SATMSGHANDLER_H
#define YM_SATMSGHANDLER_H

/// @file ym/SatMsgHandler.h
/// @brief SatMsgHandler のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2013-2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatMsgHandler SatMsgHandler.h "ym/SatMsgHandler.h"
/// @ingroup SatGroup
/// @brief SAT ソルバの内部状態を出力するためのメッセージハンドラ
/// @sa SatSolver
//////////////////////////////////////////////////////////////////////
class SatMsgHandler
{
protected:

  /// @brief デストラクタ
  virtual
  ~SatMsgHandler() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ヘッダの出力
  virtual
  void
  print_header() = 0;

  /// @brief 内部状態の出力を行う仮想関数
  /// @param[in] stats SAT ソルバの内部状態
  virtual
  void
  print_message(const SatStats& stats) = 0;

  /// @brief フッタの出力
  /// @param[in] stats SAT ソルバの内部状態
  virtual
  void
  print_footer(const SatStats& stats) = 0;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATMSGHANDLER_H
