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
    assert v1 == SatBool3.true
    assert v1 != SatBool3.false
    assert v1 != SatBool3.x

def test_false():
    v2 = SatBool3('False')
    assert str(v2) == "false"
    assert v2 != SatBool3.true
    assert v2 == SatBool3.false
    assert v2 != SatBool3.x

def test_x():
    v3 = SatBool3('x')
    assert str(v3) == "x"
    assert v3 != SatBool3.true
    assert v3 != SatBool3.false
    assert v3 == SatBool3.x

def test_bool():
    v1 = SatBool3.true
    v2 = SatBool3.false
    v3 = SatBool3.x
    assert bool(v1)
    assert not bool(v2)
    assert not bool(v3)
    
def test_invert():
    v1 = SatBool3.true
    v2 = SatBool3.false
    v3 = SatBool3.x
    assert ~v1 == v2
    assert ~v2 == v1
    assert ~v3 == v3

all_val3_list = (SatBool3.true, SatBool3.false, SatBool3.x)

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
    if v1 == SatBool3.x or v2 == SatBool3.x:
        assert v == SatBool3.x
    elif v1 == v2:
        assert v == SatBool3.false
    else:
        assert v == SatBool3.true

def test_ixor(val3_pair):
    v1, v2 = val3_pair
    v1_old = v1
    v1 ^= v2
    if v1_old == SatBool3.x or v2 == SatBool3.x:
        assert v1 == SatBool3.x
    elif v1_old == v2:
        assert v1 == SatBool3.false
    else:
        assert v1 == SatBool3.true
    
