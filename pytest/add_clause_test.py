#! /usr/bin/env python3

"""

:file: add_clause_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
from ymsat import SatBool3, SatSolver
from solver_test import SolverTest


@pytest.fixture
def sat_test_fixture():
    return SolverTest()

def test_add_clause0(sat_test_fixture):
    vals = [1, 1]
    sat_test_fixture.check(1, vals)

def test_add_clause1_1(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    sat_test_fixture.solver.add_clause(lit1)
    vals = [0, 1]
    sat_test_fixture.check(1, vals)

def test_add_clause1_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    sat_test_fixture.solver.add_clause(~lit1)
    vals = [1, 0]
    sat_test_fixture.check(1, vals)

def test_add_clause2_1(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_clause(lit1, lit2)
    vals = [0, 1, 1, 1]
    sat_test_fixture.check(2, vals)

def test_add_clause2_1l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_clause([lit1, lit2])
    vals = [0, 1, 1, 1]
    sat_test_fixture.check(2, vals)

def test_add_clause2_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_clause(~lit1, lit2)
    vals = [1, 0, 1, 1]
    sat_test_fixture.check(2, vals)

def test_add_clause3_1(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_clause(lit1, lit2, lit3)
    vals = [0, 1, 1, 1, 1, 1, 1, 1]
    sat_test_fixture.check(3, vals)

def test_add_clause3_1l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_clause([lit1, lit2, lit3])
    vals = [0, 1, 1, 1, 1, 1, 1, 1]
    sat_test_fixture.check(3, vals)

def test_add_clause3_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_clause(lit1, lit2, lit3)
    vals = [0, 1, 1, 1, 1, 1, 1, 1]
    sat_test_fixture.check(3, vals)

def test_add_clause3_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_clause(~lit1, lit2, ~lit3)
    vals = [1, 1, 1, 1, 1, 0, 1, 1]
    sat_test_fixture.check(3, vals)

def test_add_clause4_1(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    sat_test_fixture.solver.add_clause(lit1, lit2, lit3, lit4)
    vals = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    sat_test_fixture.check(4, vals)

def test_add_clause4_1l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    sat_test_fixture.solver.add_clause([lit1, lit2, lit3, lit4])
    vals = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    sat_test_fixture.check(4, vals)

def test_add_clause5_1(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    sat_test_fixture.solver.add_clause(lit1, lit2, lit3, lit4, lit5)
    vals = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    sat_test_fixture.check(5, vals)

def test_add_clause5_1l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    sat_test_fixture.solver.add_clause([lit1, lit2, lit3], (lit4, lit5))
    vals = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    sat_test_fixture.check(5, vals)

def test_add_clause_with_cond1_1(sat_test_fixture):
    clit = sat_test_fixture.cond_var_list[0]
    sat_test_fixture.solver.set_conditional_literals(clit)
    lit1 = sat_test_fixture.var_list[0]
    sat_test_fixture.solver.add_clause(lit1)
    vals = [0, 1]
    sat_test_fixture.check_with_cond(1, vals)
    sat_test_fixture.solver.clear_conditional_literals()

def test_add_clause_with_cond1_2(sat_test_fixture):
    clit = sat_test_fixture.cond_var_list[0]
    sat_test_fixture.solver.set_conditional_literals(clit)
    lit1 = sat_test_fixture.var_list[0]
    sat_test_fixture.solver.add_clause(~lit1)
    vals = [1, 0]
    sat_test_fixture.check_with_cond(1, vals)
    sat_test_fixture.solver.clear_conditional_literals()

def test_add_clause_with_cond2_1(sat_test_fixture):
    clit = sat_test_fixture.cond_var_list[0]
    sat_test_fixture.solver.set_conditional_literals(clit)
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_clause(lit1, lit2)
    vals = [0, 1, 1, 1]
    sat_test_fixture.check_with_cond(2, vals)
    sat_test_fixture.solver.clear_conditional_literals()

def test_add_clause_with_cond2_2(sat_test_fixture):
    clit = sat_test_fixture.cond_var_list[0]
    sat_test_fixture.solver.set_conditional_literals(clit)
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_clause(lit1, ~lit2)
    vals = [1, 1, 0, 1]
    sat_test_fixture.check_with_cond(2, vals)
    sat_test_fixture.solver.clear_conditional_literals()
