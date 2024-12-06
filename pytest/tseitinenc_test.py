#! /usr/bin/env python3

"""SatSolver の Tseitin encoding に関するテスト

:file: tseitinenc_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

import pytest
from ymsat import SatBool3, SatSolver
from solver_test import SolverTest


class TseitinEncTest(SolverTest):

    def __init__(self, solver_type=None):
        super().__init__(solver_type)

    def check_and(self, ni):
        np = 1 << ni
        tv = [ None for _ in range(np) ]
        for p in range(np):
            val = 1
            for i in range(ni):
                if (p & (1 << i)) == 0:
                    val = 0
                    break
            tv[p] = val
        vals = SolverTest.make_vals(ni, tv, False)
        self.check(ni + 1, vals)

    def check_nand(self, ni):
        np = 1 << ni
        tv = [ None for _ in range(np) ]
        for p in range(np):
            val = 1
            for i in range(ni):
                if (p & (1 << i)) == 0:
                    val = 0
                    break
            tv[p] = val
        vals = SolverTest.make_vals(ni, tv, True)
        self.check(ni + 1, vals)

    def check_or(self, ni):
        np = 1 << ni
        tv = [ None for _ in range(np) ]
        for p in range(np):
            val = 0
            for i in range(ni):
                if (p & (1 << i)) != 0:
                    val = 1
                    break
            tv[p] = val
        vals = SolverTest.make_vals(ni, tv, False)
        self.check(ni + 1, vals)

    def check_nor(self, ni):
        np = 1 << ni
        tv = [ None for _ in range(np) ]
        for p in range(np):
            val = 0
            for i in range(ni):
                if (p & (1 << i)) != 0:
                    val = 1
                    break
            tv[p] = val
        vals = SolverTest.make_vals(ni, tv, True)
        self.check(ni + 1, vals)

    def check_xor(self, ni):
        np = 1 << ni
        tv = [ None for _ in range(np) ]
        for p in range(np):
            val = 0
            for i in range(ni):
                if (p & (1 << i)) != 0:
                    val ^= 1
            tv[p] = val
        vals = SolverTest.make_vals(ni, tv, False)
        self.check(ni + 1, vals)

    def check_xnor(self, ni):
        np = 1 << ni
        tv = [ None for _ in range(np) ]
        for p in range(np):
            val = 0
            for i in range(ni):
                if (p & (1 << i)) != 0:
                    val ^= 1
            tv[p] = val
        vals = SolverTest.make_vals(ni, tv, True)
        self.check(ni + 1, vals)

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
    return TseitinEncTest()

def test_buffgate(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_buffgate(lit1, lit2)
    vals = [1, 0, 0, 1]
    sat_test_fixture.check(2, vals)

def test_notgate(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    sat_test_fixture.solver.add_notgate(lit1, lit2)
    vals = [0, 1, 1, 0]
    sat_test_fixture.check(2, vals)
    
def test_andgate2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_andgate(lit3, lit1, lit2)
    sat_test_fixture.check_and(2)
    
def test_andgate3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    sat_test_fixture.solver.add_andgate(lit4, lit3, lit1, lit2)
    sat_test_fixture.check_and(3)
    
def test_andgate4(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    sat_test_fixture.solver.add_andgate(lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_and(4)
    
def test_andgate5(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    sat_test_fixture.solver.add_andgate(lit6, lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_and(5)
    
def test_andgate5l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    ilits = (lit1, lit2, lit3, lit4, lit5)
    sat_test_fixture.solver.add_andgate(lit6, ilits)
    sat_test_fixture.check_and(5)
    
def test_nandgate2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_nandgate(lit3, lit1, lit2)
    sat_test_fixture.check_nand(2)
    
def test_nandgate3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    sat_test_fixture.solver.add_nandgate(lit4, lit3, lit1, lit2)
    sat_test_fixture.check_nand(3)
    
def test_nandgate4(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    sat_test_fixture.solver.add_nandgate(lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_nand(4)
    
def test_nandgate5(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    sat_test_fixture.solver.add_nandgate(lit6, lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_nand(5)
    
def test_nandgate5l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    ilits = (lit1, lit2, lit3, lit4, lit5)
    sat_test_fixture.solver.add_nandgate(lit6, ilits)
    sat_test_fixture.check_nand(5)
    
def test_orgate2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_orgate(lit3, lit1, lit2)
    sat_test_fixture.check_or(2)
    
def test_orgate3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    sat_test_fixture.solver.add_orgate(lit4, lit3, lit1, lit2)
    sat_test_fixture.check_or(3)
    
def test_orgate4(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    sat_test_fixture.solver.add_orgate(lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_or(4)
    
def test_orgate5(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    sat_test_fixture.solver.add_orgate(lit6, lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_or(5)
    
def test_orgate5l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    ilits = (lit1, lit2, lit3, lit4, lit5)
    sat_test_fixture.solver.add_orgate(lit6, ilits)
    sat_test_fixture.check_or(5)
    
def test_norgate2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_norgate(lit3, lit1, lit2)
    sat_test_fixture.check_nor(2)
    
def test_norgate3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    sat_test_fixture.solver.add_norgate(lit4, lit3, lit1, lit2)
    sat_test_fixture.check_nor(3)
    
def test_norgate4(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    sat_test_fixture.solver.add_norgate(lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_nor(4)
    
def test_norgate5(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    sat_test_fixture.solver.add_norgate(lit6, lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_nor(5)
    
def test_norgate5l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    ilits = (lit1, lit2, lit3, lit4, lit5)
    sat_test_fixture.solver.add_norgate(lit6, ilits)
    sat_test_fixture.check_nor(5)
    
def test_xorgate2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_xorgate(lit3, lit1, lit2)
    sat_test_fixture.check_xor(2)
    
def test_xorgate3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    sat_test_fixture.solver.add_xorgate(lit4, lit3, lit1, lit2)
    sat_test_fixture.check_xor(3)
    
def test_xorgate4(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    sat_test_fixture.solver.add_xorgate(lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_xor(4)
    
def test_xorgate5(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    sat_test_fixture.solver.add_xorgate(lit6, lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_xor(5)
    
def test_xorgate5l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    ilits = (lit1, lit2, lit3, lit4, lit5)
    sat_test_fixture.solver.add_xorgate(lit6, ilits)
    sat_test_fixture.check_xor(5)
    
def test_xnorgate2(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    sat_test_fixture.solver.add_xnorgate(lit3, lit1, lit2)
    sat_test_fixture.check_xnor(2)
    
def test_xnorgate3(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    sat_test_fixture.solver.add_xnorgate(lit4, lit3, lit1, lit2)
    sat_test_fixture.check_xnor(3)
    
def test_xnorgate4(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    sat_test_fixture.solver.add_xnorgate(lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_xnor(4)
    
def test_xnorgate5(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    sat_test_fixture.solver.add_xnorgate(lit6, lit5, lit4, lit3, lit1, lit2)
    sat_test_fixture.check_xnor(5)
    
def test_xnorgate5l(sat_test_fixture):
    lit1 = sat_test_fixture.var_list[0]
    lit2 = sat_test_fixture.var_list[1]
    lit3 = sat_test_fixture.var_list[2]
    lit4 = sat_test_fixture.var_list[3]
    lit5 = sat_test_fixture.var_list[4]
    lit6 = sat_test_fixture.var_list[5]
    ilits = (lit1, lit2, lit3, lit4, lit5)
    sat_test_fixture.solver.add_xnorgate(lit6, ilits)
    sat_test_fixture.check_xnor(5)

def test_half_adder(sat_test_fixture):
    alit = sat_test_fixture.var_list[0]
    blit = sat_test_fixture.var_list[1]
    slit = sat_test_fixture.var_list[2]
    olit = sat_test_fixture.var_list[3]

    sat_test_fixture.solver.add_half_adder(alit, blit, slit, olit)

    vals = [ None for _ in range(16) ]
    for p in range(16):
        a = (p >> 0) & 1
        b = (p >> 1) & 1
        s = (p >> 2) & 1
        o = (p >> 3) & 1
        exp_s = (a + b) % 2
        exp_o = (a + b) // 2
        if s == exp_s and o == exp_o:
            vals[p] = 1
        else:
            vals[p] = 0
    sat_test_fixture.check(4, vals)

def test_full_adder(sat_test_fixture):
    alit = sat_test_fixture.var_list[0]
    blit = sat_test_fixture.var_list[1]
    clit = sat_test_fixture.var_list[2]
    slit = sat_test_fixture.var_list[3]
    olit = sat_test_fixture.var_list[4]

    sat_test_fixture.solver.add_full_adder(alit, blit, clit, slit, olit)

    vals = [ None for _ in range(32) ]
    for p in range(32):
        a = (p >> 0) & 1
        b = (p >> 1) & 1
        c = (p >> 2) & 1
        s = (p >> 3) & 1
        o = (p >> 4) & 1
        exp_s = (a + b + c) % 2
        exp_o = (a + b + c) // 2
        if s == exp_s and o == exp_o:
            vals[p] = 1
        else:
            vals[p] = 0
    sat_test_fixture.check(5, vals)

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
