#! /usr/bin/env python3

"""SatSolver.add_at_most_XXX()関するエンコーディングのテスト

:file: at_most_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
from ymsat import SatBool3, SatSolver
from solver_test import SolverTest


class AtMostTest(SolverTest):

    def __init__(self, solver_type=None):
        super().__init__(solver_type)

    def check_at_most(self, n, k):
        self.check_common(n, lambda c: c <= k)

    def check_at_least(self, n, k):
        self.check_common(n, lambda c: c >= k)

    def check_exact(self, n, k):
        self.check_common(n, lambda c: c == k)

    def check_not_one(self, n):
        self.check_common(n, lambda c: c != 1)
        
    def check_common(self, n, comp):
        np = 1 << n
        for p in range(np):
            assumptions = []
            c = 0
            for i in range(n):
                lit = self.var_list[i]
                if p & (1 << i):
                    c += 1
                else:
                    lit *= True
                assumptions.append(lit)
            exp_ans = SatBool3.true if comp(c) else SatBool3.false
            ans = self.solver.solve(assumptions)
            assert ans == exp_ans

@pytest.fixture
def sat_test_fixture():
    return AtMostTest()

def test_at_most_one_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_at_most_one(lit1, lit2)
    sat_test_fixture.check_at_most(2, 1)

def test_at_most_one_3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_at_most_one(lit1, lit2, lit3)
    sat_test_fixture.check_at_most(3, 1)

def test_at_most_one_5l(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(5) ]
    sat_test_fixture.solver.add_at_most_one(lit_list)
    sat_test_fixture.check_at_most(5, 1)

def test_at_most_two_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_at_most_two(lit1, lit2)
    sat_test_fixture.check_at_most(2, 2)

def test_at_most_two_3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_at_most_two(lit1, lit2, lit3)
    sat_test_fixture.check_at_most(3, 2)

def test_at_most_two_5l(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(5) ]
    sat_test_fixture.solver.add_at_most_two(lit_list)
    sat_test_fixture.check_at_most(5, 2)

def test_at_most_3_10(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(10) ]
    sat_test_fixture.solver.add_at_most_k(3, lit_list)
    sat_test_fixture.check_at_most(10, 3)

def test_at_most_5_10(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(10) ]
    sat_test_fixture.solver.add_at_most_k(5, lit_list)
    sat_test_fixture.check_at_most(10, 5)

def test_at_most_3_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_at_most_k(3, lit_list)
    sat_test_fixture.check_at_most(15, 3)

def test_at_most_5_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_at_most_k(5, lit_list)
    sat_test_fixture.check_at_most(15, 5)

def test_at_most_10_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_at_most_k(10, lit_list)
    sat_test_fixture.check_at_most(15, 10)

def test_at_least_one_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_at_least_one(lit1, lit2)
    sat_test_fixture.check_at_least(2, 1)

def test_at_least_one_3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_at_least_one(lit1, lit2, lit3)
    sat_test_fixture.check_at_least(3, 1)

def test_at_least_one_5l(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(5) ]
    sat_test_fixture.solver.add_at_least_one(lit_list)
    sat_test_fixture.check_at_least(5, 1)

def test_at_least_two_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_at_least_two(lit1, lit2)
    sat_test_fixture.check_at_least(2, 2)

def test_at_least_two_3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_at_least_two(lit1, lit2, lit3)
    sat_test_fixture.check_at_least(3, 2)

def test_at_least_two_5l(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(5) ]
    sat_test_fixture.solver.add_at_least_two(lit_list)
    sat_test_fixture.check_at_least(5, 2)

def test_at_least_3_10(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(10) ]
    sat_test_fixture.solver.add_at_least_k(3, lit_list)
    sat_test_fixture.check_at_least(10, 3)

def test_at_least_5_10(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(10) ]
    sat_test_fixture.solver.add_at_least_k(5, lit_list)
    sat_test_fixture.check_at_least(10, 5)

def test_at_least_3_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_at_least_k(3, lit_list)
    sat_test_fixture.check_at_least(15, 3)

def test_at_least_5_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_at_least_k(5, lit_list)
    sat_test_fixture.check_at_least(15, 5)

def test_at_least_10_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_at_least_k(10, lit_list)
    sat_test_fixture.check_at_least(15, 10)

def test_exact_one_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_exact_one(lit1, lit2)
    sat_test_fixture.check_exact(2, 1)

def test_exact_one_3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_exact_one(lit1, lit2, lit3)
    sat_test_fixture.check_exact(3, 1)

def test_exact_one_5l(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(5) ]
    sat_test_fixture.solver.add_exact_one(lit_list)
    sat_test_fixture.check_exact(5, 1)

def test_exact_two_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_exact_two(lit1, lit2)
    sat_test_fixture.check_exact(2, 2)

def test_exact_two_3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_exact_two(lit1, lit2, lit3)
    sat_test_fixture.check_exact(3, 2)

def test_exact_two_5l(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(5) ]
    sat_test_fixture.solver.add_exact_two(lit_list)
    sat_test_fixture.check_exact(5, 2)

def test_exact_3_10(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(10) ]
    sat_test_fixture.solver.add_exact_k(3, lit_list)
    sat_test_fixture.check_exact(10, 3)

def test_exact_5_10(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(10) ]
    sat_test_fixture.solver.add_exact_k(5, lit_list)
    sat_test_fixture.check_exact(10, 5)

def test_exact_3_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_exact_k(3, lit_list)
    sat_test_fixture.check_exact(15, 3)

def test_exact_5_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_exact_k(5, lit_list)
    sat_test_fixture.check_exact(15, 5)

def test_exact_10_15(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(15) ]
    sat_test_fixture.solver.add_exact_k(10, lit_list)
    sat_test_fixture.check_exact(15, 10)

def test_not_one_2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_not_one(lit1, lit2)
    sat_test_fixture.check_not_one(2)

def test_not_one_3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_not_one(lit1, lit2, lit3)
    sat_test_fixture.check_not_one(3)

def test_not_one_5l(sat_test_fixture):
    lit_list = [ sat_test_fixture.var_list[i] for i in range(5) ]
    sat_test_fixture.solver.add_not_one(lit_list)
    sat_test_fixture.check_not_one(5)
