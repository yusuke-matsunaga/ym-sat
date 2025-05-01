
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ymsat/include
  )


# ===================================================================
#  マクロの定義
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================

add_subdirectory( glueminisat-2.2.8 )
add_subdirectory( lingeling )
add_subdirectory( MiniSat )
add_subdirectory( MiniSat2 )
add_subdirectory( ymsat )
add_subdirectory( ymsat_old )

add_subdirectory( gtest )


# ===================================================================
#  ソースの設定
# ===================================================================

set (main_SOURCES
  SatMsgHandlerS.cc
  SatSolver.cc
  SatSolver_bv.cc
  SatSolver_count.cc
  SatSolver_tseitin.cc
  SatSolverImpl.cc
  SatInitParam.cc
  SatLogger.cc
  SatLoggerS.cc
  SatOrderedSet.cc
  SatDimacs.cc
  Expr2Cnf.cc
  Aig2Cnf.cc
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
ym_add_object_library ( ym_sat
  ${main_SOURCES}
  ${glueminisat_SOURCES}
  ${lingeling_SOURCES}
  ${MiniSat_SOURCES}
  ${MiniSat2_SOURCES}
  ${ymsat_SOURCES}
  ${ymsat_old_SOURCES}
  )
