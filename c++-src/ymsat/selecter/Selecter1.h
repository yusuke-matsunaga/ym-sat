#ifndef SELECTER1_H
#define SELECTER1_H

/// @file Selecter1.h
/// @brief Selecter1 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.

#include "../Selecter.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class Selecter1 Selecter1.h "Selecter1.h"
/// @brief 次の割り当てを選ぶクラス
//////////////////////////////////////////////////////////////////////
class Selecter1 :
  public Selecter
{
public:

  /// @brief コンストラクタ
  Selecter1(
    CoreMgr& mgr,    ///< [in] Coreマネージャ
    bool phase_cache ///< [in] 以前の極性を使うヒューリスティック
  );

  /// @brief デストラクタ
  ~Selecter1();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 次の割り当てを選ぶ．
  Literal
  next_decision() override;


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コアマネージャ
  CoreMgr& mMgr;

  // phase_cache フラグ
  bool mPhaseCache;

};

END_NAMESPACE_YM_SAT

#endif // SELECTER_H
