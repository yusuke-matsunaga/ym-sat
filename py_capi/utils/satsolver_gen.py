#! /usr/bin/env python3

""" PySatSolver を生成するスクリプト

:file: satsolver_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import IntArg, BoolArg
from mk_py_capi import RawObjArg, TypedRawObjArg, TypedObjConvArg
from mk_py_capi import OptArg, KwdArg


class LitsParser:
    """
    １つ以上の SatLiteral のシーケンスを読み込むパーサークラス
    特例として SatLiteral 単体のオブジェクトも受け付ける．
    """

    def __init__(self, *,
                 cvarname):
        self.__cvarname = cvarname

    def has_args(self):
        return True

    def has_keywords(self):
        return False

    def __call__(self, writer):
        writer.gen_vardecl(typename='std::vector<SatLiteral>',
                           varname=self.__cvarname)
        with writer.gen_if_block(f'!parse_lits(args, {self.__cvarname})'):
            writer.gen_return('nullptr')


class LitLitsParser:
    """
    (出力を表す)SatLiteral と 1つ以上の SatLiteral のシーケンスを
    読み込むパーサークラス
    """

    def __init__(self, *,
                 ovarname,
                 cvarname):
        self.__ovarname = ovarname
        self.__cvarname = cvarname

    def has_args(self):
        return True

    def has_keywords(self):
        return False

    def __call__(self, writer):
        writer.gen_vardecl(typename='SatLiteral',
                           varname=self.__ovarname)
        writer.gen_vardecl(typename='std::vector<SatLiteral>',
                           varname=self.__cvarname)
        with writer.gen_if_block(f'!parse_lit_lits(args, {self.__ovarname}, {self.__cvarname})'):
            writer.gen_return('nullptr')


class SatSolverGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='SatSolver',
                         pyname='SatSolver',
                         namespace='YM',
                         header_include_files=['ym/SatSolver.h'],
                         source_include_files=['pym/PySatSolver.h',
                                               'pym/PySatInitParam.h',
                                               'pym/PySatBool3.h',
                                               'pym/PySatLiteral.h',
                                               'pym/PySatModel.h',
                                               'pym/PyList.h',
                                               ])

        def preamble_body(writer):
            writer.gen_include('parse_lits.cc')
        self.add_preamble(preamble_body)

        def new_body(writer):
            with writer.gen_if_block('obj == nullptr'):
                writer.gen_tp_alloc(objclass='SatSolver_Object')
                writer.gen_stmt('new (&my_obj->mVal) SatSolver()')
                writer.gen_return_self()
            with writer.gen_else_block():
                writer.gen_autoref_assign('init_param',
                                          'PySatInitParam::_get_ref(obj)')
                writer.gen_tp_alloc(objclass='SatSolver_Object')
                writer.gen_stmt('new (&my_obj->mVal) SatSolver(init_param)')
                writer.gen_return_self()
        self.add_new(func_body=new_body,
                     arg_list=[OptArg(),
                               TypedRawObjArg(name='init_param',
                                              pytypename='PySatInitParam::_typeobject()',
                                              cvarname='obj',
                                              cvardefault='nullptr')])

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
            writer.gen_stmt('val.set_conditional_literals(lits_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('set_conditional_literals',
                                    func_body=meth_set_conditional_literals,
                                    arg_parser=LitsParser(cvarname='lits_list'),
                                    doc_str="set 'conditional literals'")

        def meth_clear_conditional_literals(writer):
            writer.gen_stmt('val.clear_conditional_literals()')
            writer.gen_return_py_none()
        self.add_method('clear_conditional_literals',
                        func_body=meth_clear_conditional_literals,
                        doc_str="clear 'conditional literals'")

        def meth_add_clause(writer):
            writer.gen_stmt('val.add_clause(lits_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('add_clause',
                                    func_body=meth_add_clause,
                                    arg_parser=LitsParser(cvarname='lits_list'),
                                    doc_str='add clause')

        def meth_add_buffgate(writer):
            writer.gen_stmt('val.add_buffgate(lit1, lit2)')
            writer.gen_return_py_none()
        self.add_method('add_buffgate',
                        func_body=meth_add_buffgate,
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

        def meth_add_notgate(writer):
            writer.gen_stmt('val.add_notgate(lit1, lit2)')
            writer.gen_return_py_none()
        self.add_method('add_notgate',
                        func_body=meth_add_notgate,
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
                        doc_str="add clauses representing 'NOT' gate")

        def meth_add_andgate(writer):
            writer.gen_stmt('val.add_andgate(olit, lits_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('add_andgate',
                                    func_body=meth_add_andgate,
                                    arg_parser=LitLitsParser(ovarname='olit',
                                                             cvarname='lits_list'),
                                    doc_str="add clauses representing 'AND' gate")

        def meth_add_nandgate(writer):
            writer.gen_stmt('val.add_nandgate(olit, lits_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('add_nandgate',
                                    func_body=meth_add_nandgate,
                                    arg_parser=LitLitsParser(ovarname='olit',
                                                             cvarname='lits_list'),
                                    doc_str="add clauses representing 'NAND' gate")

        def meth_add_orgate(writer):
            writer.gen_stmt('val.add_orgate(olit, lits_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('add_orgate',
                                    func_body=meth_add_orgate,
                                    arg_parser=LitLitsParser(ovarname='olit',
                                                             cvarname='lits_list'),
                                    doc_str="add clauses representing 'OR' gate")

        def meth_add_norgate(writer):
            writer.gen_stmt('val.add_norgate(olit, lits_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('add_norgate',
                                    func_body=meth_add_norgate,
                                    arg_parser=LitLitsParser(ovarname='olit',
                                                             cvarname='lits_list'),
                                    doc_str="add clauses representing 'NOR' gate")

        def meth_add_xorgate(writer):
            writer.gen_stmt('val.add_xorgate(olit, lits_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('add_xorgate',
                                    func_body=meth_add_xorgate,
                                    arg_parser=LitLitsParser(ovarname='olit',
                                                             cvarname='lits_list'),
                                    doc_str="add clauses representing 'XOR' gate")

        def meth_add_xnorgate(writer):
            writer.gen_stmt('val.add_xnorgate(olit, lits_list)')
            writer.gen_return_py_none()
        self.add_method_with_parser('add_xnorgate',
                                    func_body=meth_add_xnorgate,
                                    arg_parser=LitLitsParser(ovarname='olit',
                                                             cvarname='lits_list'),
                                    doc_str="add clauses representing 'XNOR' gate")

        def meth_add_half_adder(writer):
            writer.gen_stmt('val.add_half_adder(alit, blit, slit, clit)')
            writer.gen_return_py_none()
        self.add_method('add_half_adder',
                        func_body=meth_add_half_adder,
                        arg_list=[TypedObjConvArg(name='a',
                                                  cvarname='alit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  TypedObjConvArg(name='b',
                                                  cvarname='blit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  TypedObjConvArg(name='s',
                                                  cvarname='slit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  TypedObjConvArg(name='c',
                                                  cvarname='clit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral')],
                        doc_str="add clauses representing 'HALF ADDER'")

        def meth_add_full_adder(writer):
            writer.gen_stmt('val.add_full_adder(alit, blit, ilit, slit, clit)')
            writer.gen_return_py_none()
        self.add_method('add_full_adder',
                        func_body=meth_add_full_adder,
                        arg_list=[TypedObjConvArg(name='a',
                                                  cvarname='alit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  TypedObjConvArg(name='b',
                                                  cvarname='blit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  TypedObjConvArg(name='i',
                                                  cvarname='ilit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  TypedObjConvArg(name='s',
                                                  cvarname='slit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  TypedObjConvArg(name='c',
                                                  cvarname='clit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral')],
                        doc_str="add clauses representing 'FULL ADDER'")

        def meth_add_adder(writer):
            writer.gen_vardecl(typename='std::vector<SatLiteral>',
                               varname='alits')
            with writer.gen_if_block('!PyList<SatLiteral, PySatLiteral>::FromPyObject(alits_obj, alits)'):
                writer.gen_type_error('"argument 1 should be a sequence of \'SatLiteral\'"')
            writer.gen_vardecl(typename='std::vector<SatLiteral>',
                               varname='blits')
            with writer.gen_if_block('!PyList<SatLiteral, PySatLiteral>::FromPyObject(blits_obj, blits)'):
                writer.gen_type_error('"argument 2 should be a sequence of \'SatLiteral\'"')
            writer.gen_vardecl(typename='std::vector<SatLiteral>',
                               varname='slits')
            with writer.gen_if_block('!PyList<SatLiteral, PySatLiteral>::FromPyObject(slits_obj, slits)'):
                writer.gen_type_error('"argument 4 should be a sequence of \'SatLiteral\'"')
            writer.gen_stmt('val.add_adder(alits, blits, ilit, slits, clit)')
            writer.gen_return_py_none()
        self.add_method('add_adder',
                        func_body=meth_add_adder,
                        arg_list=[RawObjArg(name='a',
                                            cvarname='alits_obj'),
                                  RawObjArg(name='b',
                                            cvarname='blits_obj'),
                                  TypedObjConvArg(name='i',
                                                  cvarname='ilit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral'),
                                  RawObjArg(name='s',
                                            cvarname='slits_obj'),
                                  TypedObjConvArg(name='c',
                                                  cvarname='clit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral')],
                        doc_str="add clauses representing 'ADDER'")

        def meth_solve(writer):
            writer.gen_vardecl(typename='std::vector<SatLiteral>',
                               varname='assumptions')
            with writer.gen_if_block('obj1 != nullptr'):
                with writer.gen_if_block('PySatLiteral::Check(obj1)'):
                    writer.gen_autoref_assign('lit', 'PySatLiteral::_get_ref(obj1)')
                    writer.gen_stmt('assumptions.push_back(lit)')
                with writer.gen_elseif_block('PyList<SatLiteral, PySatLiteral>::Check(obj1)'):
                    writer.gen_assign('assumptions',
                                      'PyList<SatLiteral, PySatLiteral>::Get(obj1)')
                with writer.gen_else_block():
                    writer.gen_type_error('"\'SatLiteral\' or sequence of \'SatLiteral\' is expected"')
            writer.gen_auto_assign('ans', 'val.solve(assumptions, time_limit)')
            writer.gen_return_pyobject('PySatBool3', 'ans')
        self.add_method('solve',
                        func_body=meth_solve,
                        arg_list=[OptArg(),
                                  RawObjArg(name='assumptions',
                                            cvarname='obj1'),
                                  KwdArg(),
                                  IntArg(name='time_limit',
                                         cvarname='time_limit',
                                         cvardefault=0)],
                        doc_str="solve SAT problem")

        def meth_model(writer):
            writer.gen_autoref_assign('model', 'val.model()')
            writer.gen_return_pyobject('PySatModel', 'model')
        self.add_method('model',
                        func_body=meth_model,
                        doc_str="return the model for SAT problem")

        def meth_read_model(writer):
            writer.gen_autoref_assign('model', 'val.model()')
            writer.gen_auto_assign('ans', 'model[lit]')
            writer.gen_return_pyobject('PySatBool3', 'ans')
        self.add_method('read_model',
                        func_body=meth_read_model,
                        arg_list=[TypedObjConvArg(name='lit',
                                                  cvarname='lit',
                                                  cvartype='SatLiteral',
                                                  cvardefault=None,
                                                  pyclassname='PySatLiteral')],
                        doc_str="read model value for specified literal")

        def meth_conflict_literal(writer):
            writer.gen_autoref_assign('lits', 'val.conflict_literals()')
            writer.gen_return_pyobject('PyList<SatLiteral, PySatLiteral>', 'lits')
        self.add_method('conflict_literals',
                        func_body=meth_conflict_literal,
                        doc_str="return conflict literals of the last SAT problem")


if __name__ == '__main__':

    gen = SatSolverGen()

    gen.make_header()

    gen.make_source()
