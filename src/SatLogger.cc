
/// @file SatLogger.cc
/// @brief SatLogger の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016 Yusuke Matsunaga
/// All rights reserved.


#include "SatLogger.h"
#include "ym/SatVarId.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatLogger
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatLogger::SatLogger()
{
}

// @brief デストラクタ
SatLogger::~SatLogger()
{
}

// @brief 変数を追加する．
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
void
SatLogger::new_var(SatVarId id)
{
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatLogger::add_clause(const vector<SatLiteral>& lits)
{
}

// @brief 1項の節(リテラル)を追加する．
void
SatLogger::add_clause(SatLiteral lit1)
{
}

// @brief 2項の節を追加する．
void
SatLogger::add_clause(SatLiteral lit1,
		      SatLiteral lit2)
{
}

// @brief 3項の節を追加する．
void
SatLogger::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3)
{
}

// @brief 4項の節を追加する．
void
SatLogger::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4)
{
}

// @brief 5項の節を追加する．
void
SatLogger::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4,
		      SatLiteral lit5)
{
}

// @brief assumption 付きの SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @retval kSat 充足した．
// @retval kUnsat 充足不能が判明した．
// @retval kUndet わからなかった．
//
// i 番めの変数の割り当て結果は model[i] に入る．
void
SatLogger::solve(const vector<SatLiteral>& assumptions)
{
}

END_NAMESPACE_YM_SAT
