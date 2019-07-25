
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

ym_add_gtest ( sat_SatBool3_test
  SatBool3Test.cc
  $<TARGET_OBJECTS:ym_base_obj_ad>
  $<TARGET_OBJECTS:ym_sat_obj_ad>
  )

ym_add_gtest ( sat_SatVarId_test
  SatVarIdTest.cc
  $<TARGET_OBJECTS:ym_base_obj_ad>
  $<TARGET_OBJECTS:ym_sat_obj_ad>
  )

ym_add_gtest ( sat_SatLiteral_test
  SatLiteralTest.cc
  $<TARGET_OBJECTS:ym_base_obj_ad>
  $<TARGET_OBJECTS:ym_sat_obj_ad>
  )

ym_add_gtest ( sat_AddClause_test
  AddClauseTest.cc
  $<TARGET_OBJECTS:ym_base_obj_ad>
  $<TARGET_OBJECTS:ym_sat_obj_ad>
  )

ym_add_gtest ( sat_Dimacs_test
  DimacsTest.cc
  $<TARGET_OBJECTS:ym_base_obj_ad>
  $<TARGET_OBJECTS:ym_sat_obj_ad>
  )
