
/// @file SatInitParamTest.cc
/// @brief SatInitParamTest の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/SatInitParam.h"


BEGIN_NAMESPACE_YM

TEST(SatInitParamTest, const1)
{
  string type{"ymsat2"};
  SatInitParam param{type};

  EXPECT_EQ( type, param.type() );
}

TEST(SatInitParamTest, const2)
{
  const char* type{"ymsat2"};
  SatInitParam param{type};

  EXPECT_EQ( string{type}, param.type() );
}

TEST(SatInitParamTest, const3)
{
  string type{"ymsat2"};
  auto js_obj = JsonValue::Object();
  js_obj.emplace("type", JsonValue{type});
  SatInitParam param{js_obj};

  EXPECT_EQ( string{type}, param.type() );
}

TEST(SatInitParamTest, const4)
{
  string type{"ymsat2"};
  JsonValue js_obj{unordered_map<string, JsonValue>{{"type", JsonValue{type}},
						    {"foo", JsonValue{"foo"}}}};
  SatInitParam param{js_obj};

  EXPECT_EQ( string{type}, param.type() );

  auto js_obj2 = param.js_obj();
  EXPECT_EQ( js_obj, js_obj2 );
}

TEST(SatInitParamTest, bad_const1)
{
  string type{"badsat"};
  ASSERT_THROW({
      SatInitParam param{type};
    }, std::invalid_argument);
}

TEST(SatInitParamTest, bad_const2)
{
  const char* type{"badsat"};
  ASSERT_THROW({
      SatInitParam param{type};
    }, std::invalid_argument);
}

TEST(SatInitParamTest, bad_const3)
{
  string type{"badsat"};
  JsonValue js_obj{unordered_map<string, JsonValue>{{"type", type}}};
  ASSERT_THROW({
      SatInitParam param{js_obj};
    }, std::invalid_argument);
}

TEST(SatInitParamTest, from_json1)
{
  string data_dir{DATA_DIR};
  string filename{"ymsat.json"};
  string path{data_dir + "/" + filename};

  auto param = SatInitParam::from_json(path);

  EXPECT_EQ( "glueminisat2", param.type() );

  auto js_obj = param.js_obj();
  EXPECT_EQ( "bar", js_obj["foo"].get_string() );
}

TEST(SatInitParamTest, from_json2)
{
  ASSERT_THROW({
      auto param = SatInitParam::from_json("/foo/bar");
    }, std::invalid_argument);
}

END_NAMESPACE_YM
