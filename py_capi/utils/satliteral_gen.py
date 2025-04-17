#! /usr/bin/env python3

""" PySatLiteral を生成するスクリプト

:file: satliteral_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import MulOp, Iop


class SatLiteralGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='SatLiteral',
                         pyname='SatLiteral',
                         namespace='YM',
                         header_include_files=['ym/SatLiteral.h'],
                         source_include_files=['pym/PySatLiteral.h',
                                               'pym/PyLong.h',
                                               'pym/PyBool.h',
                                               ])

        self.add_new('disabled')
        
        self.add_dealloc(None)

        self.add_richcompare('cmp_default')
        
        def meth_is_valid(writer):
            writer.gen_return_py_bool('val.is_valid()')
        self.add_method('is_valid',
                        func_body=meth_is_valid,
                        doc_str='True if valid')
        
        def meth_is_positive(writer):
            writer.gen_return_py_bool('val.is_positive()')
        self.add_method('is_positive',
                        func_body=meth_is_positive,
                        doc_str='True if positive literal')
        
        def meth_is_negative(writer):
            writer.gen_return_py_bool('val.is_negative()')
        self.add_method('is_negative',
                        func_body=meth_is_negative,
                        doc_str='True if negative literal')

        def meth_make_positive(writer):
            writer.gen_return_pyobject('PySatLiteral', 'val.make_positive()')
        self.add_method('make_positive',
                        func_body=meth_make_positive,
                        doc_str='return the positive literal of the same variable')

        def meth_make_negative(writer):
            writer.gen_return_pyobject('PySatLiteral', 'val.make_negative()')
        self.add_method('make_negative',
                        func_body=meth_make_negative,
                        doc_str='return the negative literal of the same variable')

        self.add_nb_invert()
        self.add_nb_multiply(expr=None,
                             op_list1=[MulOp('PyBool', useref=False)])
        self.add_nb_inplace_multiply(stmt=None,
                                     op_list1=[Iop('PyBool',
                                                   'val1 = val1 * val2',
                                                   useref=False)])

        def hash_body(writer):
            writer.gen_return('val.hash()')
        self.add_hash(hash_body)

        def get_varid(writer):
            writer.gen_return_py_long('val.varid()')
        self.add_getter('get_varid',
                        func_body=get_varid)
        self.add_attr('varid',
                      getter_name='get_varid',
                      doc_str='variable ID')

        def get_index(writer):
            writer.gen_return_py_long('val.index()')
        self.add_getter('get_index',
                        func_body=get_index)
        self.add_attr('index',
                      getter_name='get_index',
                      doc_str='unique index')
        
        self.add_conv('default')

        self.add_deconv('default')


if __name__ == '__main__':

    gen = SatLiteralGen()

    gen.make_header()

    gen.make_source()
    
    
