#! /usr/bin/env python3

"""SatBool3 のテストプログラム

:file: SatBool3_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2022 Yusuke Matsunaga, All rights reserved.
"""

import pytest
import itertools
from ymsat import SatBool3


def test_true():
    v1 = SatBool3('true')
    assert str(v1) == "true"
    assert v1 == SatBool3._True
    assert v1 != SatBool3._False
    assert v1 != SatBool3._X

def test_false():
    v2 = SatBool3('False')
    assert str(v2) == "false"
    assert v2 != SatBool3._True
    assert v2 == SatBool3._False
    assert v2 != SatBool3._X

def test_x():
    v3 = SatBool3('x')
    assert str(v3) == "x"
    assert v3 != SatBool3._True
    assert v3 != SatBool3._False
    assert v3 == SatBool3._X

def test_bool():
    v1 = SatBool3._True
    v2 = SatBool3._False
    v3 = SatBool3._X
    assert bool(v1)
    assert not bool(v2)
    assert not bool(v3)
    
def test_invert():
    v1 = SatBool3._True
    v2 = SatBool3._False
    v3 = SatBool3._X
    assert ~v1 == v2
    assert ~v2 == v1
    assert ~v3 == v3

all_val3_list = (SatBool3._True, SatBool3._False, SatBool3._X)

def all_val3_pair():
    return itertools.product(all_val3_list, all_val3_list)

def all_val3_pair_name():
    for v1, v2 in all_val3_pair():
        yield f'{v1}-{v2}'
        
@pytest.fixture(params=all_val3_pair(), ids=all_val3_pair_name())
def val3_pair(request):
    return request.param

def test_xor(val3_pair):
    v1, v2 = val3_pair
    v = v1 ^ v2
    if v1 == SatBool3._X or v2 == SatBool3._X:
        assert v == SatBool3._X
    elif v1 == v2:
        assert v == SatBool3._False
    else:
        assert v == SatBool3._True

def test_ixor(val3_pair):
    v1, v2 = val3_pair
    v1_old = v1
    v1 ^= v2
    if v1_old == SatBool3._X or v2 == SatBool3._X:
        assert v1 == SatBool3._X
    elif v1_old == v2:
        assert v1 == SatBool3._False
    else:
        assert v1 == SatBool3._True
    
