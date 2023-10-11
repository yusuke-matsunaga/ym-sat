
/// @file SatDimacs.cc
/// @brief SatDimacs の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatDimacs.h"


BEGIN_NAMESPACE_YM_SAT

// @brief 評価する．
bool
SatDimacs::eval(
  const vector<int>& model
) const
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
void
SatDimacs::write_dimacs(
  ostream& s
) const
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
bool
SatDimacs::read_dimacs(
  istream& s
)
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
  regex pat_L{R"(^\s*(-?\d+))"};

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
	ostringstream buf;
	buf << "Error at line " << lineno << ": duplicated 'p' block";
	mMessageList.push_back(buf.str());
	return false;
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
	ostringstream buf;
	buf << "Error at line " << lineno << ": syntax error";
	mMessageList.push_back(buf.str());
	return false;
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

  if ( dec_nv == 0 ) {
    ostringstream buf;
    buf << "Error at line " << lineno << ": unexpected end-of-file";
    mMessageList.push_back(buf.str());
    return false;
  }
  if ( dec_nv < max_v ) {
    auto msg = "Warning: actual number of variables is more than the declared";
    mMessageList.push_back(msg);
  }
  if ( dec_nc > act_nc ) {
    auto msg = "Warning: actual number of clauses is less than the declared";
    mMessageList.push_back(msg);
  }
  else if ( dec_nc < act_nc ) {
    auto msg = "Warning: actual number of clauses is more than the declared";
    mMessageList.push_back(msg);
  }

  return true;
}

END_NAMESPACE_YM_SAT
