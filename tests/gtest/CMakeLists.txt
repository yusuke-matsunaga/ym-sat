
# ===================================================================
# インクルードパスの設定
# ===================================================================


# ===================================================================
# サブディレクトリの設定
# ===================================================================


# ===================================================================
#  ソースファイルの設定
# ===================================================================


set ( DATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../testdata )

# ===================================================================
#  テスト用のターゲットの設定
# ===================================================================

ym_add_gtest ( sat_SatBool3_test
  SatBool3Test.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_SatLiteral_test
  SatLiteralTest.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_AddClause_test
  AddClauseTest.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_SatTseitinEnc_test
  SatTseitinEncTest.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_SatCountEnc_test
  SatCountEncTest.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_SatBvEnc_test
  SatBvEncTest.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_SatOrderedSet_test
  SatOrderedSetTest.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_Dimacs_test
  DimacsTest.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  DEFINITIONS
  "-DDATA_DIR=\"${DATA_DIR}\""
  )
