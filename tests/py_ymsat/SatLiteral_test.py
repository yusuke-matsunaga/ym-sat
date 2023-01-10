#! /usr/bin/env python3

"""

:file: SatLiteral_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

from ymsat import SatSolver, SatLiteral
import pytest


def test_bad_constructor():
    with pytest.raises(Exception) as e:
        lit = SatLiteral()
    assert e.type == TypeError
    assert str(e.value) == "Instantiation of 'SatLiteral' is disabled"

def test_is_valid():
    solver = SatSolver()
    lit = solver.new_variable()

    assert lit.is_valid()
    assert lit.is_positive()
    assert not lit.is_negative()

def test_make_negative():
    solver = SatSolver()
    lit = solver.new_variable()
    nlit = lit.make_negative()

    assert not nlit.is_positive()
    assert nlit.is_negative()
    assert lit.varid == nlit.varid

def test_make_positive():
    solver = SatSolver()
    lit = solver.new_variable()
    nlit = lit.make_negative()
    plit = lit.make_positive()

    assert plit == lit
    
def test_compare():
    solver = SatSolver()
    lit1 = solver.new_variable()
    lit2 = solver.new_variable()

    assert not lit1 == lit2
    assert lit1 != lit2

def test_invert():
    solver = SatSolver()
    lit = solver.new_variable()

    ilit = ~lit
    nlit = ilit.make_positive()
    iilit = ~ilit
    
    assert ilit.is_negative()
    assert nlit == lit
    assert iilit == lit

def test_mul_True():
    solver = SatSolver()
    lit = solver.new_variable()

    ilit = lit * True
    nlit = ilit.make_positive()
    iilit = ~ilit
    
    assert ilit.is_negative()
    assert nlit == lit
    assert iilit == lit

def test_imul_True():
    solver = SatSolver()
    lit = solver.new_variable()

    ilit = lit
    ilit *= True
    nlit = ilit.make_positive()
    iilit = ~ilit
    
    assert ilit.is_negative()
    assert nlit == lit
    assert iilit == lit

def test_mul_False():
    solver = SatSolver()
    lit = solver.new_variable()

    ilit = lit * False
    assert ilit == lit

def test_imul_False():
    solver = SatSolver()
    lit = solver.new_variable()

    ilit = lit
    ilit *= False
    assert ilit == lit
