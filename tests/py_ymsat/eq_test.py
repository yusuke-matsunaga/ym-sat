#! /usr/bin/env python3

"""SatSolver.add_eq()に関するエンコーディングのテスト

:file: eq_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import random
from ymsat import SatBool3, SatSolver
from solver_test import SolverTest


class CompTest(SolverTest):

    def __init__(self, solver_type=None):
        super().__init__(solver_type)

    def check_eq(self, na, nb):
        lit_a = [ self.var_list[i] for i in range(na) ]
        lit_b = [ self.var_list[i + na] for i in range(nb) ]
        self.solver.add_eq(lit_a, lit_b)
        self.check_common(na, nb, lambda a, b: a == b)

    def check_ne(self, na, nb):
        lit_a = [ self.var_list[i] for i in range(na) ]
        lit_b = [ self.var_list[i + na] for i in range(nb) ]
        self.solver.add_ne(lit_a, lit_b)
        self.check_common(na, nb, lambda a, b: a != b)

    def check_lt(self, na, nb):
        lit_a = [ self.var_list[i] for i in range(na) ]
        lit_b = [ self.var_list[i + na] for i in range(nb) ]
        self.solver.add_lt(lit_a, lit_b)
        self.check_common(na, nb, lambda a, b: a < b)

    def check_le(self, na, nb):
        lit_a = [ self.var_list[i] for i in range(na) ]
        lit_b = [ self.var_list[i + na] for i in range(nb) ]
        self.solver.add_le(lit_a, lit_b)
        self.check_common(na, nb, lambda a, b: a <= b)

    def check_gt(self, na, nb):
        lit_a = [ self.var_list[i] for i in range(na) ]
        lit_b = [ self.var_list[i + na] for i in range(nb) ]
        self.solver.add_gt(lit_a, lit_b)
        self.check_common(na, nb, lambda a, b: a > b)

    def check_ge(self, na, nb):
        lit_a = [ self.var_list[i] for i in range(na) ]
        lit_b = [ self.var_list[i + na] for i in range(nb) ]
        self.solver.add_ge(lit_a, lit_b)
        self.check_common(na, nb, lambda a, b: a >= b)
        
    def check_common(self, na, nb, comp):
        nall = na + nb
        assert nall < self.var_num
        amask = (1 << na) - 1
        bmask = (1 << nb) - 1
        if nall <= 15:
            np = 1 << nall
            ashift = 0
            bshift = na
            vals = [ None for _ in range(np) ]
            for p in range(np):
                a = (p >> ashift) & amask
                b = (p >> bshift) & bmask
                if comp(a, b):
                    vals[p] = 1
                else:
                    vals[p] = 0
            self.check(nall, vals)
        else:
            num = 20000
            for _ in range(num):
                a = random.randint(0, amask)
                b = random.randint(0, bmask)
                assumptions = []
                for i in range(na):
                    lit = self.var_list[i]
                    if (a & (1 << i)) == 0:
                        lit = ~lit
                    assumptions.append(lit)
                for i in range(nb):
                    lit = self.var_list[i + na]
                    if (b & (1 << i)) == 0:
                        lit = ~lit
                    assumptions.append(lit)
                ans = self.solver.solve(assumptions)
                exp_ans = SatBool3._True if comp(a, b) else SatBool3._False
                if ans != exp_ans:
                    print(f'a = {a}, b = {b}')
                assert ans == exp_ans

                
@pytest.fixture
def sat_test_fixture():
    return CompTest()

@pytest.fixture(params=[(1, 1), (2, 2), (5, 5), (7, 7), (7, 4), (4, 7), (10, 10), (10, 5), (5, 10)])
def na_nb(request):
    return request.param

def test_eq(sat_test_fixture, na_nb):
    na, nb = na_nb
    sat_test_fixture.check_eq(na, nb)

def test_ne(sat_test_fixture, na_nb):
    na, nb = na_nb
    sat_test_fixture.check_ne(na, nb)

def test_lt(sat_test_fixture, na_nb):
    na, nb = na_nb
    sat_test_fixture.check_lt(na, nb)

def test_le(sat_test_fixture, na_nb):
    na, nb = na_nb
    sat_test_fixture.check_le(na, nb)

def test_gt(sat_test_fixture, na_nb):
    na, nb = na_nb
    sat_test_fixture.check_gt(na, nb)

def test_ge(sat_test_fixture, na_nb):
    na, nb = na_nb
    sat_test_fixture.check_ge(na, nb)
