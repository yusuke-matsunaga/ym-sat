
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

set (glueminisat_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/SatSolverGlueMiniSat2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Options.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/Solver.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/System.cc
  PARENT_SCOPE
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
