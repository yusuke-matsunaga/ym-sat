#! /usr/bin/env python3

""" ymsat モジュールを生成するスクリプト

:file: ymsat_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import ModuleGen


class YmsatGen(ModuleGen):

    def __init__(self):
        super().__init__(modulename='ymsat',
                         pyclass_list=['PyCnfSize',
                                       'PySatBool3',
                                       'PySatLiteral',
                                       'PySatModel',
                                       'PySatSolver',
                                      ],
                         include_files=['pym/PyCnfSize.h',
                                        'pym/PySatBool3.h',
                                        'pym/PySatLiteral.h',
                                        'pym/PySatModel.h',
                                        'pym/PySatSolver.h',
                                       ],
                         namespace='YM')


if __name__ == '__main__':

    gen = YmlogicGen()
    gen.make_header()
    gen.make_source()
