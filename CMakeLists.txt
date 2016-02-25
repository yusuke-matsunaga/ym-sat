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
  src/ymsat/VarHeap.cc
  src/ymsat/YmSat_base.cc
  src/ymsat/YmSat_solve.cc
  src/ymsat/YmSatMS2.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
#ym_add_library (ym_sat
#  SHARED
#  ${SAT_SOURCES}
#  DESTINATION ${YM_INSTALL_LIBDIR}
#  )

add_library ( ym_sat
  OBJECT
  ${ym_sat_SOURCES}
  )

target_compile_options ( ym_sat
  PRIVATE "-O3" "-fPIC"
  )

target_compile_definitions ( ym_sat
  PRIVATE "-DNDEBUG"
  )
