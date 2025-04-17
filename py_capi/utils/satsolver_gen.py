#! /usr/bin/env python3

""" PySatSolver を生成するスクリプト

:file: satsolver_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import BoolArg, ObjConvArgBase, ObjConvArg, TypedObjConvArg, OptArg


class LitsArg(ObjConvArgBase):

    def __init__(self, *,
                 name=None,
                 cvarname):
        super().__init__(name=name,
                         cvartype='std::vector<SatLiteral>',
                         cvarname=cvarname,
                         cvardefault=None)
        
    def conv_body(self, writer):
        with writer.gen_if_block(f'PySatLiteral::Check({self.tmpname})'):
            writer.gen_auto_assign('lit',
                                   f'PySatLiteral::_get_ref({self.tmpname})')
            writer.gen_stmt(f'{self.cvarname}.push_back(lit)')
        with writer.gen_elseif_block(f'!PyList<SatLiteral, PySatLiteral>::FromPyObject({self.tmpname}, {self.cvarname})'):
            writer.gen_type_error('"\'a SatLiteral\' or a sequence of \'SatLiteral\'s is expected"')
                 

class SatSolverGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='SatSolver',
                         pyname='SatSolver',
                         namespace='YM',
                         header_include_files=['ym/SatSolver.h'],
                         source_include_files=['pym/PySatSolver.h',
                                               'pym/PySatBool3.h',
                                               'pym/PySatLiteral.h',
                                               'pym/PyList.h',
                                               ])

        self.add_new('disabled')
        
        self.add_dealloc('default')

        def meth_new_variable(writer):
            writer.gen_return_pyobject('PySatLiteral',
                                       'val.new_variable(decision)')
        self.add_method('new_variable',
                        func_body=meth_new_variable,
                        arg_list=[OptArg(),
                                  BoolArg(name='decision',
                                          cvarname='decision',
                                          cvardefault='true')],
                        doc_str='make new variable')

        def meth_set_conditional_literals(writer):
            writer.gen_stmt('val.set_conditional_literals(lit_list)')
            writer.gen_return_py_none()
        self.add_method('set_conditional_literals',
                        func_body=meth_set_conditional_literals,
                        arg_list=[ObjConvArg(name='lit_list',
                                             cvartype='std::vector<SatLiteral>',
                                             cvarname='lit_list',
                                             cvardefault=None,
                                             pyclassname='PyList<SatLiteral, PySatLiteral>')],
                        doc_str="set 'conditional literals'")
        
        def meth_clear_conditional_literals(writer):
            writer.gen_stmt('val.clear_conditional_literals()')
            writer.gen_return_py_none()
        self.add_method('clear_conditional_literals',
                        func_body=meth_clear_conditional_literals,
                        doc_str="clear 'conditional literals'")

        def meth_add_clause(writer):
            writer.gen_stmt('val.add_clause(lit_list)')
            writer.gen_return_py_none()
        self.add_method('add_clause',
                        func_body=meth_add_clause,
                        arg_list=[LitsArg(name='lit_list',
                                          cvarname='lit_list')],
                        doc_str='add clause')

        def meth_add_bufgate(writer):
            writer.gen_stmt('val.add_buffgate(lit1, lit2)')
            writer.gen_return_py_none()
        self.add_method('add_bufgate',
                        func_body=meth_add_bufgate,
                        arg_list=[TypedObjConvArg(name='lit1',
                                                  cvarname='lit1',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  TypedObjConvArg(name='lit2',
                                                  cvarname='lit2',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral')],
                        doc_str="add clauses representing 'BUF' gate")
                        
                        
        

if __name__ == '__main__':

    gen = SatSolverGen()

    gen.make_header()

    gen.make_source()
    
    
