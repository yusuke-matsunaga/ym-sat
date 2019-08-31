### @file literal.pxi
### @brief literal の python インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017 Yusuke Matsunaga
### All rights reserved.

from libcpp cimport bool
from CXX_SatLiteral cimport SatLiteral


### @brief リテラルを表すクラス
###
### C++ のクラスとは別に Python の拡張型を定義する．
### 相互の変換関数を用意する．
cdef class Literal :

    # 変数番号と極性を収めた実体
    cdef int __body

    ### @brief 初期化
    ### @param[in] varid 変数番号
    def __init__(Literal self, int varid, bool inv = False) :
        self.set(varid, inv)

    ### @brief 内容をセットする．
    ### @param[in] varid 変数番号
    def set(Literal self, int varid, bool inv = False) :
        cdef int vid = varid.val()
        cdef int inv_offset = 0
        if varid < 0 :
            self.__body = -1
        else :
            if inv :
                inv_offset = 1
            self.__body = (vid << 1) | inv_offset

    ### @brief 変数番号を得る．
    def varid(Literal self) :
        return self.__body >> 1

    ### @brief 正極性の時 True を返す．
    def is_positive(Literal self) :
        if (self.__body & 1) == 0 :
            return True
        else :
            return False

    ### @brief 負極性の時 True を返す．
    def is_negative(Literal self) :
        return not self.is_positive()

    ### @brief 変数番号が同じの正極性のリテラルを返す．
    def make_positive(Literal self) :
        return Literal(self.varid(), False)

    ### @brief 変数番号が同じの負極性のリテラルを返す．
    def make_positive(Literal self) :
        return Literal(self.varid(), True)

    ### @brief 否定演算子
    def __invert__(Literal self) :
        return Literal(self.varid(), self.is_positive())

    ### @brief 文字列演算子
    def __repr__(Literal self) :
        inv_str = '' if self.is_positive() else '~'
        return '{}{}'.format(inv_str, self.varid())


### @brief Python の Literal を C++ の SatLiteral に変換する．
### @param[in] literal Python バージョンの Literal
cdef SatLiteral from_literal(literal) :
    cdef int varid = literal.varid()
    return SatLiteral(varid, literal.is_negative())

### @brief C++ の SatLiteral を Python の Literal に変換する．
### @param[in] c_literal C++ バージョンの SatLiteral
cdef to_literal(SatLiteral c_literal) :
    cdef int varid = c_literal.varid()
    return Literal(varid, c_literal.is_negative())
