﻿#ifndef YM_SATMSGHANDLERIMPL1_H
#define YM_SATMSGHANDLERIMPL1_H

/// @file ym/SatMsgHandlerImpl1.h
/// @brief SatMsgHandlerImpl1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatMsgHandler.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatMsgHandlerImpl1 SatMsgHandlerImpl1.h "ym/SatMsgHandlerImpl1.h"
/// @brief SatMsgHandler の実装例
//////////////////////////////////////////////////////////////////////
class SatMsgHandlerImpl1 :
  public SatMsgHandler
{
public:

  /// @brief コンストラクタ
  /// @param[in] s 出力先のストリーム
  SatMsgHandlerImpl1(ostream& s);

  /// @brief デストラクタ
  virtual
  ~SatMsgHandlerImpl1();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ヘッダの出力
  virtual
  void
  print_header();

  /// @brief メッセージ出力関数
  virtual
  void
  print_message(const SatStats& stats);

  /// @brief フッタの出力
  /// @param[in] stats SAT ソルバの内部状態
  virtual
  void
  print_footer(const SatStats& stats);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力ストリーム
  ostream& mS;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATMSGHANDLERIMPL1_H
