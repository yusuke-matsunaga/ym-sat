
### @file solvertype.pxi
### @brief solvertype の cython インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2018, 2019 Yusuke Matsunaga
### All rights reserved.

from libcpp.string cimport string
from CXX_iostream cimport ostream
from CXX_SatSolverType cimport SatSolverType as CXX_SatSolverType


### @brief SatSolverType クラスの Python バージョン
cdef class SolverType :

    # C++ レベルのオブジェクト本体
    cdef CXX_SatSolverType _this

    ### @brief 初期化
    def __init__(SolverType self, sat_type = None, sat_option = None) :
        cdef string c_sat_type = sat_type.encode('UTF-8') if sat_type != None else string()
        cdef string c_sat_option = sat_option.encode('UTF-8') if sat_option != None else string()
        self._this.set(c_sat_type, c_sat_option, NULL)
