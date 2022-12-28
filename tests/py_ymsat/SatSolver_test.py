#! /usr/bin/env python3

"""SatSolver のテストプログラム

:file: SatSolver_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2022 Yusuke Matsunaga, All rights reserved.
"""

import ymsat

solver = ymsat.SatSolver()

v1 = solver.new_variable()
v2 = solver.new_variable()
v3 = solver.new_variable()

solver.add_clause(v1, ~v2)
solver.add_clause(~v1, v3)
solver.add_clause(~v3)

r = solver.solve()

assert r == ymsat.SatBool3.true

assert solver.read_model(v1) == ymsat.SatBool3.false
assert solver.read_model(v2) == ymsat.SatBool3.false
assert solver.read_model(v3) == ymsat.SatBool3.false
