
# ===================================================================
# インクルードパスの設定
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================


# ===================================================================
#  テスト用のターゲットの設定
# ===================================================================

add_executable ( sat_test1
  sat_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

target_compile_options ( sat_test1
  PRIVATE "-g"
  )

target_link_libraries ( sat_test1
  ${YM_LIB_DEPENDS}
  )

add_executable ( sat_timer_test1
  timer_test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_logic_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

target_compile_options ( sat_timer_test1
  PRIVATE "-g"
  )

target_link_libraries ( sat_timer_test1
  ${YM_LIB_DEPENDS}
  )
