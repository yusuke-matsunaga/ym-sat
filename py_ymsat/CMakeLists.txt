
# ===================================================================
# パッケージの検査
# ===================================================================


# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================

set ( py_ymsat_SOURCES
  PySatBool3.cc
  ymsat_module.cc
  py_ymsat.cc
  )


# ===================================================================
#  library ターゲットの設定
# ===================================================================

add_executable( py_ymsat
  ${py_ymsat_SOURCES}
  $<TARGET_OBJECTS:ym_sat_obj>
  $<TARGET_OBJECTS:ym_base_obj>
  ${YM_SUBMODULE_OBJ_LIST}
  )

target_compile_definitions ( py_ymsat
  PRIVATE "-DNODEBUG"
  )

target_include_directories ( py_ymsat
  PRIVATE
  ${Python3_INCLUDE_DIRS}
  )

target_link_libraries ( py_ymsat
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )

target_link_options( py_ymsat
  PRIVATE "-pthread"
  )

add_executable( py_ymsat_d
  ${py_ymsat_SOURCES}
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  ${YM_SUBMODULE_OBJ_D_LIST}
  )

target_compile_options ( py_ymsat_d
  PRIVATE "-g"
  )

target_include_directories ( py_ymsat_d
  PRIVATE
  ${Python3_INCLUDE_DIRS}
  )

target_link_libraries ( py_ymsat_d
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )


# ===================================================================
# テストターゲット
# ===================================================================
if (0)
add_test ( py_ymsat_test
  py_ymsat -m pytest ${CMAKE_CURRENT_SOURCE_DIR}/tests
  )

set_property ( TEST py_ymsat_test
  PROPERTY ENVIRONMENT TESTDATA_DIR=${PROJECT_SOURCE_DIR}/testdata/kifu
  )
endif (0)