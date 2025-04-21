#! /usr/bin/env python3

""" PySatInitParam を生成するスクリプト

:file: satinitparam_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import PyObjGen
from mk_py_capi import RawObjArg, StringArg, OptArg


class SatInitParamGen(PyObjGen):

    def __init__(self):
        super().__init__(classname='SatInitParam',
                         pyname='SatInitParam',
                         namespace='YM',
                         header_include_files=['ym/SatInitParam.h'],
                         source_include_files=['pym/PySatInitParam.h',
                                               'pym/PyJsonValue.h',
                                               'pym/PyString.h',
                                               ])

        def new_body(writer):
            with writer.gen_if_block('obj == nullptr'):
                writer.gen_tp_alloc(objclass='SatInitParam_Object')
                writer.gen_stmt('new (&my_obj->mVal) SatInitParam()')
                writer.gen_return_self()
            with writer.gen_else_block():
                writer.gen_vardecl(typename='std::string',
                                   varname='str_val')
                with writer.gen_if_block('PyString::FromPyObject(obj, str_val)'):
                    writer.gen_tp_alloc(objclass='SatInitParam_Object')
                    writer.gen_stmt('new (&my_obj->mVal) SatInitParam(str_val)')
                    writer.gen_return_self()
                writer.gen_vardecl(typename='JsonValue',
                                   varname='js_val')
                with writer.gen_if_block('PyJsonValue::FromPyObject(obj, js_val)'):
                    writer.gen_tp_alloc(objclass='SatInitParam_Object')
                    writer.gen_stmt('new (&my_obj->mVal) SatInitParam(js_val)')
                    writer.gen_return_self()
                writer.gen_type_error('"invalid parameter type for \'SatInitParam\'"')
        self.add_new(func_body=new_body,
                     arg_list=[OptArg(),
                               RawObjArg(cvarname='obj',
                                         cvardefault='nullptr')])

        self.add_dealloc('default')

        def meth_from_json(writer):
            writer.gen_auto_assign('val', 'SatInitParam::from_json(filename)')
            writer.gen_auto_assign('type', '&SatInitParam_Type')
            writer.gen_tp_alloc(objclass='SatInitParam_Object')
            writer.gen_stmt('new (&my_obj->mVal) SatInitParam(val)')
            writer.gen_return_self()
        self.add_static_method('from_json',
                               func_body=meth_from_json,
                               arg_list=[StringArg(name='filename',
                                                cvarname='filename')],
                               doc_str='read JSON file and return new SatInitParam object')


if __name__ == '__main__':

    gen = SatInitParamGen()

    gen.make_header()

    gen.make_source()
