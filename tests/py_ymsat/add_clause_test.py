#! /usr/bin/env python3

"""

:file: add_clause_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
from ymsat import SatBool3, SatSolver


class AddClauseTest:

    def __init__(self):
        self.solver = SatSolver()
        self.var_num = 100
        self.var_list = [ self.solver.new_variable() for _ in range(self.var_num) ]
        self.cond_var_list = [ self.solver.new_variable() for _ in range(2) ]

    def check(self, ni, vals):
        np = 1 << ni
        for p in range(np):
            assumptions = []
            for i in range(ni):
                lit = self.var_list[i]
                if (p & (1 << i)) == 0:
                    lit *= True
                assumptions.append(lit)
            ans = self.solver.solve(assumptions)
            exp_ans = SatBool3._True if vals[p] else SatBool3._False
            assert ans == exp_ans

    def check_with_cond(self, ni, vals):
        np = 1 << ni
        for p in range(np):
            assumptions = []
            assumptions.append(~self.cond_var_list[0])
            for i in range(ni):
                lit = self.var_list[i]
                if (p & (1 << i)) == 0:
                    lit *= True
                assumptions.append(lit)
            ans = self.solver.solve(assumptions)
            exp_ans = SatBool3._True
            assert ans == exp_ans
        for p in range(np):
            assumptions = []
            assumptions.append(self.cond_var_list[0])
            for i in range(ni):
                lit = self.var_list[i]
                if (p & (1 << i)) == 0:
                    lit *= True
                assumptions.append(lit)
            ans = self.solver.solve(assumptions)
            exp_ans = SatBool3._True if vals[p] else SatBool3._False
            assert ans == exp_ans

    def make_vals(self, ni, tv, inv, vals):
        np = 1 << ni
        ni1 = ni + 1
        np1 = 1 << ni1
        vals = [ None for _ in range(np1) ]
        v0 = 1 if inv else 0
        v1 = 0 if inv else 1
        for p in range(np):
            vals[p +  0] = (tv[p] == v0)
            vals[p + np] = (tv[p] == v1)

@pytest.fixture
def add_clause_test():
    return AddClauseTest()

def test_add_clause0(add_clause_test):
    vals = [1, 1]
    add_clause_test.check(1, vals)

def test_add_clause1_1(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    add_clause_test.solver.add_clause(lit1)
    vals = [0, 1]
    add_clause_test.check(1, vals)

def test_add_clause1_2(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    add_clause_test.solver.add_clause(~lit1)
    vals = [1, 0]
    add_clause_test.check(1, vals)

def test_add_clause2_1(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    add_clause_test.solver.add_clause(lit1, lit2)
    vals = [0, 1, 1, 1]
    add_clause_test.check(2, vals)

def test_add_clause2_1l(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    add_clause_test.solver.add_clause([lit1, lit2])
    vals = [0, 1, 1, 1]
    add_clause_test.check(2, vals)

def test_add_clause2_2(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    add_clause_test.solver.add_clause(~lit1, lit2)
    vals = [1, 0, 1, 1]
    add_clause_test.check(2, vals)

def test_add_clause3_1(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    lit3 = add_clause_test.var_list[2]
    add_clause_test.solver.add_clause(lit1, lit2, lit3)
    vals = [0, 1, 1, 1, 1, 1, 1, 1]
    add_clause_test.check(3, vals)

def test_add_clause3_1l(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    lit3 = add_clause_test.var_list[2]
    add_clause_test.solver.add_clause([lit1, lit2, lit3])
    vals = [0, 1, 1, 1, 1, 1, 1, 1]
    add_clause_test.check(3, vals)

def test_add_clause3_2(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    lit3 = add_clause_test.var_list[2]
    add_clause_test.solver.add_clause(lit1, lit2, lit3)
    vals = [0, 1, 1, 1, 1, 1, 1, 1]
    add_clause_test.check(3, vals)

def test_add_clause3_2(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    lit3 = add_clause_test.var_list[2]
    add_clause_test.solver.add_clause(~lit1, lit2, ~lit3)
    vals = [1, 1, 1, 1, 1, 0, 1, 1]
    add_clause_test.check(3, vals)

def test_add_clause4_1(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    lit3 = add_clause_test.var_list[2]
    lit4 = add_clause_test.var_list[3]
    add_clause_test.solver.add_clause(lit1, lit2, lit3, lit4)
    vals = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    add_clause_test.check(4, vals)

def test_add_clause4_1l(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    lit3 = add_clause_test.var_list[2]
    lit4 = add_clause_test.var_list[3]
    add_clause_test.solver.add_clause([lit1, lit2, lit3, lit4])
    vals = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    add_clause_test.check(4, vals)

def test_add_clause5_1(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    lit3 = add_clause_test.var_list[2]
    lit4 = add_clause_test.var_list[3]
    lit5 = add_clause_test.var_list[4]
    add_clause_test.solver.add_clause(lit1, lit2, lit3, lit4, lit5)
    vals = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    add_clause_test.check(5, vals)

def test_add_clause5_1l(add_clause_test):
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    lit3 = add_clause_test.var_list[2]
    lit4 = add_clause_test.var_list[3]
    lit5 = add_clause_test.var_list[4]
    add_clause_test.solver.add_clause([lit1, lit2, lit3], (lit4, lit5))
    vals = [0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1]
    add_clause_test.check(5, vals)

def test_add_clause_with_cond1_1(add_clause_test):
    clit = add_clause_test.cond_var_list[0]
    add_clause_test.solver.set_conditional_literals(clit)
    lit1 = add_clause_test.var_list[0]
    add_clause_test.solver.add_clause(lit1)
    vals = [0, 1]
    add_clause_test.check_with_cond(1, vals)
    add_clause_test.solver.clear_conditional_literals()

def test_add_clause_with_cond1_2(add_clause_test):
    clit = add_clause_test.cond_var_list[0]
    add_clause_test.solver.set_conditional_literals(clit)
    lit1 = add_clause_test.var_list[0]
    add_clause_test.solver.add_clause(~lit1)
    vals = [1, 0]
    add_clause_test.check_with_cond(1, vals)
    add_clause_test.solver.clear_conditional_literals()

def test_add_clause_with_cond2_1(add_clause_test):
    clit = add_clause_test.cond_var_list[0]
    add_clause_test.solver.set_conditional_literals(clit)
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    add_clause_test.solver.add_clause(lit1, lit2)
    vals = [0, 1, 1, 1]
    add_clause_test.check_with_cond(2, vals)
    add_clause_test.solver.clear_conditional_literals()

def test_add_clause_with_cond2_2(add_clause_test):
    clit = add_clause_test.cond_var_list[0]
    add_clause_test.solver.set_conditional_literals(clit)
    lit1 = add_clause_test.var_list[0]
    lit2 = add_clause_test.var_list[1]
    add_clause_test.solver.add_clause(lit1, ~lit2)
    vals = [1, 1, 0, 1]
    add_clause_test.check_with_cond(2, vals)
    add_clause_test.solver.clear_conditional_literals()
