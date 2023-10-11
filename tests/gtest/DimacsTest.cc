
/// @file DimacsTest.cc
/// @brief SatDimacst のテストプログラム
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.


#include "gtest/gtest.h"
#include "ym/SatDimacs.h"


BEGIN_NAMESPACE_YM

TEST(DimacsTest, null)
{
  SatDimacs dimacs;

  EXPECT_EQ( 0, dimacs.variable_num() );
  EXPECT_EQ( 0, dimacs.clause_num() );
  EXPECT_TRUE( dimacs.clause_list().empty() );
}

TEST(DimacsTest, add_clause1)
{
  SatDimacs dimacs;

  vector<int> tmp_lits;
  tmp_lits.push_back(1);

  dimacs.add_clause(tmp_lits);

  EXPECT_EQ( 1, dimacs.variable_num() );
  EXPECT_EQ( 1, dimacs.clause_num() );

  const vector<int>& lits1 = dimacs.clause_list()[0];
  EXPECT_EQ( 1, lits1.size() );
  EXPECT_EQ( 1, lits1[0] );
}

TEST(DimacsTest, add_clause2)
{
  SatDimacs dimacs;

  vector<int> tmp_lits;
  tmp_lits.push_back(1);
  tmp_lits.push_back(-2);

  dimacs.add_clause(tmp_lits);

  EXPECT_EQ( 2, dimacs.variable_num() );
  EXPECT_EQ( 1, dimacs.clause_num() );

  const vector<int>& lits1 = dimacs.clause_list()[0];
  EXPECT_EQ( 2, lits1.size() );
  EXPECT_EQ( 1, lits1[0] );
  EXPECT_EQ( -2, lits1[1] );
}

TEST(DimacsTest, read_dimacs)
{
  SatDimacs dimacs;

  string data_dir{DATA_DIR};
  string filename{"uf20-01.cnf"};
  string path{data_dir + "/" + filename};
  ifstream fin(path);
  bool stat = dimacs.read_dimacs(fin);
  EXPECT_TRUE( stat );
}

TEST(DimacsTest, read_dimacs_err1)
{
  SatDimacs dimacs;

  const char* data_str =
    "1 2 3 0\n"
    "%\n"
    "0";
  istringstream buf{data_str};
  bool stat = dimacs.read_dimacs(buf);
  EXPECT_FALSE( stat );
  auto& msg_list = dimacs.message_list();
  auto& msg = msg_list.front();
  EXPECT_EQ( "Error at line 2: unexpected end-of-file", msg );
}

TEST(DimacsTest, read_dimacs_err2)
{
  SatDimacs dimacs;

  const char* data_str =
    "p cnf 10 20\n"
    "p cnf 20 30\n"
    "%\n"
    "0";
  istringstream buf{data_str};
  bool stat = dimacs.read_dimacs(buf);
  EXPECT_FALSE( stat );
  auto& msg_list = dimacs.message_list();
  auto& msg = msg_list.front();
  EXPECT_EQ( "Error at line 2: duplicated 'p' block", msg );
}

TEST(DimacsTest, read_dimacs_err3)
{
  SatDimacs dimacs;

  const char* data_str =
    "p cnf 10 20\n"
    "1 2 0\n"
    "abc 0\n"
    "%\n"
    "0";
  istringstream buf{data_str};
  bool stat = dimacs.read_dimacs(buf);
  EXPECT_FALSE( stat );
  auto& msg_list = dimacs.message_list();
  auto& msg = msg_list.front();
  EXPECT_EQ( "Error at line 3: syntax error", msg );
}

TEST(DimacsTest, read_dimacs_err4)
{
  SatDimacs dimacs;

  const char* data_str =
    "p cnf 10 20\n"
    "1 2 0\n"
    "-2 abc 0\n"
    "%\n"
    "0";
  istringstream buf{data_str};
  bool stat = dimacs.read_dimacs(buf);
  EXPECT_FALSE( stat );
  auto& msg_list = dimacs.message_list();
  auto& msg = msg_list.front();
  EXPECT_EQ( "Error at line 3: syntax error", msg );
}

END_NAMESPACE_YM
