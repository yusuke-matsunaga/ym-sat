
/// @file SatSolverImpl.cc
/// @brief SatSolverImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolverType.h"
#include "ymsat/YmSatMS2.h"
#include "ymsat/YmSat1.h"
#include "ymsat1/YmSat.h"
#include "ymsat/old/YmSatMS2.h"
#include "MiniSat/SatSolverMiniSat.h"
#include "MiniSat2/SatSolverMiniSat2.h"
#include "glueminisat-2.2.8/SatSolverGlueMiniSat2.h"
#include "lingeling/SatSolverLingeling.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// SatSolverImpl
//////////////////////////////////////////////////////////////////////

// @brief 継承クラスを作るクラスメソッド
// @param[in] solver_type SATソルバのタイプ
unique_ptr<SatSolverImpl>
SatSolverImpl::new_impl(
  const SatSolverType& solver_type
)
{
  const string& type = solver_type.type();
  const string& option = solver_type.option();
  if ( type == "minisat" ) {
    // minisat-1.4
    return unique_ptr<SatSolverImpl>(new SatSolverMiniSat(option));
  }
  else if ( type == "minisat2" ) {
    // minisat-2.2
    return unique_ptr<SatSolverImpl>(new SatSolverMiniSat2(option));
  }
  else if ( type == "glueminisat2" ) {
    // glueminisat-2.2.8
    return unique_ptr<SatSolverImpl>(new SatSolverGlueMiniSat2(option));
  }
  else if ( type == "lingeling" ) {
    // lingeling
    return unique_ptr<SatSolverImpl>(new SatSolverLingeling(option));
  }
  else if ( type == "ymsat1" ) {
    return unique_ptr<SatSolverImpl>(new YmSat1(option));
  }
  else if ( type == "ymsat2" ) {
    return unique_ptr<SatSolverImpl>(new YmSatMS2(option));
  }
  else if ( type == "ymsat2old" ) {
    return unique_ptr<SatSolverImpl>(new nsSatold::YmSatMS2(option));
  }
  else if ( type == "ymsat1_old" ) {
    return unique_ptr<SatSolverImpl>(new nsSat1::YmSat(option));
  }
  ASSERT_NOT_REACHED;
  return unique_ptr<SatSolverImpl>{nullptr};
}

END_NAMESPACE_YM_SAT
