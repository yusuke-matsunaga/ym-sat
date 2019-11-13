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
    ### @param[in] args リテラルのリスト
    def set_conditional_literals(Solver self, *args) :
        cdef vector[CXX_SatLiteral] c_lits
        for arg in args :
            if isinstance(arg, Literal) :
                c_lits.push_back(from_literal(arg))
            else :
                for lit in arg :
                    c_lits.push_back(from_literal(lit))
        self._this_ptr.set_conditional_literals(c_lits)

    ### @brief 条件リテラルの設定を解除する．
    def clear_conditional_literals(Solver self) :
        self._this_ptr.clear_conditional_literals()

    ### @brief 節を追加する．
    ### @param[in] args リテラルのリスト
    def add_clause(Solver self, *args) :
        cdef vector[CXX_SatLiteral] c_lits
        for arg in args :
            if isinstance(arg, Literal) :
                c_lits.push_back(from_literal(arg))
            else :
                for lit in arg :
                    c_lits.push_back(from_literal(lit))
        self._this_ptr.add_clause(c_lits)

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
        cdef vector[CXX_SatLiteral] c_conflicts
        cdef CXX_SatBool3 c_stat
        cdef CXX_SatLiteral c_lit
        cdef CXX_SatBool3 c_val
        cdef int i, n
        if assumptions :
            c_assumptions.reserve(len(assumptions))
            for lit in assumptions :
                c_assumptions.push_back(from_literal(lit))
        c_stat = self._this_ptr.solve(c_assumptions, time_limit)
        stat = to_Bool3(c_stat)
        model = None
        if stat == Bool3._True :
            n = self._this_ptr.model_size()
            model = dict()
            for i in range(n) :
                lit = Literal(i, False)
                c_lit = from_literal(lit)
                c_val = self._this_ptr.read_model(c_lit)
                val = to_Bool3(c_val)
                model[ lit] =  val
                model[~lit] = ~val
        elif stat == Bool3._False :
            conflicts = []
            #for c_lit in self._this_ptr.conflict_literals() :
            #    conflicts.append(to_literal(c_lit))
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
