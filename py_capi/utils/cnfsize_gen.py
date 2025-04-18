#! /usr/bin/env python3

""" PyCnfSize を生成するスクリプト

:file: cnfsize_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen, OptArg, KwdArg, IntArg, BoolArg


class CnfSizeGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='CnfSize',
                         pyname='CnfSize',
                         namespace='YM',
                         header_include_files=['ym/CnfSize.h'],
                         source_include_files=['pym/PyCnfSize.h',
                                               'pym/PyLong.h',
                                               'pym/PyString.h'])

        self.add_dealloc(None)

        self.add_new('default')

        def get_clause_num(writer):
            writer.gen_return_py_long('val.clause_num')
        self.add_getter('get_clause_num',
                        func_body=get_clause_num)
        self.add_attr('clause_num',
                      getter_name='get_clause_num',
                      doc_str='number of clauses')

        def get_literal_num(writer):
            writer.gen_return_py_long('val.literal_num')
        self.add_getter('get_literal_num',
                        func_body=get_literal_num)
        self.add_attr('literal_num',
                      getter_name='get_literal_num',
                      doc_str='number of literals')

        self.add_richcompare('eq_default')

        self.add_nb_add()
        self.add_nb_subtract()
        self.add_nb_inplace_add()
        self.add_nb_inplace_subtract()

        self.add_conv('default')

        self.add_deconv('default')


if __name__ == '__main__':

    gen = CnfSizeGen()

    gen.make_header()

    gen.make_source()
