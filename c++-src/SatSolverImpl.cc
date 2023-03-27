
/// @file SatSolverImpl.cc
/// @brief SatSolverImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolverType.h"
#include "SatCore.h"
#include "ymsat1/YmSat.h"
#include "MiniSat/SatSolverMiniSat.h"
#include "MiniSat2/SatSolverMiniSat2.h"
#include "glueminisat-2.2.8/SatSolverGlueMiniSat2.h"
#include "lingeling/SatSolverLingeling.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// SatSolverType
//////////////////////////////////////////////////////////////////////

SatSolverType::SatSolverType(
  const string& type,
  const string& option,
  ostream* log_out
) : mType{type},
    mOption{option},
    mLogOut{log_out}
{
  if ( mType == "minisat" ) {
    // minisat-1.4
    ;
  }
  else if ( mType == "minisat2" ) {
    // minisat-2.2
    ;
  }
  else if ( mType == "glueminisat2" ) {
    // glueminisat-2.2.8
    ;
  }
  else if ( mType == "lingeling" ) {
    ;
  }
  else if ( mType == "ymsat1" ) {
    ;
  }
  else if ( mType == "ymsat2" ) {
    ;
  }
  else if ( mType == "ymsat1_old" ) {
    ;
  }
  else if ( mType == "" ) {
    // lingeling 今はデフォルトにしている．
    mType = "lingeling";
  }
  else {
    mType = "lingeling";
    cerr << "SatSolver: unknown type '" << type << "', '"
	 << mType << "' is used, instead." << endl;
  }
}


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
    unordered_map<string, string> opt_dic;
    return unique_ptr<SatSolverImpl>{new SatCore{"minisat1", "uip1", "wlposi", opt_dic}};
  }
  else if ( type == "ymsat2" ) {
    unordered_map<string, string> opt_dic = {
      {"phase_cache", "on"}
    };
    return unique_ptr<SatSolverImpl>{new SatCore{"minisat2", "uip2", "nega", opt_dic}};
  }
  else if ( type == "ymsat1_old" ) {
    return unique_ptr<SatSolverImpl>(new nsSat1::YmSat(option));
  }
  ASSERT_NOT_REACHED;
  return unique_ptr<SatSolverImpl>{nullptr};
}

END_NAMESPACE_YM_SAT
