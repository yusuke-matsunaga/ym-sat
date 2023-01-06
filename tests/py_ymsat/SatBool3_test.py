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
    assert v1 == ymsat.SatBool3._True
    assert v1 != ymsat.SatBool3._False
    assert v1 != ymsat.SatBool3._X

def test_false():
    v2 = ymsat.SatBool3('False')
    assert str(v2) == "false"
    assert v2 != ymsat.SatBool3._True
    assert v2 == ymsat.SatBool3._False
    assert v2 != ymsat.SatBool3._X

def test_x():
    v3 = ymsat.SatBool3('x')
    assert str(v3) == "x"
    assert v3 != ymsat.SatBool3._True
    assert v3 != ymsat.SatBool3._False
    assert v3 == ymsat.SatBool3._X

def test_invert():
    v1 = ymsat.SatBool3._True
    v2 = ymsat.SatBool3._False
    v3 = ymsat.SatBool3._X
    assert ~v1 == v2
    assert ~v2 == v1
    assert ~v3 == v3

def test_xor():
    v1 = ymsat.SatBool3._True
    v2 = ymsat.SatBool3._False
    v3 = ymsat.SatBool3._X
    assert v1 ^ v1 == ymsat.SatBool3._False
    assert v1 ^ v2 == ymsat.SatBool3._True
    assert v1 ^ v3 == ymsat.SatBool3._X

    assert v2 ^ v1 == ymsat.SatBool3._True
    assert v2 ^ v2 == ymsat.SatBool3._False
    assert v2 ^ v3 == ymsat.SatBool3._X

    assert v3 ^ v1 == ymsat.SatBool3._X
    assert v3 ^ v2 == ymsat.SatBool3._X
    assert v3 ^ v3 == ymsat.SatBool3._X

def test_ixor():
    v = ymsat.SatBool3._True
    v ^= ymsat.SatBool3._True
    v == ymsat.SatBool3._False

    v = ymsat.SatBool3._True
    v ^= ymsat.SatBool3._False
    v == ymsat.SatBool3._True

    v = ymsat.SatBool3._True
    v ^= ymsat.SatBool3._X
    v == ymsat.SatBool3._X

    v = ymsat.SatBool3._False
    v ^= ymsat.SatBool3._True
    v == ymsat.SatBool3._True

    v = ymsat.SatBool3._False
    v ^= ymsat.SatBool3._False
    v == ymsat.SatBool3._False

    v = ymsat.SatBool3._False
    v ^= ymsat.SatBool3._X
    v == ymsat.SatBool3._X

    v = ymsat.SatBool3._X
    v ^= ymsat.SatBool3._True
    v == ymsat.SatBool3._X

    v = ymsat.SatBool3._X
    v ^= ymsat.SatBool3._False
    v == ymsat.SatBool3._X
    
    v = ymsat.SatBool3._X
    v ^= ymsat.SatBool3._X
    v == ymsat.SatBool3._X
    
