# ===================================================================
# CMAKE のおまじない
# ===================================================================


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================


# ===================================================================
# オプション
# ===================================================================


# ===================================================================
# パッケージの検査
# ===================================================================


# ===================================================================
# ヘッダファイルの生成
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${PROJECT_BINARY_DIR}/include
  ${PROJECT_SOURCE_DIR}/ym-common/include
  ${PROJECT_SOURCE_DIR}/ym-base/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/private_include
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory( tests/gtest )
add_subdirectory( py_ymsat )


# ===================================================================
#  ソースの設定
# ===================================================================

set (ym_sat_SOURCES
  c++-src/SatMsgHandlerS.cc
  c++-src/SatSolver.cc
  c++-src/SatSolver_bv.cc
  c++-src/SatSolver_count.cc
  c++-src/SatSolver_tseitin.cc
  c++-src/SatLogger.cc
  c++-src/SatLoggerS.cc
  c++-src/SatStats.cc
  c++-src/SatBinaryNum.cc
  c++-src/SatOrderedSet.cc
  c++-src/SatDimacs.cc

  c++-src/MiniSat/SatSolverMiniSat.cc
  c++-src/MiniSat/Solver.C

  c++-src/MiniSat2/Options.cc
  c++-src/MiniSat2/SatSolverMiniSat2.cc
  c++-src/MiniSat2/Solver.cc
  c++-src/MiniSat2/System.cc

  c++-src/glueminisat-2.2.8/SatSolverGlueMiniSat2.cc
  c++-src/glueminisat-2.2.8/Options.cc
  c++-src/glueminisat-2.2.8/Solver.cc
  c++-src/glueminisat-2.2.8/System.cc

  c++-src/lingeling/SatSolverLingeling.cc
  c++-src/lingeling/lglbnr.c
  c++-src/lingeling/lgldimacs.c
  c++-src/lingeling/lglib.c

  c++-src/ymsat/core/AssignList.cc
  c++-src/ymsat/core/SatClause.cc
  c++-src/ymsat/core/CoreMgr.cc
  c++-src/ymsat/controller/ControllerMS1.cc
  c++-src/ymsat/controller/ControllerMS2.cc
  c++-src/ymsat/analyzer/SaBase.cc
  c++-src/ymsat/analyzer/SaFactory.cc
  c++-src/ymsat/analyzer/SaUIP1.cc
  c++-src/ymsat/analyzer/SaUIP2.cc
  c++-src/ymsat/selecter/Selecter1.cc
  c++-src/ymsat/YmSat.cc
  c++-src/ymsat/YmSat1.cc
  c++-src/ymsat/YmSatMS2.cc

  c++-src/ymsat1/AssignList.cc
  c++-src/ymsat1/SaBase.cc
  c++-src/ymsat1/SaFactory.cc
  c++-src/ymsat1/SaUIP1.cc
  c++-src/ymsat1/SaUIP2.cc
  c++-src/ymsat1/SatClause.cc
  c++-src/ymsat1/VarHeap.cc
  c++-src/ymsat1/YmSat.cc

  c++-src/ymsat_old/AssignList.cc
  c++-src/ymsat_old/SaBase.cc
  c++-src/ymsat_old/SaFactory.cc
  c++-src/ymsat_old/SaUIP1.cc
  c++-src/ymsat_old/SaUIP2.cc
  c++-src/ymsat_old/SatClause.cc
  c++-src/ymsat_old/VarHeap.cc
  c++-src/ymsat_old/YmSat_base.cc
  c++-src/ymsat_old/YmSat_solve.cc
  c++-src/ymsat_old/YmSatMS2.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
ym_add_object_library ( ym_sat
  ${ym_sat_SOURCES}
  )
