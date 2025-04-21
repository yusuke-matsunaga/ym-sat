
/// @file ymsat_module.cc
/// @brief Python 用の ymsat モジュールを定義する．
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "pym/PyCnfSize.h"
#include "pym/PySatBool3.h"
#include "pym/PySatLiteral.h"
#include "pym/PySatModel.h"
#include "pym/PySatInitParam.h"
#include "pym/PySatSolver.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// メソッド定義
PyMethodDef methods[] = {
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

// モジュール定義構造体
PyModuleDef ymsat_module = {
  PyModuleDef_HEAD_INIT,
  "ymsat",
  PyDoc_STR(""),
  -1,
  methods,
};

END_NONAMESPACE

PyMODINIT_FUNC
PyInit_ymsat()
{
  auto m = PyModule::init(&ymsat_module);
  if ( m == nullptr ) {
    return nullptr;
  }

  if ( !PyCnfSize::init(m) ) {
    goto error;
  }
  if ( !PySatBool3::init(m) ) {
    goto error;
  }
  if ( !PySatLiteral::init(m) ) {
    goto error;
  }
  if ( !PySatModel::init(m) ) {
    goto error;
  }
  if ( !PySatInitParam::init(m) ) {
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
