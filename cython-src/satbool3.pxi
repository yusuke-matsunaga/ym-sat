### @file satbool3.pxi
### @brief SatBool3 の python インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

from enum import Enum
from CXX_SatBool3 cimport SatBool3 as CXX_SatBool3, __int_to_SatBool3, __SatBool3_to_int

### @brief SatBool3 の Python バージョン
###
### もとの C++ の列挙型とは別に Python の拡張型として
### 定義する．
### 相互の変換関数を用意することでシームレスに使えるようにする．
class SatBool3(Enum) :
    _X     = 0
    _True  = 1
    _False = 2

    ### @brief 否定する．
    def negate(self) :
        if self == SatBool3._True :
            return SatBool3._False
        elif self == SatBool3._False :
            return SatBool3._True
        elif self == SatBool3._X :
            return SatBool3._X
        else :
            assert False

    ### @brief 否定演算子(~)
    def __invert__(self) :
        return self.negate()

    ### @brief 文字列表現を返す．
    def __repr__(self) :
        if self == SatBool3._X :
            return 'X'
        elif self == SatBool3._True :
            return 'True'
        elif self == SatBool3._False :
            return 'False'
        else :
            assert False


### @brief Python の Bool3 を C++ の SatBool3 に変換する．
### @param[in] val Python バージョンの Bool3
cdef CXX_SatBool3 from_SatBool3(val) :
    cdef int c_val
    if val == SatBool3._X :
        c_val = 0
    elif val == SatBool3._True :
        c_val = 1
    elif val == SatBool3._False :
        c_val = -1
    else :
        assert False
    return __int_to_SatBool3(c_val)

### @brief C++ の SatBool3 を Python の Bool3 に変換する．
### @param[in] c_val C++ バージョンの SatBool3
cdef to_SatBool3(CXX_SatBool3 c_satbool3) :
    cdef int c_val = __SatBool3_to_int(c_satbool3)
    if c_val == 0 :
        return SatBool3._X
    elif c_val == 1 :
        return SatBool3._True
    elif c_val == -1 :
        return SatBool3._False
    else :
        assert False