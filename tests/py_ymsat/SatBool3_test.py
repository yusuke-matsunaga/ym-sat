#! /usr/bin/env python3

"""SatBool3 のテストプログラム

:file: SatBool3_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2022 Yusuke Matsunaga, All rights reserved.
"""

import ymsat

v1 = ymsat.SatBool3('true')
assert str(v1) == "true"
assert v1 == ymsat.SatBool3.true

v2 = ymsat.SatBool3('False')
assert str(v2) == "false"
assert v2 == ymsat.SatBool3.false

v3 = ymsat.SatBool3('x')
assert str(v3) == "x"
assert v3 == ymsat.SatBool3.x

assert ~v1 == v2
assert ~v2 == v1
assert ~v3 == v3

