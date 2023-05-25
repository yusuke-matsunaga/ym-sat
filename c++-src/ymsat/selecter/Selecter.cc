
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


BEGIN_NAMESPACE_YM_SAT

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

// @brief パースする．
unordered_map<string, string>
parse_option(
  const string& input
)
{
  unordered_map<string, string> ans_dict;
  // mDelim で区切る
  string tmp_str{input};
  for ( ; ; ) {
    string::size_type p = find_first_of(tmp_str, ',');
    auto tmp = strip_wspace(tmp_str.substr(0, p));
    // tmp を mOptDelim で区切る．
    string::size_type q = find_first_of(tmp, ':');
    if ( q == string::npos ) {
      // mOptDelim がなかった
      ans_dict.emplace(tmp, string{});
    }
    else {
      auto l_str = strip_wspace(tmp.substr(0, q));
      auto r_str = strip_wspace(tmp.substr(q + 1, string::npos));
      ans_dict.emplace(l_str, r_str);
    }
    if ( p == string::npos ) {
      // 末尾だったので終わる．
      break;
    }
    // tmp_str を切り詰める．
    tmp_str = tmp_str.substr(p + 1, string::npos);
  }

  return ans_dict;
}

END_NONAMESPACE

// @brief インスタンスを作るクラスメソッド
Selecter*
Selecter::new_obj(
  SatCore& core,
  const Json::Value& js_obj
)
{
  string type;
  double var_freq = 0.0;
  bool phase_cache = false;
  if ( js_obj.isMember("selector") ) {
    auto sel_obj = js_obj["selector"];
    if ( sel_obj.isString() ) {
      type = sel_obj.asString();
    }
    else if ( sel_obj.isObject() ) {
      if ( sel_obj.isMember("type") ) {
	type = sel_obj["type"].asString();
      }
      if ( sel_obj.isMember("var_freq") ) {
	var_freq = sel_obj["var_freq"].asDouble();
      }
      if ( sel_obj.isMember("phase_cache") ) {
	phase_cache = sel_obj["phase_cache"].asBool();
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
