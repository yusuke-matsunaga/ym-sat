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
assert v1 != ymsat.SatBool3.false
assert v1 != ymsat.SatBool3.x

v2 = ymsat.SatBool3('False')
assert str(v2) == "false"
assert v2 != ymsat.SatBool3.true
assert v2 == ymsat.SatBool3.false
assert v2 != ymsat.SatBool3.x

v3 = ymsat.SatBool3('x')
assert str(v3) == "x"
assert v3 != ymsat.SatBool3.true
assert v3 != ymsat.SatBool3.false
assert v3 == ymsat.SatBool3.x

assert ~v1 == v2
assert ~v2 == v1
assert ~v3 == v3

assert v1 ^ v1 == ymsat.SatBool3.false
assert v1 ^ v2 == ymsat.SatBool3.true
assert v1 ^ v3 == ymsat.SatBool3.x

assert v2 ^ v1 == ymsat.SatBool3.true
assert v2 ^ v2 == ymsat.SatBool3.false
assert v2 ^ v3 == ymsat.SatBool3.x

assert v3 ^ v1 == ymsat.SatBool3.x
assert v3 ^ v2 == ymsat.SatBool3.x
assert v3 ^ v3 == ymsat.SatBool3.x

v = v1
v ^= v1
v == ymsat.SatBool3.false

v = v1
v ^= v2
v == ymsat.SatBool3.true

v = v1
v ^= v3
v == ymsat.SatBool3.x

v = v2
v ^= v1
v == ymsat.SatBool3.true

v = v2
v ^= v2
v == ymsat.SatBool3.false

v = v2
v ^= v3
v == ymsat.SatBool3.x

v = v3
v ^= v1
v == ymsat.SatBool3.x

v = v3
v ^= v2
v == ymsat.SatBool3.x

v = v3
v ^= v3
v == ymsat.SatBool3.x
