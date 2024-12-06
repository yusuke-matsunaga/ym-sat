#! /usr/bin/env python3

"""SatSolver のテストプログラム

:file: SatSolver_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2022 Yusuke Matsunaga, All rights reserved.
"""

import pytest
from ymsat import SatSolver, SatBool3


@pytest.fixture
def solver():
    return SatSolver()

def test_add_clause1(solver):
    v1 = solver.new_variable()
    v2 = solver.new_variable()
    v3 = solver.new_variable()

    solver.add_clause(v1, ~v2)
    solver.add_clause(~v1, v3)
    solver.add_clause(~v3)

    r = solver.solve()

    assert r == SatBool3.true

    model = solver.model()
    assert model[v1] == SatBool3.false
    assert model[v2] == SatBool3.false
    assert model[v3] == SatBool3.false

    assert model[v1] == solver.read_model(v1)
    assert model[v2] == solver.read_model(v2)
    assert model[v3] == solver.read_model(v3)
