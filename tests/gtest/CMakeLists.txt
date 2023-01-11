
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

ym_add_gtest ( sat_add_clause_test
  add_clause_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_tseitinenc_test
  tseitinenc_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_adder_test
  adder_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_counter_test
  counter_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_at_most_test
  at_most_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_at_least_test
  at_least_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_exact_test
  exact_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_not_one_test
  not_one_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_eq_test
  eq_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_eq_const_test
  eq_const_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_ne_test
  ne_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_ne_const_test
  ne_const_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_lt_test
  lt_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_lt_const_test
  lt_const_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_le_test
  le_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_le_const_test
  le_const_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_gt_test
  gt_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_gt_const_test
  gt_const_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_ge_test
  ge_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_base_obj_d>
  $<TARGET_OBJECTS:ym_sat_obj_d>
  )

ym_add_gtest ( sat_ge_const_test
  ge_const_test.cc
  SatTestFixture.cc
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
