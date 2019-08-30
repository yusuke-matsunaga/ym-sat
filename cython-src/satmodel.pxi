
### @file satmodel.pxi
### @brief satmodel の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2019 Yusuke Matsunaga
### All rights reserved.

from CXX_SatModel cimport SatModel as CXX_SatModel
from CXX_SatBool3 cimport SatBool3 as CXX_SatBool3


### @brief SatModel クラスの Python バージョン
cdef class SatModel :

    # C++ レベルのオブジェクト本体
    cdef CXX_SatModel _this

    ### @brief 初期化
    def __init__(self) :
        pass

    ### @brief 値を取り出す．
    def get(self, lit) :
        cdef CXX_SatLiteral c_lit = from_literal(lit)
        cdef CXX_SatBool3 c_val = self._this.get(c_lit)
        return to_SatBool3(c_val)
