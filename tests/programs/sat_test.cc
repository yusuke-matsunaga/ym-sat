
/// @file sat_text.cc
/// @brief sat_text の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM

void
sat_test()
{
  SatSolver solver{SatSolverType{"ymsat2"}};

  auto var0 = solver.new_variable(true);
  auto var1 = solver.new_variable(true);
  auto var2 = solver.new_variable(true);
  auto var3 = solver.new_variable(true);
  auto var4 = solver.new_variable(true);
  auto var5 = solver.new_variable(true);

  solver.add_buffgate(var1, var2);
  solver.add_orgate(var2, var3, var4);
  solver.add_notgate(var3, var0);
  solver.add_notgate(var4, var5);
  solver.add_notgate(var5, var0);

  vector<SatLiteral> assumptions{~var2};

  auto ret = solver.solve(assumptions);
}

END_NAMESPACE_YM

int
main(
  int argc,
  char** argv
)
{
  YM_NAMESPACE::sat_test();

  return 0;
}
