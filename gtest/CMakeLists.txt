
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

ym_add_gtest ( SatBool3Test
  SatBool3Test.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  )

ym_add_gtest ( SatVarIdTest
  SatVarIdTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  )

ym_add_gtest ( SatLiteralTest
  SatLiteralTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  )

ym_add_gtest ( AddClauseTest
  AddClauseTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  )

ym_add_gtest ( DimacsTest
  DimacsTest.cc
  $<TARGET_OBJECTS:ym_base_ad>
  $<TARGET_OBJECTS:ym_sat_ad>
  )
