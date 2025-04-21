#! /usr/bin/env python3

"""SatSolver.add_adder() に関するテスト

:file: adder_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
from ymsat import SatBool3, SatSolver
from solver_test import SolverTest


class AdderTest(SolverTest):

    def __init__(self, solver_type=None):
        super().__init__(solver_type)

    def check_adder(self, na, nb, ns):
        assert na <= ns
        assert nb <= ns
        nall = na + nb + ns + 2
        assert nall <= self.var_num

        alits = [ self.var_list[i] for i in range(na) ]
        blits = [ self.var_list[i + na] for i in range(nb) ]
        ilit = self.var_list[na + nb]
        slits = [ self.var_list[i + na + nb + 1] for i in range(ns) ]
        olit = self.var_list[na + nb + ns + 1]

        self.solver.add_adder(alits, blits, ilit, slits, olit)

        nexp = 1 << nall
        vals = [ None for _ in range(nexp) ]
        amask = (1 << na) - 1
        ashift = 0
        bmask = (1 << nb) - 1
        bshift = na
        imask = 1
        ishift = na + nb
        smask = (1 << ns) - 1
        sshift = na + nb + 1
        omask = 1
        oshift = na + nb + 1 + ns
        for p in range(nexp):
            a = (p >> ashift) & amask
            b = (p >> bshift) & bmask
            i = (p >> ishift) & imask
            s = (p >> sshift) & smask
            o = (p >> oshift) & omask
            s_exp = a + b + i
            o_exp = 0
            if s_exp >= (1 << ns):
                o_exp = 1
                s_exp -= (1 << ns)
            if s == s_exp and o == o_exp:
                vals[p] = 1
            else:
                vals[p] = 0
        self.check(nall, vals)

@pytest.fixture
def sat_test_fixture():
    return AdderTest()

def test_add_4_4_4(sat_test_fixture):
    sat_test_fixture.check_adder(4, 4, 4)

def test_add_4_3_4(sat_test_fixture):
    sat_test_fixture.check_adder(4, 3, 4)

def test_add_4_2_4(sat_test_fixture):
    sat_test_fixture.check_adder(4, 2, 4)

def test_add_3_4_4(sat_test_fixture):
    sat_test_fixture.check_adder(3, 4, 4)

def test_add_2_4_4(sat_test_fixture):
    sat_test_fixture.check_adder(2, 4, 4)

def test_add_3_3_4(sat_test_fixture):
    sat_test_fixture.check_adder(3, 3, 4)

def test_add_3_2_4(sat_test_fixture):
    sat_test_fixture.check_adder(3, 2, 4)

def test_add_3_1_4(sat_test_fixture):
    sat_test_fixture.check_adder(3, 1, 4)

def test_add_3_1_5(sat_test_fixture):
    sat_test_fixture.check_adder(3, 1, 5)
