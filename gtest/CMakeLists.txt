
# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories(
  ${GTEST_INCLUDE_DIR}
  )


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================



# ===================================================================
#  テスト用のターゲットの設定
# ===================================================================

add_executable ( SatVarIdTest
  SatVarIdTest.cc
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_sat_d>
  )

target_compile_options ( SatVarIdTest
  PRIVATE "-g"
  )

target_link_libraries ( SatVarIdTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( SatSatVarIdTest
  SatVarIdTest
  )

add_executable ( SatLiteralTest
  SatLiteralTest.cc
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_sat_d>
  )

target_compile_options ( SatLiteralTest
  PRIVATE "-g"
  )

target_link_libraries ( SatLiteralTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( SatSatLiteralTest
  SatLiteralTest
  )

add_executable ( AddClauseTest
  AddClauseTest.cc
  $<TARGET_OBJECTS:ym_base_d>
  $<TARGET_OBJECTS:ym_sat_d>
  )

target_compile_options ( AddClauseTest
  PRIVATE "-g"
  )

target_link_libraries ( AddClauseTest
  ${YM_LIB_DEPENDS}
  pthread
  ${GTEST_BOTH_LIBRARIES}
  )

add_test ( SatAddClauseTest
  AddClauseTest
  )
