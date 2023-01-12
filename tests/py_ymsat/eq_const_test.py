#! /usr/bin/env python3

"""SatSolver.add_eq()に関するエンコーディングのテスト

:file: eq_const_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import random
from ymsat import SatBool3, SatSolver
from solver_test import SolverTest


class CompConstTest(SolverTest):

    def __init__(self, solver_type=None):
        super().__init__(solver_type)

    def check_eq(self, na, b):
        lit_a = [ self.var_list[i] for i in range(na) ]
        self.solver.add_eq(lit_a, b)
        self.check_common(na, lambda a: a == b)

    def check_ne(self, na, b):
        lit_a = [ self.var_list[i] for i in range(na) ]
        self.solver.add_ne(lit_a, b)
        self.check_common(na, lambda a: a != b)

    def check_lt(self, na, b):
        lit_a = [ self.var_list[i] for i in range(na) ]
        self.solver.add_lt(lit_a, b)
        self.check_common(na, lambda a: a < b)

    def check_le(self, na, b):
        lit_a = [ self.var_list[i] for i in range(na) ]
        self.solver.add_le(lit_a, b)
        self.check_common(na, lambda a: a <= b)

    def check_gt(self, na, b):
        lit_a = [ self.var_list[i] for i in range(na) ]
        self.solver.add_gt(lit_a, b)
        self.check_common(na, lambda a: a > b)

    def check_ge(self, na, b):
        lit_a = [ self.var_list[i] for i in range(na) ]
        self.solver.add_ge(lit_a, b)
        self.check_common(na, lambda a: a >= b)
        
    def check_common(self, na, comp):
        assert na < self.var_num
        amask = (1 << na) - 1
        if na <= 15:
            np = 1 << na
            ashift = 0
            vals = [ None for _ in range(np) ]
            for p in range(np):
                if comp(p):
                    vals[p] = 1
                else:
                    vals[p] = 0
            self.check(na, vals)
        else:
            num = 20000
            for _ in range(num):
                a = random.randint(0, amask)
                assumptions = []
                for i in range(na):
                    lit = self.var_list[i]
                    if (a & (1 << i)) == 0:
                        lit = ~lit
                    assumptions.append(lit)
                ans = self.solver.solve(assumptions)
                exp_ans = SatBool3._True if comp(a) else SatBool3._False
                assert ans == exp_ans

                
@pytest.fixture
def sat_test_fixture():
    return CompConstTest()

@pytest.fixture(params=[(1, 1), (2, 3), (5, 6), (7, 10), (7, 13), (10, 496), (15, 0), (15, 10)])
def na_b(request):
    return request.param

def test_eq(sat_test_fixture, na_b):
    na, b = na_b
    sat_test_fixture.check_eq(na, b)

def test_ne(sat_test_fixture, na_b):
    na, b = na_b
    sat_test_fixture.check_ne(na, b)

def test_lt(sat_test_fixture, na_b):
    na, b = na_b
    sat_test_fixture.check_lt(na, b)

def test_le(sat_test_fixture, na_b):
    na, b = na_b
    sat_test_fixture.check_le(na, b)

def test_gt(sat_test_fixture, na_b):
    na, b = na_b
    sat_test_fixture.check_gt(na, b)

def test_ge(sat_test_fixture, na_b):
    na, b = na_b
    sat_test_fixture.check_ge(na, b)
