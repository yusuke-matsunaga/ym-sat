#! /usr/bin/env python3

"""SatSolver.add_counter() に関するテスト

:file: counter_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
from ymsat import SatBool3, SatSolver
from solver_test import SolverTest


class CounterTest(SolverTest):

    def __init__(self, solver_type=None):
        super().__init__(solver_type)

    def check_counter(self, ni, no):
        no_exp = 1 << no

        assert ni < no_exp
        assert ni + no < self.var_num

        ilits = [ self.var_list[i] for i in range(ni) ]

        tmp_olits = self.solver.add_counter(ilits)

        for i in range(no):
            olit = self.var_list[i + ni]
            self.solver.add_buffgate(tmp_olits[i], olit)

        nexp = 1 << (ni + no)
        vals = [ None for _ in range(nexp) ]
        for p in range(nexp):
            c_exp = 0
            for i in range(ni):
                if p & (1 << i):
                    c_exp += 1
            c = 0
            for i in range(no):
                if p & (1 << (i + ni)):
                    c |= (1 << i)
            if c == c_exp:
                vals[p] = 1
            else:
                vals[p] = 0
        self.check(ni + no, vals)

    def check_counter2(self, ni, no):
        no_exp = 1 << no

        assert ni < no_exp
        assert ni + no < self.var_num

        ilits = [ self.var_list[i] for i in range(ni) ]

        tmp_olits = self.solver.add_counter(ilits)

        for i in range(no):
            olit = self.var_list[i + ni]
            self.solver.add_buffgate(tmp_olits[i], olit)

        num = 20000
        do_random = True
        if num > (1 << ni):
            num = (1 << ni)
            do_random = False

        for j in range(num):
            c_exp = 0
            assumptions = []
            ivals = [ None for _ in range(ni) ]
            for i in range(ni):
                lit = self.var_list[i]
                if do_random:
                    ivals[i] = random.randint(0, 1)
                else:
                    ivals[i] = (j >> i) & 1
                if ivals[i]:
                    c_exp += 1
                else:
                    lit *= True
                assumptions.append(lit)
            for i in range(no):
                lit = self.var_list[i + ni]
                if (c_exp & (1 << i)) == 0:
                    lit *= True
                assumptions.append(lit)
            ans = self.solver.solve(assumptions)
            assert ans == SatBool3.true
            ng = False
            for i in range(no):
                olit = self.var_list[i + ni]
                v = self.solver.read_model(olit)
                exp_v = SatBool3.true if c_exp & (1 << i) else SatBool3.false
                assert v == exp_v
                  
@pytest.fixture
def sat_test_fixture():
    return CounterTest()

def test_counter_2_2(sat_test_fixture):
    sat_test_fixture.check_counter(2, 2)

def test_counter_3_2(sat_test_fixture):
    sat_test_fixture.check_counter(3, 2)

def test_counter_4_3(sat_test_fixture):
    sat_test_fixture.check_counter(4, 3)

def test_counter_5_3(sat_test_fixture):
    sat_test_fixture.check_counter(5, 3)

def test_counter_6_3(sat_test_fixture):
    sat_test_fixture.check_counter(6, 3)

def test_counter_7_3(sat_test_fixture):
    sat_test_fixture.check_counter(7, 3)

def test_counter_8_4(sat_test_fixture):
    sat_test_fixture.check_counter(8, 4)

def test_counter_9_4(sat_test_fixture):
    sat_test_fixture.check_counter(9, 4)

def test_counter_10_4(sat_test_fixture):
    sat_test_fixture.check_counter(10, 4)

def test_counter_11_4(sat_test_fixture):
    sat_test_fixture.check_counter(11, 4)

def test_counter_12_4(sat_test_fixture):
    sat_test_fixture.check_counter(12, 4)

def test_counter_13_4(sat_test_fixture):
    sat_test_fixture.check_counter(13, 4)

def test_counter_14_4(sat_test_fixture):
    sat_test_fixture.check_counter(14, 4)

def test_counter_15_4(sat_test_fixture):
    sat_test_fixture.check_counter(15, 4)

def test_counter2_2_2(sat_test_fixture):
    sat_test_fixture.check_counter2(2, 2)
