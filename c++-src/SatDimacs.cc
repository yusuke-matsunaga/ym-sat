
/// @file SatDimacs.cc
/// @brief SatDimacs の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatDimacs.h"


BEGIN_NAMESPACE_YM_SAT

// @brief 評価する．
// @param[in] model モデル
// @retval true 充足した．
// @retval false 充足しなかった．
//
// model[i] に i 番目の変数の値 ( 0 or 1 ) を入れる．
bool
SatDimacs::eval(const vector<int>& model) const
{
  for ( auto lit_list: mClauseList ) {
    bool sat = false;
    for ( auto lit: lit_list ) {
      int var;
      int pol;
      tie(var, pol) = decode_lit(lit);
      if ( model[var] == pol ) {
	sat = true;
	break;
      }
    }
    if ( !sat ) {
      // この節は充足していない．
      return false;
    }
  }
  return true;
}

// @brief 内容を DIMACS 形式で出力する．
// @param[in] s 出力先のストリーム
void
SatDimacs::write_dimacs(ostream& s) const
{
  s << "p cnf " << variable_num() << " " << clause_num() << endl;
  for ( auto lit_list: mClauseList ) {
    const char* sp = "";
    for ( auto lit: lit_list ) {
      s << sp << lit;
      sp = " ";
    }
    s << " 0" << endl;
  }
}

// @brief DIMACS 形式のファイルを読んで SatDimacs に設定する．
// @param[in] s 入力元のストリーム
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
SatDimacs::read_dimacs(istream& s)
{
  // 読込用の内部状態
  enum {
    ST_INIT,
    ST_P1,
    ST_P2,
    ST_P3,
    ST_BODY1,
    ST_BODY2,
    ST_BODY3
  } state = ST_INIT;

  // 宣言された変数の数
  int dec_nv{0};
  // 宣言された節の数
  int dec_nc{0};

  // 実際に読み込んだ変数の最大値
  int max_v{0};
  // 実際に読み込んだ節の数
  int act_nc{0};

  clear();

  // コメント行のパタン
  regex pat_C{R"(^c)"};

  // 先頭行のパタン
  regex pat_P{R"(^p\s+cnf\s+(\d+)\s+(\d+)\s*$)"};

  // end-marker ? のパタン
  regex pat_E{R"(^%)"};

  // リテラルのパタン
  regex pat_L{R"(\s*(-?\d+))"};

  // 行バッファ
  string buff;
  // 行番号
  int lineno = 0;
  while ( getline(s, buff) ) {
    ++ lineno;
    if ( regex_search(buff, pat_C) ) {
      // コメント行だった．
      continue;
    }
    smatch match;
    if ( regex_match(buff, match, pat_P) ) {
      if ( dec_nv > 0 ) {
	// p 行が重複していた．
#warning "TODO: エラーメッセージを出力する．"
      }
      dec_nv = stoi(match[1]);
      dec_nc = stoi(match[2]);
      continue;
    }

    if ( regex_search(buff, pat_E) ) {
      break;
    }

    vector<int> lits;
    auto start = buff.cbegin();
    auto end = buff.cend();
    while ( true ) {
      if ( !regex_search(start, end, match, pat_L) ) {
	// シンタックスエラー
#warning "TODO: エラーメッセージを出力する．"
      }
      int lit = stoi(match[1]);
      if ( lit == 0 ) {
	// end-mark
	add_clause(lits);
	++ act_nc;
	break;
      }
      lits.push_back(lit);
      int v = abs(lit);
      if ( max_v < v ) {
	max_v = v;
      }
      start = match[0].second;
    }
  }
  normal_end:
  if ( dec_nv == 0 ) {
#if 0
    add_msg(__FILE__, __LINE__,
	    loc,
	    kMsgError,
	    "ERR02",
	    "unexpected end-of-file");
#endif
    cout << "ERR02: unexpected end-of-file" << endl;
    goto error;
  }
  if ( dec_nv < max_v ) {
#if 0
    add_msg(__FILE__, __LINE__,
	    loc,
	    kMsgWarning,
	    "WRN01",
	    "actual number of variables is more than the declared");
#endif
  }
  if ( dec_nc > act_nc ) {
#if 0
    add_msg(__FILE__, __LINE__,
	    loc,
	    kMsgWarning,
	    "WRN02",
	    "actual number of clauses is less than the declared");
#endif
  }
  else if ( dec_nc < act_nc ) {
#if 0
    add_msg(__FILE__, __LINE__,
	    loc,
	    kMsgWarning,
	    "WRN03",
	    "actual number of clauses is more than the declared");
#endif
  }

  return true;

 p_error:
#if 0
  add_msg(__FILE__, __LINE__,
	  loc,
	  kMsgError,
	  "ERR03",
	  "syntax error \"p cnf <num of vars> <num of clauses>\" expected");
#endif
  cout << "ERR03 : "
       << "syntax error \"p cnf <num of vars> <num of clauses>\" expected"
       << endl;

  goto error;

 n_error:
#if 0
  add_msg(__FILE__, __LINE__,
	  loc,
	  kMsgError,
	  "ERR04",
	  "syntax error \"<lit_1> <lit_2> ... <lit_n> 0\" expected");
#endif
  cout << "ERR04 : "
       << "syntax error \"<lit_1> <lit_2> ... <lit_n> 0\" expected"
       << endl;
  goto error;

 error:

  return false;
}

END_NAMESPACE_YM_SAT
