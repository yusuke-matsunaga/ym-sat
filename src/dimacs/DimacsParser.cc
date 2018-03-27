
/// @file DimacsParser.cc
/// @brief DimacsParser の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/DimacsParser.h"
#include "ym/DimacsHandler.h"
#include "DimacsScanner.h"
#include "ym/MsgMgr.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// DimacsParser
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
DimacsParser::DimacsParser()
{
}

// @brief デストラクタ
DimacsParser::~DimacsParser()
{
}

// @brief 読み込みを行なう．
// @param[in] ido 入力データ
// @retval true 読み込みが成功した．
// @retval false 読み込みが失敗した．
bool
DimacsParser::read(IDO& ido)
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

  bool stat = true;
  for ( auto handler: mHandlerList ) {
    if ( !handler->init() ) {
      stat = false;
    }
  }
  if ( !stat ) {
    goto error;
  }

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
      for ( auto handler: mHandlerList ) {
	if ( !handler->read_p(loc, dec_nv, dec_nc) ) {
	  stat = false;
	}
      }
      if ( !stat ) {
	goto error;
      }
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
      for ( auto handler: mHandlerList ) {
	if ( !handler->read_clause(loc, lits) ) {
	  stat = false;
	}
      }
      if ( !stat ) {
	goto error;
      }
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

  for ( auto handler: mHandlerList ) {
    if ( !handler->end() ) {
      stat = false;
    }
  }
  if ( !stat ) {
    goto error;
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
  for ( auto handler: mHandlerList ) {
    handler->error_exit();
  }

  return false;
}

// @brief イベントハンドラの登録
void
DimacsParser::add_handler(DimacsHandler* handler)
{
  mHandlerList.push_back(handler);
}

END_NAMESPACE_YM_SAT
