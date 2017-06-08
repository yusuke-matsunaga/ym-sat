# @file varid.pxi
# @brief VarId の定義ファイル
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.


from CXX_SatVarId cimport SatVarId, kSatVarIdIllegal


# @brief 変数番号を表すクラス
#
# C++ の SatVarId とは別の拡張型として定義して
# 相互の変換関数を用意する．
cdef class VarId :
    # 変数番号の実体
    cdef int _val

    # @brief 初期化
    def __init__(VarId self, val = None) :
        if val != None :
            self._val = int(val)
        else :
            self._val = -1

    # @brief 値を返す．
    def val(VarId self) :
        return self._val


# @brief Python の VarId を C++ の SatVarId に変換する．
cdef SatVarId from_varid(varid) :
    if varid._val == -1 :
        return SatVarId
    else :
        return SatVarId(varid._val)


# @brief C++ の SatVarId を Python の VarId に変換する．
def to_varid(SatVarId c_varid) :
    if c_varid == kSatVarIdIllegal :
        return VarId()
    else :
        return VarId(c_varid.val())
