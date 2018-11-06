#ifndef YM_SATMSGHANDLERS_H
#define YM_SATMSGHANDLERS_H

/// @file ym/SatMsgHandlerS.h
/// @brief SatMsgHandlerS のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2013 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatMsgHandler.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatMsgHandlerS SatMsgHandlerS.h "ym/SatMsgHandlerS.h"
/// @brief SatMsgHandler の実装例
//////////////////////////////////////////////////////////////////////
class SatMsgHandlerS :
  public SatMsgHandler
{
public:

  /// @brief コンストラクタ
  /// @param[in] s 出力先のストリーム
  SatMsgHandlerS(ostream& s);

  /// @brief デストラクタ
  ~SatMsgHandlerS();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief ヘッダの出力
  void
  print_header() override;

  /// @brief メッセージ出力関数
  void
  print_message(const SatStats& stats) override;

  /// @brief フッタの出力
  /// @param[in] stats SAT ソルバの内部状態
  void
  print_footer(const SatStats& stats) override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 出力ストリーム
  ostream& mS;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATMSGHANDLERS_H
