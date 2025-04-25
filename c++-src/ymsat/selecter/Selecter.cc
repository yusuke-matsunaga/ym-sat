
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
