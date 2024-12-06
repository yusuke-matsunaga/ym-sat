
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

set (ymsat_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/core/Clause.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/core/SatCore.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/core/VarHeap.cc

  ${CMAKE_CURRENT_SOURCE_DIR}/controller/Controller.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/controller/ControllerMS1.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/controller/ControllerMS2.cc

  ${CMAKE_CURRENT_SOURCE_DIR}/analyzer/Analyzer.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/analyzer/SaBase.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/analyzer/SaUIP1.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/analyzer/SaUIP2.cc

  ${CMAKE_CURRENT_SOURCE_DIR}/selecter/Selecter.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/selecter/SelBase.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/selecter/SelWlPosi.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/selecter/SelWlNega.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/selecter/SelPosi.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/selecter/SelNega.cc
  ${CMAKE_CURRENT_SOURCE_DIR}/selecter/SelRandom.cc
  PARENT_SCOPE
  )


# ===================================================================
#  ターゲットの設定
# ===================================================================
