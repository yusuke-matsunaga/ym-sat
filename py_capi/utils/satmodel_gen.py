#! /usr/bin/env python3

""" PySatModel を生成するスクリプト

:file: satmodel_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen


class SatModelGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='SatModel',
                         pyname='SatModel',
                         namespace='YM',
                         header_include_files=['ym/SatModel.h'],
                         source_include_files=['pym/PySatModel.h',
                                               'pym/PySatBool3.h',
                                               'pym/PySatLiteral.h',
                                               ])

        self.add_new('disabled')

        self.add_dealloc('default')

        def mp_length(writer):
            writer.gen_auto_assign('len_val', 'val.size()')

        def mp_subscript(writer):
            writer.gen_vardecl(typename='SatLiteral',
                               varname='lit')
            with writer.gen_if_block('!PySatLiteral::FromPyObject(key, lit)'):
                writer.gen_type_error('"argument 1 should be a \'SatLiteral\'"')
            writer.gen_return_pyobject('PySatBool3', 'val.get(lit)')

        self.add_mapping(mp_length=mp_length,
                         mp_subscript=mp_subscript)

        self.add_conv('default')

        self.add_deconv('default')


if __name__ == '__main__':

    gen = SatModelGen()

    gen.make_header()

    gen.make_source()
