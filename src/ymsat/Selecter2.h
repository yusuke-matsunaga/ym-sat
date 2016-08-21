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

  /// @brief デストラクタ
  virtual
  ~Selecter1() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の割り当てを選ぶ．
  /// @param[in] mgr コアマネージャ
  virtual
  SatLiteral
  next_decision(CoreMgr& mgr);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////


};

END_NAMESPACE_YM_SAT

#endif // SELECTER2_H
