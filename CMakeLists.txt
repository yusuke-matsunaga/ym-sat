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
  src
  )

# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

#add_subdirectory ( cython-src )


# ===================================================================
#  ソースの設定
# ===================================================================

set (ym_sat_SOURCES
  src/SatMsgHandlerImpl1.cc
  src/SatSolver.cc
  src/SatLogger.cc
  src/SatLoggerS.cc
  src/SatStats.cc

  src/MiniSat/SatSolverMiniSat.cc
  src/MiniSat/Solver.C

  src/MiniSat2/Options.cc
  src/MiniSat2/SatSolverMiniSat2.cc
  src/MiniSat2/Solver.cc
  src/MiniSat2/System.cc

  src/glueminisat-2.2.8/SatSolverGlueMiniSat2.cc
  src/glueminisat-2.2.8/Options.cc
  src/glueminisat-2.2.8/Solver.cc
  src/glueminisat-2.2.8/System.cc

  src/dimacs/DimacsParser.cc
  src/dimacs/DimacsScanner.cc
  src/dimacs/DimacsVerifier.cc
  src/dimacs/SatDimacsHandler.cc

  src/ymsat/AssignList.cc
  src/ymsat/SaBase.cc
  src/ymsat/SaFactory.cc
  src/ymsat/SaUIP1.cc
  src/ymsat/SaUIP2.cc
  src/ymsat/SatClause.cc
  src/ymsat/Selecter1.cc
  src/ymsat/CoreMgr.cc
  src/ymsat/YmSat.cc
  src/ymsat/YmSat1.cc
  src/ymsat/YmSatMS2.cc

  src/ymsat1/AssignList.cc
  src/ymsat1/SaBase.cc
  src/ymsat1/SaFactory.cc
  src/ymsat1/SaUIP1.cc
  src/ymsat1/SaUIP2.cc
  src/ymsat1/SatClause.cc
  src/ymsat1/VarHeap.cc
  src/ymsat1/YmSat.cc

  src/ymsat_old/AssignList.cc
  src/ymsat_old/SaBase.cc
  src/ymsat_old/SaFactory.cc
  src/ymsat_old/SaUIP1.cc
  src/ymsat_old/SaUIP2.cc
  src/ymsat_old/SatClause.cc
  src/ymsat_old/VarHeap.cc
  src/ymsat_old/YmSat_base.cc
  src/ymsat_old/YmSat_solve.cc
  src/ymsat_old/YmSatMS2.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
ym_add_object_library ( ym_sat
  ${ym_sat_SOURCES}
  )
