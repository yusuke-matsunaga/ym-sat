#! /usr/bin/env python3

""" ファイルを生成するスクリプト

:file: mk_all.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2025 Yusuke Matsunaga, All rights reserved.
"""

import os
import sys
from argparse import ArgumentParser
from pathlib import Path


# このファイルのあるディレクトリ
curdir = os.path.dirname(__file__)
rootdir = os.path.join(curdir, '..', '..', '..')
module_path = os.path.join(rootdir, 'ym-common', 'utils')

sys.path.append(module_path)

parser = ArgumentParser(prog='mk_all',
                        description="make 'ymsat' module")

parser.add_argument('-i', '--include_dir', type=str)
parser.add_argument('-s', '--source_dir', type=str)

args = parser.parse_args()

if args.include_dir is None:
    include_dir = os.path.join(curdir, '..', '..', 'include', 'pym')
else:
    include_dir = args.include_dir
if args.source_dir is None:
    source_dir = os.path.join(curdir, '..')
else:
    source_dir = args.source_dir

from ymsat_gen import YmsatGen
ymsat_gen = YmsatGen()
filename = os.path.join(include_dir, 'ymsat.h')
with open(filename, 'wt') as fout:
    ymsat_gen.make_header(fout=fout)
filename = os.path.join(source_dir, 'ymsat_module.cc')
with open(filename, 'wt') as fout:
    ymsat_gen.make_source(fout=fout)

from cnfsize_gen import CnfSizeGen
cnfsize_gen = CnfSizeGen()

from satbool3_gen import SatBool3Gen
satbool3_gen = SatBool3Gen()

from satliteral_gen import SatLiteralGen
satliteral_gen = SatLiteralGen()

from satmodel_gen import SatModelGen
satmodel_gen = SatModelGen()

from satsolver_gen import SatSolverGen
satsolver_gen = SatSolverGen()

for gen, name in ((cnfsize_gen, 'PyCnfSize'),
                  (satbool3_gen, 'PySatBool3'),
                  (satliteral_gen, 'PySatLiteral'),
                  (satmodel_gen, 'PySatModel'),
                  (satsolver_gen, 'PySatSolver'),
                  ):
    filename = os.path.join(include_dir, f'{name}.h')
    with open(filename, 'wt') as fout:
        gen.make_header(fout=fout)
    filename = os.path.join(source_dir, f'{name}.cc')
    with open(filename, 'wt') as fout:
        gen.make_source(fout=fout)
