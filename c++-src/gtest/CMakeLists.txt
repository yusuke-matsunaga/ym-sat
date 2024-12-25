
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

ym_add_gtest ( sat_CnfSize_test
  CnfSizeTest.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_SatInitParam_test
  SatInitParamTest.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  DEFINITIONS
  "-DDATA_DIR=\"${DATA_DIR}\""
  )

ym_add_gtest ( sat_SatInitParam2_test
  SatInitParamEnvTest.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

set_property ( TEST sat_SatInitParam2_test
  PROPERTY ENVIRONMENT YMSAT_CONF=${DATA_DIR}/ymsat.json
  )

ym_add_gtest ( sat_SatInitParam3_test
  SatInitParamEnvTest.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

set_property ( TEST sat_SatInitParam3_test
  PROPERTY ENVIRONMENT YMSAT_CONFDIR=${DATA_DIR}
  )

ym_add_gtest ( sat_SatBool3_test
  SatBool3Test.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_SatLiteral_test
  SatLiteralTest.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_add_clause_test
  add_clause_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_tseitinenc_test
  tseitinenc_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_adder_test
  adder_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_counter_test
  counter_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_at_most_test
  at_most_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_comp_test
  comp_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_comp_const_test
  comp_const_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_conflict_test
  conflict_test.cc
  SatTestFixture.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_SatOrderedSet_test
  SatOrderedSetTest.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_Dimacs_test
  DimacsTest.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  DEFINITIONS
  "-DDATA_DIR=\"${DATA_DIR}\""
  )

ym_add_gtest ( sat_SatSolver_test
  SatSolver_test.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )

ym_add_gtest ( sat_timer_test
  timer_test.cc
  $<TARGET_OBJECTS:ym_sat_obj_d>
  $<TARGET_OBJECTS:ym_base_obj_d>
  )
