# @file satsolver.pxi
# @brief satsolver の python インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017 Yusuke Matsunaga
# All rights reserved.

cimport CXX_SatSolver
from CXX_vector cimport vector
from CXX_SatBool3 cimport SatBool3, kB3X, kB3True, kB3False
from CXX_SatVarId cimport SatVarId
from CXX_SatLiteral cimport SatLiteral
cimport CXX_SatStats


## @brief SatSolver クラスの Python バージョン
cdef class SatSolver :

    ## C++ レベルのオブジェクト本体
    cdef CXX_SatSolver.SatSolver* _this_ptr

    ## @brief 初期化
    def __init__(SatSolver self, args) :
        pass

    ## @brief 新しい変数を確保する．
    def new_var(SatSolver self, bool decision = True) :
        cdef SatVarId c_varid = self._this_ptr.new_var(decision)
        return VarId(c_varid.val())

    ## @brief 節を追加する．
    def add_clause(SatSolver self, *args) :
        pass

    ## @brief 問題を解く．
    def solve(SatSolver self) :
        cdef vector[SatBool3] c_model
        cdef SatBool3 c_stat = self._this_ptr.solve(c_model)
        cdef SatBool3 c_val
        stat = to_bool3(c_stat)
        model = []
        if stat == Bool3.TRUE :
            for i in range(c_model.size()) :
                model.append(to_bool3(c_model[i]))
        return stat, model

    ## @brief assumption 付きの問題を解く．
    def solve_with_assumption(SatSolver self, assumptions) :
        cdef vector[SatLiteral] c_assumptions
        cdef vector[SatBool3] c_model
        cdef SatBool3 c_stat
        cdef SatBool3 c_val
        c_assumptions.reserve(len(assumptions))
        for lit in assumptions :
            c_assumptions.push_back(from_literal(lit))
        c_stat = self._this_ptr.solve(c_assumptions, c_model)
        stat = to_bool3(c_stat)
        model = []
        if stat == Bool3.TRUE :
            for i in range(c_model.size()) :
                model.append(to_bool3(c_model[i]))
        return stat, model

    ## @brief solve() を中止する．
    def stop(SatSolver self) :
        self._this_ptr.stop()

    ## @brief 時間計測を制御する．
    def timer_on(SatSolver self, bool enable) :
        self._this_ptr.timer_on(enable)

    ## @brief 矛盾回数の制限値を設定する．
    def set_max_conflict(SatSolver self, int limit) :
        return self._this_ptr.set_max_conflict(limit)

    ## @brief 正常な状態なら True を返す．
    def sane(SatSolver self) :
        return self._this_ptr.sane()

    ## @brief 統計情報を得る．
    def get_stats(SatSolver self) :
        cdef SatStats c_stats
        self._this_ptr.get_stats(c_stats)
        return Stats(c_stats)

    ## @brief 変数の数を返す．
    def variable_num(SatSolver self) :
        return self._this_ptr.variable_num()

    ## @brief 節の数を返す．
    def clause_num(SatSolver self) :
        return self._this_ptr.clause_num()

    ## @brief リテラルの数を返す．
    def literal_num(SatSolver self) :
        return self._this_ptr.literal_num()
