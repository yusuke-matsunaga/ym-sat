#ifndef YM_DIMACSPARSER_H
#define YM_DIMACSPARSER_H

/// @file ym/DimacsParser.h
/// @brief DimacsParser のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/ym_sat.h"
#include "ym/IDO.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class DimacsParser DimacsParser.h "ym/DimacsParser.h"
/// @ingroup SatGroup
/// @brief DIMACS形式の CNFファイルを読み込むパーサークラス
/// @sa DimacsHandler
///
/// 使い方は DimacsHandler (の派生クラス)を add_handler() で登録しておいて
/// read() で読み込む．
/// 読み込み時のアクションは DimacsHandler の派生クラスの仮想関数として
/// 定義しておく．
//////////////////////////////////////////////////////////////////////
class DimacsParser
{
public:

  /// @brief コンストラクタ
  DimacsParser();

  /// @brief デストラクタ
  ~DimacsParser();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 読み込みを行う．
  /// @param[in] ido 入力データ
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read(IDO& ido);

  /// @brief イベントハンドラの登録
  void
  add_handler(DimacsHandler* handler);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // イベントハンドラのリスト
  vector<DimacsHandler*> mHandlerList;

};

END_NAMESPACE_YM_SAT

#endif // YM_DIMACSPARSER_H
