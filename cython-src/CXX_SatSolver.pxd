# @file CXX_SatSolver.pxd
# @brief SatSolver の cython インターフェイス
# @author Yusuke Matsunaga (松永 裕介)
#
# Copyright (C) 2017, 2019 Yusuke Matsunaga
# All rights reserved.

from libcpp cimport bool
from libcpp.string cimport string
from libcpp.vector cimport vector
from CXX_SatBool3 cimport SatBool3
from CXX_SatLiteral cimport SatLiteral
from CXX_SatSolverType cimport SatSolverType
from CXX_SatStats cimport SatStats
from CXX_SatModel cimport SatModel

ctypedef unsigned int ymuint
ctypedef unsigned long ymuint64


cdef extern from "ym/SatSolver.h" namespace "nsYm" :

    # SatSolver クラスの cython バージョン
    cdef cppclass SatSolver :
        SatSolver(const SatSolverType& solver_type)
        SatLiteral new_variable(bool decision)
        void set_conditional_literals(vector[SatLiteral]& lits)
        void clear_conditional_literals()
        void add_clause(vector[SatLiteral]& lits)
        SatBool3 solve(SatModel& model, int time_limit)
        SatBool3 solve(vector[SatLiteral]& assumptions, SatModel& model, int time_limit)
        void stop()
        void timer_on(bool enable)
        ymuint64 set_max_conflict(ymuint64 val)
        bool sane()
        void get_stats(SatStats& stats)
        ymuint variable_num()
        ymuint clause_num()
        ymuint literal_num()
