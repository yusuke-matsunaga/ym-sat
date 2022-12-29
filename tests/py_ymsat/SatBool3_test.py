#! /usr/bin/env python3

"""SatBool3 のテストプログラム

:file: SatBool3_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2022 Yusuke Matsunaga, All rights reserved.
"""

import ymsat


def test_true():
    v1 = ymsat.SatBool3('true')
    assert str(v1) == "true"
    assert v1 == ymsat.SatBool3.true
    assert v1 != ymsat.SatBool3.false
    assert v1 != ymsat.SatBool3.x

def test_false():
    v2 = ymsat.SatBool3('False')
    assert str(v2) == "false"
    assert v2 != ymsat.SatBool3.true
    assert v2 == ymsat.SatBool3.false
    assert v2 != ymsat.SatBool3.x

def test_x():
    v3 = ymsat.SatBool3('x')
    assert str(v3) == "x"
    assert v3 != ymsat.SatBool3.true
    assert v3 != ymsat.SatBool3.false
    assert v3 == ymsat.SatBool3.x

def test_invert():
    v1 = ymsat.SatBool3.true
    v2 = ymsat.SatBool3.false
    v3 = ymsat.SatBool3.x
    assert ~v1 == v2
    assert ~v2 == v1
    assert ~v3 == v3

def test_xor():
    v1 = ymsat.SatBool3.true
    v2 = ymsat.SatBool3.false
    v3 = ymsat.SatBool3.x
    assert v1 ^ v1 == ymsat.SatBool3.false
    assert v1 ^ v2 == ymsat.SatBool3.true
    assert v1 ^ v3 == ymsat.SatBool3.x

    assert v2 ^ v1 == ymsat.SatBool3.true
    assert v2 ^ v2 == ymsat.SatBool3.false
    assert v2 ^ v3 == ymsat.SatBool3.x

    assert v3 ^ v1 == ymsat.SatBool3.x
    assert v3 ^ v2 == ymsat.SatBool3.x
    assert v3 ^ v3 == ymsat.SatBool3.x

def test_ixor():
    v = ymsat.SatBool3.true
    v ^= ymsat.SatBool3.true
    v == ymsat.SatBool3.false

    v = ymsat.SatBool3.true
    v ^= ymsat.SatBool3.false
    v == ymsat.SatBool3.true

    v = ymsat.SatBool3.true
    v ^= ymsat.SatBool3.x
    v == ymsat.SatBool3.x

    v = ymsat.SatBool3.false
    v ^= ymsat.SatBool3.true
    v == ymsat.SatBool3.true

    v = ymsat.SatBool3.false
    v ^= ymsat.SatBool3.false
    v == ymsat.SatBool3.false

    v = ymsat.SatBool3.false
    v ^= ymsat.SatBool3.x
    v == ymsat.SatBool3.x

    v = ymsat.SatBool3.x
    v ^= ymsat.SatBool3.true
    v == ymsat.SatBool3.x

    v = ymsat.SatBool3.x
    v ^= ymsat.SatBool3.false
    v == ymsat.SatBool3.x
    
    v = ymsat.SatBool3.x
    v ^= ymsat.SatBool3.x
    v == ymsat.SatBool3.x
    
