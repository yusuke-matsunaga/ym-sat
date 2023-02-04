
/// @file ymsat_module.cc
/// @brief Python 用の ymsat モジュールの実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "pym/PySatBool3.h"
#include "pym/PySatLiteral.h"
#include "pym/PySatSolver.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// メソッド定義構造体
PyMethodDef ymsat_methods[] = {
  {nullptr, nullptr, 0, nullptr},
};

// モジュール定義構造体
PyModuleDef ymsat_module = {
  PyModuleDef_HEAD_INIT,
  "ymsat",
  PyDoc_STR("ymsat: Extension module for SAT solver"),
  -1,
  ymsat_methods,
};

END_NONAMESPACE


PyMODINIT_FUNC
PyInit_ymsat()
{
  auto m = PyModule::init(&ymsat_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PySatBool3::init(m) ) {
    goto error;
  }

  if ( !PySatLiteral::init(m) ) {
    goto error;
  }

  if ( !PySatSolver::init(m) ) {
    goto error;
  }

  return m;

 error:
  Py_DECREF(m);
  return nullptr;
}

END_NAMESPACE_YM
