#ifndef SELECTER2_H
#define SELECTER2_H

/// @file Selecter2.h
/// @brief Selecter2 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "Selecter.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class Selecter Selecter1.h "Selecter1.h"
/// @brief 次の割り当てを選ぶクラス
//////////////////////////////////////////////////////////////////////
class Selecter2 :
  public Selecter
{
public:

  /// @brief コンストラクタ
  /// @param[in] mgr コアマネージャ
  Selecter2(CoreMgr& mgr);

  /// @brief デストラクタ
  virtual
  ~Selecter2();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の割り当てを選ぶ．
  virtual
  SatLiteral
  next_decision();


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コアマネージャ
  CoreMgr& mMgr;

};

END_NAMESPACE_YM_SAT

#endif // SELECTER2_H
