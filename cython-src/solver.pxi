### @file solver.pxi
### @brief SatSolver の python インターフェイス
### @author Yusuke Matsunaga (松永 裕介)
###
### Copyright (C) 2017, 2019 Yusuke Matsunaga
### All rights reserved.

from libcpp.string cimport string
from libcpp.vector cimport vector
from CXX_SatSolver cimport SatSolver as CXX_SatSolver
from CXX_SatBool3 cimport SatBool3 as CXX_SatBool3
from CXX_SatLiteral cimport SatLiteral as CXX_SatLiteral
from CXX_SatSolverType cimport SatSolverType as CXX_SatSolverType
from CXX_SatModel cimport SatModel as CXX_SatModel
from CXX_SatStats cimport SatStats as CXX_SatStats


### @brief SatSolver クラスの Python バージョン
cdef class Solver :

    # C++ レベルのオブジェクト本体
    cdef CXX_SatSolver* _this_ptr

    ### @brief 初期化
    ### @param[in] sat_type SATソルバの種類を表す文字列
    ### @param[in] sat_option SATソルバに対するオプションを表す文字列
    def __cinit__(Solver self, SolverType solver_type = SolverType()) :
        self._this_ptr = new CXX_SatSolver(solver_type._this)

    ### @brief 終了処理
    def __dealloc__(Solver self) :
        if self._this_ptr != NULL :
            del self._this_ptr

    ### @brief 新しい変数を確保する．
    ### @param[in] decision 決定変数の時に True にする．
    ### @return その変数に対応するリテラル(Literal)を返す．
    ###
    ### * 変数そのもの(VarId)ではないことに注意
    def new_variable(Solver self, bool decision = True) :
        cdef CXX_SatLiteral c_lit = self._this_ptr.new_variable(decision)
        return to_literal(c_lit)

    ### @brief 条件リテラルを設定する．
    ### @param[in] lit_lits リテラルのリスト
    def set_conditional_literals(Solver self, lit_list) :
        cdef vector[CXX_SatLiteral] c_lits
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.set_conditional_literals(c_lits)

    ### @brief 条件リテラルの設定を解除する．
    def clear_conditional_literals(Solver self) :
        self._this_ptr.clear_conditional_literals()

    ### @brief 節を追加する．
    ### @param[in] lit_lits リテラルのリスト
    def add_clause(Solver self, lit_list) :
        cdef vector[CXX_SatLiteral] c_lits
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_clause(c_lits)

    ### @brief 2つのリテラルが等しいという制約節を追加する．
    ### @param[in] lit1, lit2 比較対象のリテラル
    def add_eq_rel(Solver self, lit1, lit2, cvar_list = None) :
        cdef CXX_SatLiteral c_lit1 = from_literal(lit1)
        cdef CXX_SatLiteral c_lit2 = from_literal(lit2)
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        self._this_ptr.add_eq_rel(c_lit1, c_lit2)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 2つのリテラルが等しくないという制約節を追加する．
    ### @param[in] lit1, lit2 比較対象のリテラル
    def add_neq_rel(Solver self, lit1, lit2, cvar_list = None) :
        cdef CXX_SatLiteral c_lit1 = from_literal(lit1)
        cdef CXX_SatLiteral c_lit2 = from_literal(lit2)
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        self._this_ptr.add_neq_rel(c_lit1, c_lit2)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief ANDゲートの入出力の関係を表す制約節を追加する．
    ### @param[in] olit 出力のリテラル
    ### @param[in] ilit_list 入力のリテラルのリスト
    def add_andgate_rel(Solver self, olit, ilit_list) :
        cdef CXX_SatLiteral c_olit = from_literal(olit)
        cdef vector[CXX_SatLiteral] c_ilits
        for lit in ilit_list :
            c_ilits.push_back(from_literal(lit))
        self._this_ptr.add_andgate_rel(c_olit, c_ilits)

    ### @brief NANDゲートの入出力の関係を表す制約節を追加する．
    ### @param[in] olit 出力のリテラル
    ### @param[in] ilit_list 入力のリテラルのリスト
    def add_nandgate_rel(Solver self, olit, ilit_list) :
        cdef CXX_SatLiteral c_olit = from_literal(olit)
        cdef vector[CXX_SatLiteral] c_ilits
        for lit in ilit_list :
            c_ilits.push_back(from_literal(lit))
        self._this_ptr.add_nandgate_rel(c_olit, c_ilits)

    ### @brief ORゲートの入出力の関係を表す制約節を追加する．
    ### @param[in] olit 出力のリテラル
    ### @param[in] ilit_list 入力のリテラルのリスト
    def add_orgate_rel(Solver self, olit, ilit_list) :
        cdef CXX_SatLiteral c_olit = from_literal(olit)
        cdef vector[CXX_SatLiteral] c_ilits
        for lit in ilit_list :
            c_ilits.push_back(from_literal(lit))
        self._this_ptr.add_orgate_rel(c_olit, c_ilits)

    ### @brief NORゲートの入出力の関係を表す制約節を追加する．
    ### @param[in] olit 出力のリテラル
    ### @param[in] ilit_list 入力のリテラルのリスト
    def add_norgate_rel(Solver self, olit, ilit_list) :
        cdef CXX_SatLiteral c_olit = from_literal(olit)
        cdef vector[CXX_SatLiteral] c_ilits
        for lit in ilit_list :
            c_ilits.push_back(from_literal(lit))
        self._this_ptr.add_norgate_rel(c_olit, c_ilits)

    ### @brief XORゲートの入出力の関係を表す制約節を追加する．
    ### @param[in] olit 出力のリテラル
    ### @param[in] ilit_list 入力のリテラルのリスト
    def add_xorgate_rel(Solver self, olit, ilit_list) :
        cdef CXX_SatLiteral c_olit = from_literal(olit)
        cdef vector[CXX_SatLiteral] c_ilits
        for lit in ilit_list :
            c_ilits.push_back(from_literal(lit))
        self._this_ptr.add_xorgate_rel(c_olit, c_ilits)

    ### @brief XNORゲートの入出力の関係を表す制約節を追加する．
    ### @param[in] olit 出力のリテラル
    ### @param[in] ilit_list 入力のリテラルのリスト
    def add_xnorgate_rel(Solver self, olit, ilit_list) :
        cdef CXX_SatLiteral c_olit = from_literal(olit)
        cdef vector[CXX_SatLiteral] c_ilits
        for lit in ilit_list :
            c_ilits.push_back(from_literal(lit))
        self._this_ptr.add_xnorgate_rel(c_olit, c_ilits)

    ### @brief 高々1つのリテラルしか真にならないという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_at_most_one(Solver self, lit_list, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_at_most_one(c_lits)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 高々2つのリテラルしか真にならないという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_at_most_two(Solver self, lit_list, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_at_most_two(c_lits)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 高々K個のリテラルしか真にならないという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_at_most_k(Solver self, lit_list, unsigned int k, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_at_most_k(c_lits, k)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 1つ以上のリテラルが真になるという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_at_least_one(Solver self, lit_list, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_at_least_one(c_lits)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 2つ以上のリテラルが真になるという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_at_least_two(Solver self, lit_list, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_at_least_two(c_lits)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief K個以上のリテラルが真になるという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_at_least_k(Solver self, lit_list, unsigned int k, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_at_least_k(c_lits, k)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 高々1つのリテラルしか真にならないという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_exact_one(Solver self, lit_list, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_exact_one(c_lits)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 高々2つのリテラルしか真にならないという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_exact_two(Solver self, lit_list, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_exact_two(c_lits)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 高々K個のリテラルしか真にならないという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_exact_k(Solver self, lit_list, unsigned int k, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX?SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_exact_k(c_lits, k)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 真になっているリテラルの数が1でないという制約節を追加する．
    ### @param[in] lit_list リテラルのリスト
    def add_not_one(Solver self, lit_list, cvar_list = None) :
        cdef vector[CXX_SatLiteral] c_lits
        cdef vector[CXX_SatLiteral] c_cvar_list
        if cvar_list :
            for cvar in cvar_list :
                c_cvar_list.push_back(from_literal(cvar))
            self._this_ptr.set_conditional_literals(c_cvar_list)
        for lit in lit_list :
            c_lits.push_back(from_literal(lit))
        self._this_ptr.add_not_one(c_lits)
        if cvar_list :
            self._this_ptr.clear_conditional_literals()

    ### @brief 問題を解く．
    ### @param[in] assumptions 仮定(Literal)のリスト
    ### @param[in] time_limit 時間制約(秒) 0 で制約なし
    ### @return stat, model を返す．
    ###
    ### - stat は結果を表す3値(SatBool3)
    ### - mode はVarIdをキーにしてその変数の値(SatBool3)を保持する辞書
    ###
    ### 変数の値の型は3値だが常に真(Bool3.TRUE)か偽(Bool3.FALSE)となる．
    def solve(Solver self, *, assumptions = None, time_limit = 0) :
        cdef vector[CXX_SatLiteral] c_assumptions
        cdef CXX_SatModel c_model
        cdef CXX_SatBool3 c_stat
        cdef CXX_SatBool3 c_val
        cdef int i
        if assumptions :
            c_assumptions.reserve(len(assumptions))
            for lit in assumptions :
                c_assumptions.push_back(from_literal(lit))
            c_stat = self._this_ptr.solve(c_assumptions, c_model, time_limit)
        else :
            c_stat = self._this_ptr.solve(c_model, time_limit)
        stat = to_Bool3(c_stat)
        model = None
        if stat == Bool3._True :
            model = {}
            for i in range(c_model.size()) :
                c_val = c_model.get(i)
                val = to_Bool3(c_val)
                lit = Literal(i, False)
                model[lit] = val
                model[~lit] ~ ~val
        return stat, model

    ### @brief solve() を中止する．
    def stop(Solver self) :
        self._this_ptr.stop()

    ### @brief 時間計測を制御する．
    ### @param[in] enable True の時に時間計測を行う．
    def timer_on(Solver self, bool enable) :
        self._this_ptr.timer_on(enable)

    ### @brief 矛盾回数の制限値を設定する．
    ### @param[in] limit 制限値
    ### @return 以前の制限値を返す．
    def set_max_conflict(Solver self, int limit) :
        return self._this_ptr.set_max_conflict(limit)

    ### @brief 正常な状態なら True を返す．
    def sane(Solver self) :
        return self._this_ptr.sane()

    ### @brief 統計情報を得る．
    ### @return Stats を返す．
    def get_stats(Solver self) :
        cdef CXX_SatStats c_stats
        self._this_ptr.get_stats(c_stats)
        return to_stats(c_stats)

    ### @brief 変数の数を返す．
    def variable_num(Solver self) :
        return self._this_ptr.variable_num()

    ### @brief 節の数を返す．
    def clause_num(Solver self) :
        return self._this_ptr.clause_num()

    ### @brief リテラルの数を返す．
    def literal_num(Solver self) :
        return self._this_ptr.literal_num()
