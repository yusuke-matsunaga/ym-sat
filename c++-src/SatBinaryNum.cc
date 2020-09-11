
/// @file SatBinaryNum.cc
/// @brief SatBinaryNum の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatBinaryNum.h"
#include "ym/SatSolver.h"
#include "ym/SatModel.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// クラス SatBinaryNum
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
SatBinaryNum::SatBinaryNum() :
  mBitNum(0)
{
}

// @brief コンストラクタ
// @param[in] solver SATソルバ
// @param[in] bit_num ビット幅
SatBinaryNum::SatBinaryNum(SatSolver& solver,
			   SizeType bit_num)
{
  init(solver, bit_num);
}

// @brief デストラクタ
SatBinaryNum::~SatBinaryNum()
{
}

// @brief 初期化を行う．
// @param[in] solver SATソルバ
// @param[in] bit_num ビット幅
void
SatBinaryNum::init(SatSolver& solver,
		   SizeType bit_num)
{
  mBitNum = bit_num;
  mVarArray.clear();
  mVarArray.resize(mBitNum);
  for ( SizeType i = 0; i < mBitNum; ++ i ) {
    mVarArray[i] = solver.new_variable();
  }
}

// @brief SATの解から値を得る．
// @param[in] model SATの解
SizeType
SatBinaryNum::val(const SatModel& model) const
{
  SizeType ans = 0;
  for ( SizeType bit = 0; bit < bit_num(); ++ bit ) {
    auto lit{bit_var(bit)};
    if ( model[lit] == SatBool3::True ) {
      ans |= (1 << bit);
    }
  }

  return ans;
}

END_NAMESPACE_YM_SAT
