# @file CXX_SatStats.pxd
# @brief CXX_SatStats の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


cdef extern from "ym/SatStats.h" namespace "nsYm::SatStats" :

    cdef cppclass SatStats :
        void clear()
