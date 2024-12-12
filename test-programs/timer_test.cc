
/// @file timer_test.cc
/// @brief timer_test の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolver.h"
#include <libgen.h>


BEGIN_NAMESPACE_YM

void
timer_test(
  int argc,
  char**argv
)
{
  if ( argc != 2 ) {
    cerr << "USAGE: " << basename(argv[0])
	 << " <satsolver-name>(minisat|minisat2|glueminisat2|lingeling|ymsat|ymsat1|ymsat2|ymsat1_old"
	 << endl;
    return;
  }

  string name = argv[1];

  SatSolver solver{name};

  SizeType nh = 40;
  SizeType np = nh + 1;
  SizeType nv = nh * np;
  vector<SatLiteral> var_array;
  var_array.reserve(nv);
  for ( SizeType i = 0; i < nv; ++ i ) {
    auto var = solver.new_variable(true);
    var_array.push_back(var);
  }

  // 各鳩が必ずどれかの穴に入っている条件
  for ( SizeType p = 0; p < np; ++ p ) {
    vector<SatLiteral> lits;
    lits.reserve(nh);
    for ( SizeType h = 0; h < nh; ++ h ) {
      auto var = var_array[p * nh + h];
      lits.push_back(var);
    }
    solver.add_clause(lits);
  }

  // 各穴には１羽しか入っていない条件
  for ( SizeType h = 0; h < nh; ++ h ) {
    for ( SizeType p1 = 0; p1 < np - 1; ++ p1 ) {
      auto var1 = var_array[p1 * nh + h];
      for ( SizeType p2 = p1 + 1; p2 < np; ++ p2 ) {
	auto var2 = var_array[p2 * nh + h];
	solver.add_clause(~var1, ~var2);
      }
    }
  }

  auto ret = solver.solve(5);

  cout << ret << endl;
}

END_NAMESPACE_YM

int
main(
  int argc,
  char** argv
)
{
  YM_NAMESPACE::timer_test(argc, argv);

  return 0;
}
