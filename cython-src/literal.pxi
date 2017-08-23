# @file literal.pxi
# @brief literal の python インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from CXX_SatLiteral cimport SatLiteral
from CXX_SatVarId cimport SatVarId


## @brief リテラルを表すクラス
#
# C++ のクラスとは別に Python の拡張型を定義する．
# 相互の変換関数を用意する．
cdef class Literal :

    # 変数番号と極性を収めた実体
    cdef unsigned int _body

    ## @brief 初期化
    def __init__(Literal self, VarId varid, bool inv = False) :
        self.set(varid, inv)

    ## @brief 内容をセットする．
    def set(Literal self, VarId varid, bool inv) :
        cdef int vid = varid.val()
        if vid == 0 :
            inv = False
        self._body = (vid << 1) | int(inv)

    ## @brief 変数番号を得る．
    def varid(Literal self) :
        return VarId(self._body >> 1)

    ## @brief 正極性の時 True を返す．
    def is_positive(Literal self) :
        if (self._body & 1) == 0 :
            return True
        else :
            return False

    ## @brief 負極性の時 True を返す．
    def is_negative(Literal self) :
        return not self.is_positive()

    ## @brief 変数番号が同じの正極性のリテラルを返す．
    def make_positive(Literal self) :
        return Literal(self.varid(), False)

    ## @brief 変数番号が同じの負極性のリテラルを返す．
    def make_positive(Literal self) :
        return Literal(self.varid(), True)


## @brief Python の Literal を C++ の SatLiteral に変換する．
cdef SatLiteral from_literal(literal) :
    cdef SatVarId c_varid = SatVarId(literal.varid().val())
    return SatLiteral(c_varid, literal.is_negative())

## @brief C++ の SatLiteral を Python の Literal に変換する．
cdef to_literal(SatLiteral c_literal) :
    varid = to_varid(c_literal.varid())
    return Literal(varid, c_literal.is_negative())
