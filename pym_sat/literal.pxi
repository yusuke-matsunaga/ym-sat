# @file literal.pxi
# @brief literal の python インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from CXX_SatLiteral cimport SatLiteral
from CXX_SatVarId cimport SatVarId


# @brief リテラルを表すクラス
#
# C++ のクラスとは別に Python の拡張型を定義する．
# 相互の変換関数を用意する．
cdef class Literal :
    # 変数番号と極性を収めた実体
    cdef unsigned int _body

    # @brief 初期化
    def __init__(Literal self, args) :
        if args != None :
            # 未実装
            pass

    # @brief 内容をセットする．
    def set(Literal self, VarId varid, bool inv) :
        cdef int vid = varid.val()
        # 未実装

    # @brief 変数番号を得る．
    def varid(Literal self) :
        return VarId(self._body >> 1)

    # @brief 正極性の時 True を返す．
    def is_positive(Literal self) :
        if (self._body & 1) == 1 :
            return True
        else :
            return False

    # @brief 負極性の時 True を返す．
    def is_negative(Literal self) :
        return not self.is_positive()

    # @brief 変数番号が同じの正極性のリテラルを返す．
    def make_positive(Literal self) :
        return Literal(self.varid(), False)

    # @brief 変数番号が同じの負極性のリテラルを返す．
    def make_positive(Literal self) :
        return Literal(self.varid(), True)
