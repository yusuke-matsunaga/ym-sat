
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

set (ymsat_old_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/SaBase.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/SaFactory.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/SaUIP1.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/SaUIP2.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/YmSat.cc
  PARENT_SCOPE
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
