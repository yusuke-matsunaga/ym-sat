
/// @file DimacsScanner.cc
/// @brief DimacsScanner の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "DimacsScanner.h"


BEGIN_NAMESPACE_YM_SAT

BEGIN_NONAMESPACE
const bool debug_read_token = false;
END_NONAMESPACE


//////////////////////////////////////////////////////////////////////
// DimacsScanner
//////////////////////////////////////////////////////////////////////

// コンストラクタ
DimacsScanner::DimacsScanner(IDO& ido) :
  Scanner(ido)
{
}

// デストラクタ
DimacsScanner::~DimacsScanner()
{
}

// @brief トークンの読込み
// @param[out] loc 対応するファイル上の位置情報を格納する変数
Token
DimacsScanner::read_token(FileRegion& loc)
{
  Token token = scan();
  loc = cur_loc();

  if ( debug_read_token ) {
    cerr << "read_token() --> "
	 << loc << ": ";
    switch ( token ) {
    case Token::C:     cerr << "C"; break;
    case Token::P:     cerr << "P"; break;
    case Token::NUM:   cerr << "NUM(" << mCurVal << ")"; break;
    case Token::ZERO:  cerr << "ZERO"; break;
    case Token::NL:    cerr << "NL"; break;
    case Token::E_O_F: cerr << "EOF"; break;
    case Token::ERR:   cerr << "ERR"; break;
    }
    cerr << endl;
  }

  return token;
}

// @brief read_token() の下請け関数
// @return トークンを返す．
Token
DimacsScanner::scan()
{
  // トークンとは空白もしくは改行で区切られたもの
  // とりあえずそれ以外の文字はすべてトークンの構成要素とみなす．
  // 改行は単独のトークンとみなす．
  // EOF も単独のトークンとみなす．

  // 最初の空白文字をスキップする．
  int c;
  for ( ; ; ) {
    c = get();
    if ( c == EOF ) {
      return Token::E_O_F;
    }
    if ( c == '\n' ) {
      return Token::NL;
    }
    if ( c != ' ' && c != '\t' ) {
      break;
    }
  }

  if ( c == 'c' ) {
    // 手抜きで次の NL まで読み飛ばす．
    while ( (c = get()) != '\n' && c != EOF ) ;
    return Token::C;
  }
  if ( c == 'p' ) {
    c = get();
    // 次は空白でなければならない．
    if ( c != ' ' && c != '\t' ) {
      goto p_error;
    }
    // 続く空白を読み飛ばす．
    for ( ; ; ) {
      c = get();
      if ( c != ' ' && c != '\t' ) {
	break;
      }
    }
    // 次は "cnf" のはず．
    if ( c != 'c' ) {
      goto p_error;
    }
    c = get();
    if ( c != 'n' ) {
      goto p_error;
    }
    c = get();
    if ( c != 'f' ) {
      goto p_error;
    }
    c = get();
    // 次は空白のはず．
    if ( c != ' ' && c != '\t' ) {
      goto p_error;
    }
    return Token::P;

  p_error:
#if 0
    put_msg(__FILE__, __LINE__,
	    cur_loc(),
	    kMsgError,
	    "ERR05",
	    "syntax error");
#endif
    return Token::ERR;
  }

  bool minus_flag = false;
  if ( c == '-' ) {
    minus_flag = true;
    c = get();
  }

  int val = 0;
  for ( ; ; ) {
    if ( c < '0' || '9' < c ) {
#if 0
      put_msg(__FILE__, __LINE__,
	      cur_loc(),
	      kMsgError,
	      "ERR06",
	      "syntax error");
#endif
      return Token::ERR;
    }
    val = val * 10 + (c - '0');

    c = peek();
    if ( c == ' ' || c == '\t' || c == '\n' || c == EOF ) {
      if ( minus_flag ) {
	mCurVal = - val;
      }
      else {
	mCurVal = val;
      }
      if ( mCurVal == 0 ) {
	return Token::ZERO;
      }
      return Token::NUM;
    }
    accept();
  }
}

END_NAMESPACE_YM_SAT
