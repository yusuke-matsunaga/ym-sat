
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
  ../../py_ymsat/ymsat_module.cc
  py_ymsat.cc
  )


# ===================================================================
#  executable ターゲットの設定
# ===================================================================

add_executable( py_ymsat
  ${py_ymsat_SOURCES}
  $<TARGET_OBJECTS:py_ymsat_obj>
  $<TARGET_OBJECTS:ym_sat_obj>
  $<TARGET_OBJECTS:ym_base_obj>
  )

target_compile_options( py_ymsat
  PRIVATE "-O3"
  )

target_compile_definitions( py_ymsat
  PRIVATE "-DNODEBUG"
  )

target_link_libraries ( py_ymsat
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )

add_executable( py_ymsat_d
  ${py_ymsat_SOURCES}
  $<TARGET_OBJECTS:py_ymsat_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

target_compile_options( py_ymsat_d
  PRIVATE "-g"
  )

target_link_libraries ( py_ymsat_d
  ${YM_LIB_DEPENDS}
  ${Python3_LIBRARIES}
  )


# ===================================================================
# テストターゲット
# ===================================================================

add_test( py_ymsat_test
  py_ymsat -m pytest ${CMAKE_CURRENT_SOURCE_DIR}
  )
