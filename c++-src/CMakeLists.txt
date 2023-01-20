
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースの設定
# ===================================================================

set (ym_sat_SOURCES
  SatMsgHandlerS.cc
  SatSolver.cc
  SatSolver_bv.cc
  SatSolver_count.cc
  SatSolver_tseitin.cc
  SatLogger.cc
  SatLoggerS.cc
  SatOrderedSet.cc
  SatDimacs.cc

  MiniSat/SatSolverMiniSat.cc
  MiniSat/Solver.C

  MiniSat2/Options.cc
  MiniSat2/SatSolverMiniSat2.cc
  MiniSat2/Solver.cc
  MiniSat2/System.cc

  glueminisat-2.2.8/SatSolverGlueMiniSat2.cc
  glueminisat-2.2.8/Options.cc
  glueminisat-2.2.8/Solver.cc
  glueminisat-2.2.8/System.cc

  lingeling/SatSolverLingeling.cc
  lingeling/lglbnr.c
  lingeling/lgldimacs.c
  lingeling/lglib.c

  ymsat/core/AssignList.cc
  ymsat/core/Clause.cc
  ymsat/core/CoreMgr.cc
  ymsat/controller/ControllerMS1.cc
  ymsat/controller/ControllerMS2.cc
  ymsat/analyzer/SaBase.cc
  ymsat/analyzer/SaFactory.cc
  ymsat/analyzer/SaUIP1.cc
  ymsat/analyzer/SaUIP2.cc
  ymsat/selecter/Selecter1.cc
  ymsat/YmSat.cc
  ymsat/YmSat1.cc
  ymsat/YmSatMS2.cc

  ymsat1/AssignList.cc
  ymsat1/SaBase.cc
  ymsat1/SaFactory.cc
  ymsat1/SaUIP1.cc
  ymsat1/SaUIP2.cc
  ymsat1/SatClause.cc
  ymsat1/VarHeap.cc
  ymsat1/YmSat.cc

  #ymsat_old/AssignList.cc
  ymsat_old/SaBase.cc
  ymsat_old/SaFactory.cc
  ymsat_old/SaUIP1.cc
  ymsat_old/SaUIP2.cc
  #ymsat_old/SatClause.cc
  ymsat_old/VarHeap.cc
  ymsat_old/YmSat_base.cc
  ymsat_old/YmSat_solve.cc
  ymsat_old/YmSatMS2.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
ym_add_object_library ( ym_sat
  ${ym_sat_SOURCES}
  )
