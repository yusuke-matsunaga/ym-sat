
/// @file SatDimacs.cc
/// @brief SatDimacs の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatDimacs.h"
#include "ym/FileIDO.h"
#include "ym/StreamIDO.h"
#include "DimacsScanner.h"


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
      decode_lit(lit, var, pol);
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
  StreamIDO ido(s);

  return _read_dimacs(ido);
}

// @brief DIMACS 形式のファイルを読んで SatDimacs に設定する．
// @param[in] filename ファイル名
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
SatDimacs::read_dimacs(const char* filename)
{
  FileIDO ido;
  if ( !ido.open(filename) ) {
    return false;
  }

  return _read_dimacs(ido);
}

// @brief DIMACS 形式のファイルを読んで SatDimacs に設定する．
// @param[in] filename ファイル名
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
SatDimacs::read_dimacs(const string& filename)
{
  FileIDO ido;
  if ( !ido.open(filename) ) {
    return false;
  }

  return _read_dimacs(ido);
}

// @brief DIMACS 形式のファイルを読んで SatDimacs に設定する．
// @param[in] ido 入力元のデータオブジェクト
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
SatDimacs::_read_dimacs(IDO& ido)
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
  int dec_nv = 0;
  // 宣言された節の数
  int dec_nc = 0;

  // 実際に読み込んだ変数の最大値
  int max_v = 0;
  // 実際に読み込んだ節の数
  int act_nc = 0;

  vector<int> lits;

  DimacsScanner scanner(ido);

  clear();

  for ( ; ; ) {
    FileRegion loc;
    Token tk = scanner.read_token(loc);
    if ( tk == Token::kERR ) {
      return false;
    }
    if ( tk == Token::kC ) {
      // コメント行なのでなにもしない．
      continue;
    }

    switch ( state ) {
    case ST_INIT:
      if ( tk == Token::kP ) {
	state = ST_P1;
      }
      break;

    case ST_P1:
      if ( tk != Token::kNUM ) {
	goto p_error;
      }
      dec_nv = scanner.cur_val();
      state = ST_P2;
      break;

    case ST_P2:
      if ( tk != Token::kNUM ) {
	goto p_error;
      }
      dec_nc = scanner.cur_val();
      state = ST_P3;
      break;

    case ST_P3:
      if ( tk != Token::kNL ) {
	goto p_error;
      }
      // p 行は無視する．
      state = ST_BODY1;
      break;

    case ST_BODY1:
      if ( tk == Token::kP ) {
#if 0
	put_msg(__FILE__, __LINE__,
		loc,
		kMsgError,
		"ERR01",
		"duplicated 'p' lines");
#endif
	cout << "ERR01: duplicated 'p' lines" << endl;
	return false;
      }
      if ( tk == Token::kEOF ) {
	goto normal_end;
      }
      if ( tk == Token::kNL ) {
	continue;
      }
      if ( tk == Token::kNUM ) {
	int v = scanner.cur_val();
	lits.clear();
	lits.push_back(v);
	if ( v < 0 ) {
	  v = - v;
	}
	if ( max_v < v ) {
	  max_v = v;
	}
	state = ST_BODY2;
	break;
      }
      // それ以外はエラー
      goto n_error;

    case ST_BODY2:
      if ( tk == Token::kZERO ) {
	state = ST_BODY3;
      }
      else if ( tk == Token::kNL ) {
	continue;
      }
      else if ( tk == Token::kNUM ) {
	int v = scanner.cur_val();
	lits.push_back(v);
	if ( v < 0 ) {
	  v = - v;
	}
	if ( max_v < v ) {
	  max_v = v;
	}
      }
      else {
	goto n_error;
      }
      break;

    case ST_BODY3:
      if ( tk != Token::kNL && tk != Token::kEOF ) {
	goto n_error;
      }
      ++ act_nc;
      add_clause(lits);
      state = ST_BODY1;
      break;
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
