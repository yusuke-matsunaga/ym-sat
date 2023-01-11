#! /usr/bin/env python3

"""SatSolver のテスト用の基底クラス

:file: solver_test.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2023 Yusuke Matsunaga, All rights reserved.
"""

from ymsat import SatBool3, SatSolver


class SolverTest:

    def __init__(self, solver_type=None):
        if solver_type is None:
            self.solver = SatSolver()
        else:
            self.solver = SatSolver(solver_type)
        self.var_num = 100
        self.var_list = [ self.solver.new_variable() for _ in range(self.var_num) ]
        self.cond_var_list = [ self.solver.new_variable() for _ in range(2) ]

    def check(self, ni, vals):
        """現在設定されているCNF式が表す論理関数が vals に等しいか調べる"""
        np = 1 << ni
        for p in range(np):
            assumptions = []
            for i in range(ni):
                lit = self.var_list[i]
                if (p & (1 << i)) == 0:
                    lit *= True
                assumptions.append(lit)
            ans = self.solver.solve(assumptions)
            exp_ans = SatBool3._True if vals[p] else SatBool3._False
            assert ans == exp_ans

    def check_with_cond(self, ni, vals):
        """条件変数を伴う check() 関数"""
        np = 1 << ni
        for p in range(np):
            assumptions = []
            assumptions.append(~self.cond_var_list[0])
            for i in range(ni):
                lit = self.var_list[i]
                if (p & (1 << i)) == 0:
                    lit *= True
                assumptions.append(lit)
            ans = self.solver.solve(assumptions)
            exp_ans = SatBool3._True
            assert ans == exp_ans
        for p in range(np):
            assumptions = []
            assumptions.append(self.cond_var_list[0])
            for i in range(ni):
                lit = self.var_list[i]
                if (p & (1 << i)) == 0:
                    lit *= True
                assumptions.append(lit)
            ans = self.solver.solve(assumptions)
            exp_ans = SatBool3._True if vals[p] else SatBool3._False
            assert ans == exp_ans

    staticmethod
    def make_vals(ni, tv, inv):
        np = 1 << ni
        ni1 = ni + 1
        np1 = 1 << ni1
        vals = [ None for _ in range(np1) ]
        v0 = 1 if inv else 0
        v1 = 0 if inv else 1
        for p in range(np):
            vals[p +  0] = (tv[p] == v0)
            vals[p + np] = (tv[p] == v1)
        return vals
