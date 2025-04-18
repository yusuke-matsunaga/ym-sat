#! /usr/bin/env python3

""" PySatBool3 を生成するスクリプト

:file: primtype_gen.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

from mk_py_capi import EnumGen, EnumInfo


class SatBool3Gen(EnumGen):

    def __init__(self):
        super().__init__(classname='SatBool3',
                         pyname='SatBool3',
                         namespace='YM',
                         enum_list=[EnumInfo('SatBool3::False', 'false', 'false'),
                                    EnumInfo('SatBool3::True', 'true', 'true'),
                                    EnumInfo('SatBool3::X', 'x', 'x'),
                                    ],
                         ignore_case=True,
                         header_include_files=['ym/SatBool3.h'],
                         source_include_files=['pym/PySatBool3.h',
                                               'pym/PyString.h'])

        self.add_nb_invert()
        self.add_nb_xor()

        def bool_body(writer):
            writer.gen_return('val == SatBool3::True')
        self.add_nb_bool(body=bool_body)


if __name__ == '__main__':

    gen = SatBool3Gen()

    gen.make_header()
    gen.make_source()
