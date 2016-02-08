# ===================================================================
# CMAKE のおまじない
# ===================================================================


# ===================================================================
# プロジェクト名，バージョンの設定
# ===================================================================

# 名前空間の定義
set ( SAT_NAMESPACE "nsSat" )


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
include_directories (
  ${PROJECT_SOURCE_DIR}/include
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory ( cython-src )


# ===================================================================
#  ソースの設定
# ===================================================================

set (SAT_SOURCES
  src/sat/SatMsgHandlerImpl1.cc
  src/sat/SatSolver.cc
  src/sat/SatStats.cc

  src/sat/MiniSat/SatSolverMiniSat.cc
  src/sat/MiniSat/Solver.C

  src/sat/MiniSat2/Options.cc
  src/sat/MiniSat2/SatSolverMiniSat2.cc
  src/sat/MiniSat2/Solver.cc
  src/sat/MiniSat2/System.cc

  src/sat/glueminisat-2.2.8/SatSolverGlueMiniSat2.cc
  src/sat/glueminisat-2.2.8/Options.cc
  src/sat/glueminisat-2.2.8/Solver.cc
  src/sat/glueminisat-2.2.8/System.cc

  src/sat/dimacs/DimacsParser.cc
  src/sat/dimacs/DimacsScanner.cc
  src/sat/dimacs/DimacsVerifier.cc
  src/sat/dimacs/SatDimacsHandler.cc

  src/sat/ymsat/AssignList.cc
  src/sat/ymsat/SaBase.cc
  src/sat/ymsat/SaFactory.cc
  src/sat/ymsat/SaUIP1.cc
  src/sat/ymsat/SaUIP2.cc
  src/sat/ymsat/SatClause.cc
  src/sat/ymsat/VarHeap.cc
  src/sat/ymsat/YmSat_base.cc
  src/sat/ymsat/YmSat_solve.cc
  src/sat/ymsat/YmSatMS2.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
ym_add_library (ym_sat
  SHARED
  ${SAT_SOURCES}
  DESTINATION lib
  )
