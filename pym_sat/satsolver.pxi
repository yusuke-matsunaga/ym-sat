# @file satsolver.pxi
# @brief satsolver の python インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

cimport CXX_SatSolver
cimport CXX_SatBool3
cimport CXX_SatVarId
cimport CXX_SatLiteral


# @brief SatSolver クラスの Python バージョン
cdef class SatSolver :
    # C++ レベルのオブジェクト本体
    cdef CXX_SatSolver.SatSolver* _this_ptr

    # @brief 初期化
    def __init__(SatSolver self, args) :
        pass
