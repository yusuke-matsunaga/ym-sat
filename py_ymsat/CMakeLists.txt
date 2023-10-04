
# ===================================================================
# パッケージの検査
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${Python3_INCLUDE_DIRS}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( py_ymsat_SOURCES
  ymsat_module.cc
  PySatBool3.cc
  PySatLiteral.cc
  PySatSolver.cc
  )


# ===================================================================
#  library ターゲットの設定
# ===================================================================

ym_add_object_library( py_ymsat
  ${py_ymsat_SOURCES}
  )

add_library( py_ymsatlib
  SHARED
  $<TARGET_OBJECTS:py_ymsat_obj>
  $<TARGET_OBJECTS:ym_sat_obj>
  #$<TARGET_OBJECTS:ym_base_obj>
  $<TARGET_OBJECTS:ym_json_obj>
  )

set_target_properties( py_ymsatlib
  PROPERTIES
  PREFIX ""
  OUTPUT_NAME "ymsat"
  )

target_link_libraries( py_ymsatlib
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )


# ===================================================================
# インストールターゲット
# ===================================================================

install( TARGETS py_ymsatlib
  LIBRARY DESTINATION lib/${YM_PYTHON_STRING}/site-packages
  )