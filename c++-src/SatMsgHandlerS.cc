﻿
/// @file SatMsgHandlerS.cc
/// @brief SatMsgHandlerS の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2010, 2013-2014, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatMsgHandlerS.h"
#include "ym/SatStats.h"


BEGIN_NAMESPACE_YM_SAT

// @brief コンストラクタ
SatMsgHandlerS::SatMsgHandlerS(ostream& s) :
  mS(s)
{
}

// @brief デストラクタ
SatMsgHandlerS::~SatMsgHandlerS()
{
}

// @brief ヘッダの出力
void
SatMsgHandlerS::print_header()
{
  mS << "====================[ Problem Statistics ] ========================" << endl
     << "|                                                                 |" << endl
     << "|  Number of variables:                                           |" << endl
     << "|  Number of clauses:                                             |" << endl
     << "====================[ Search Statistics ] =========================" << endl
     << "| conflicts |       ORIGINAL      |             LEARNT            |" << endl
     << "|           |   Clauses      Lits |     limit   Clauses    Lit/Cl |" << endl
     << "===================================================================" << endl;
}

// @brief メッセージ出力関数
void
SatMsgHandlerS::print_message(const SatStats& stats)
{
  mS << "| "
     << setw(9) << stats.mConflictNum
     << " | "
     << setw(9) << stats.mConstrClauseNum
     << " "
     << setw(9) << stats.mConstrLitNum
     << " | "
     << setw(9) << stats.mLearntLimit
     << " "
     << setw(9) << stats.mLearntClauseNum
     << " " << setw(9);
  if ( stats.mLearntClauseNum == 0 ) {
    mS << 0;
  }
  else {
    mS << static_cast<int>(static_cast<double>(stats.mLearntLitNum) / stats.mLearntClauseNum);
  }
  mS << " |" << endl;
}

// @brief フッタの出力
// @param[in] stats SAT ソルバの内部状態
void
SatMsgHandlerS::print_footer(const SatStats& stats)
{
  mS << "===================================================================" << endl
     << "restarts          : " << stats.mRestart << endl
     << "conflicts         : " << stats.mConflictNum << endl
     << "decisions         : " << stats.mDecisionNum << endl
     << "propagations      : " << stats.mPropagationNum << endl
     << "conflict literals : " << stats.mLearntLitNum << endl
     << "CPU time(ms)      : " << stats.mTime.count() << endl;
}

END_NAMESPACE_YM_SAT
