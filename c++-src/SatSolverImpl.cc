
/// @file SatSolverImpl.cc
/// @brief SatSolverImpl の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 2018, 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatInitParam.h"
#include "SatCore.h"
#include "ymsat1/YmSat.h"
#include "MiniSat/SatSolverMiniSat.h"
#include "MiniSat2/SatSolverMiniSat2.h"
#include "glueminisat-2.2.8/SatSolverGlueMiniSat2.h"
#include "lingeling/SatSolverLingeling.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// SatSolverImpl
//////////////////////////////////////////////////////////////////////

// @brief 継承クラスを作るクラスメソッド
unique_ptr<SatSolverImpl>
SatSolverImpl::new_impl(
  const SatInitParam& init_param
)
{
  auto type = init_param.type();
  auto& js_obj = init_param.js_obj();
  if ( type == "minisat" ) {
    // minisat-1.4
    return unique_ptr<SatSolverImpl>(new SatSolverMiniSat(js_obj));
  }
  else if ( type == "minisat2" ) {
    // minisat-2.2
    return unique_ptr<SatSolverImpl>(new SatSolverMiniSat2(js_obj));
  }
  else if ( type == "glueminisat2" ) {
    // glueminisat-2.2.8
    return unique_ptr<SatSolverImpl>(new SatSolverGlueMiniSat2(js_obj));
  }
  else if ( type == "lingeling" ) {
    // lingeling
    return unique_ptr<SatSolverImpl>(new SatSolverLingeling(js_obj));
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
    return unique_ptr<SatSolverImpl>(new nsSat1::YmSat(js_obj));
  }
  ASSERT_NOT_REACHED;
  return unique_ptr<SatSolverImpl>{nullptr};
}

END_NAMESPACE_YM_SAT
