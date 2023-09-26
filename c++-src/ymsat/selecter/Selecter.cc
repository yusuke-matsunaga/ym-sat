
/// @file Selecter.cc
/// @brief Selecter の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "Selecter.h"
#include "SelWlPosi.h"
#include "SelWlNega.h"
#include "SelPosi.h"
#include "SelNega.h"
#include "SelRandom.h"
#include "ym/JsonValue.h"


BEGIN_NAMESPACE_YM_SAT

#if 0
BEGIN_NONAMESPACE

// string::find_first_of() のバリエーション
// 直前の '\' をエスケープ文字とみなす．
string::size_type
find_first_of(
  const string& input,
  char c
)
{
  string::size_type p = 0;
  string::size_type end = input.size();
  for ( ; p < end; ++ p ) {
    char c1 = input[p];
    if ( c1 == c ) {
      return p;
    }
    if ( c1 == '\\' ) {
      if ( p + 1 == end ) {
	// 末尾が \ だった．
	return string::npos;
      }
      // 次の文字を読み飛ばす．
      ++ p;
    }
  }
  return string::npos;
}

// 前後の空白を取り除く
string
strip_wspace(
  const string& input
)
{
  string::size_type p1 = 0;
  string::size_type end = input.size();
  for ( ; p1 < end; ++ p1 ) {
    if ( !isspace(input[p1]) ) {
      break;
    }
  }
  string::size_type p2 = end;
  for ( ; p2 > p1; -- p2 ) {
    if ( !isspace(input[p2 - 1]) ) {
      break;
    }
  }
  return input.substr(p1, p2);
}

END_NONAMESPACE
#endif

// @brief インスタンスを作るクラスメソッド
Selecter*
Selecter::new_obj(
  SatCore& core,
  const JsonValue& js_obj
)
{
  string type;
  double var_freq = 0.0;
  bool phase_cache = false;
  if ( js_obj.has_key("selector") ) {
    auto sel_obj = js_obj["selector"];
    if ( sel_obj.is_string() ) {
      type = sel_obj.get_string();
    }
    else if ( sel_obj.is_object() ) {
      if ( sel_obj.has_key("type") ) {
	type = sel_obj["type"].get_string();
      }
      if ( sel_obj.has_key("var_freq") ) {
	var_freq = sel_obj["var_freq"].get_float();
      }
      if ( sel_obj.has_key("phase_cache") ) {
	phase_cache = sel_obj["phase_cache"].get_bool();
      }
    }
    if ( type == "wlposi" ) {
      return new SelWlPosi{core, var_freq, phase_cache};
    }
    if ( type == "wlnega" ) {
      return new SelWlNega{core, var_freq, phase_cache};
    }
    if ( type == "posi" ) {
      return new SelPosi{core, var_freq, phase_cache};
    }
    if ( type == "nega" ) {
      return new SelNega{core, var_freq, phase_cache};
    }
    if ( type == "random" ) {
      return new SelRandom{core, var_freq, phase_cache};
    }
    else {
      cerr << type << ": unknown type, ignored. 'random' is used instead." << endl;
    }
  }
  return new SelRandom{core, var_freq, phase_cache};
}

END_NAMESPACE_YM_SAT
