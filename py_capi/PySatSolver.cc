
/// @file PySatSolver.cc
/// @brief PySatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatSolver.h"
#include "pym/PySatBool3.h"
#include "pym/PySatLiteral.h"
#include "pym/PyList.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct SatSolver_Object
{
  PyObject_HEAD
  SatSolver mVal;
};

// Python 用のタイプ定義
PyTypeObject SatSolver_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PySatSolver::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<SatSolver_Object*>(self);
  obj->mVal.~SatSolver();
  Py_TYPE(self)->tp_free(self);
}

// make new variable
PyObject*
new_variable(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "decision",
    nullptr
  };
  int decision_tmp = -1;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|p",
                                    const_cast<char**>(kwlist),
                                    &decision_tmp) ) {
    return nullptr;
  }
  bool decision = true;
  if ( decision_tmp != -1 ) {
    decision = static_cast<bool>(decision_tmp);
  }
  auto& val = PySatSolver::_get_ref(self);
  return PySatLiteral::ToPyObject(val.new_variable(decision));
}

// set 'conditional literals'
PyObject*
set_conditional_literals(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "lit_list",
    nullptr
  };
  PyObject* lit_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
                                    const_cast<char**>(kwlist),
                                    &lit_list_obj) ) {
    return nullptr;
  }
  std::vector<SatLiteral> lit_list;
  if ( lit_list_obj != nullptr ) {
    if ( !PyList<SatLiteral, PySatLiteral>::FromPyObject(lit_list_obj, lit_list) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to std::vector<SatLiteral>");
      return nullptr;
    }
  }
  auto& val = PySatSolver::_get_ref(self);
  val.set_conditional_literals(lit_list);
  Py_RETURN_NONE;
}

// clear 'conditional literals'
PyObject*
clear_conditional_literals(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatSolver::_get_ref(self);
  val.clear_conditional_literals();
  Py_RETURN_NONE;
}

// add clause
PyObject*
add_clause(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "lit_list",
    nullptr
  };
  PyObject* lit_list_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
                                    const_cast<char**>(kwlist),
                                    &lit_list_obj) ) {
    return nullptr;
  }
  std::vector<SatLiteral> lit_list;
  if ( lit_list_obj != nullptr ) {
    if ( PySatLiteral::Check(lit_list_obj) ) {
      auto lit = PySatLiteral::_get_ref(lit_list_obj);
      lit_list.push_back(lit);
    }
    else if ( !PyList<SatLiteral, PySatLiteral>::FromPyObject(lit_list_obj, lit_list) ) {
      PyErr_SetString(PyExc_TypeError, "'a SatLiteral' or a sequence of 'SatLiteral's is expected");
      return nullptr;
    }
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_clause(lit_list);
  Py_RETURN_NONE;
}

// add clauses representing 'BUF' gate
PyObject*
add_bufgate(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "lit1",
    "lit2",
    nullptr
  };
  PyObject* lit1_obj = nullptr;
  PyObject* lit2_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!",
                                    const_cast<char**>(kwlist),
                                    PySatLiteral::_typeobject(), &lit1_obj,
                                    PySatLiteral::_typeobject(), &lit2_obj) ) {
    return nullptr;
  }
  SatLiteral lit1;
  if ( lit1_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(lit1_obj, lit1) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral lit2;
  if ( lit2_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(lit2_obj, lit2) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_buffgate(lit1, lit2);
  Py_RETURN_NONE;
}

// メソッド定義
PyMethodDef methods[] = {
  {"new_variable",
   reinterpret_cast<PyCFunction>(new_variable),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("make new variable")},
  {"set_conditional_literals",
   reinterpret_cast<PyCFunction>(set_conditional_literals),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("set 'conditional literals'")},
  {"clear_conditional_literals",
   clear_conditional_literals,
   METH_NOARGS,
   PyDoc_STR("clear 'conditional literals'")},
  {"add_clause",
   reinterpret_cast<PyCFunction>(add_clause),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add clause")},
  {"add_bufgate",
   reinterpret_cast<PyCFunction>(add_bufgate),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add clauses representing 'BUF' gate")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

// new 関数
PyObject*
new_func(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'SatSolver' is disabled");
  return nullptr;
}

END_NONAMESPACE


// @brief SatSolver オブジェクトを使用可能にする．
bool
PySatSolver::init(
  PyObject* m
)
{
  SatSolver_Type.tp_name = "SatSolver";
  SatSolver_Type.tp_basicsize = sizeof(SatSolver_Object);
  SatSolver_Type.tp_itemsize = 0;
  SatSolver_Type.tp_dealloc = dealloc_func;
  SatSolver_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  SatSolver_Type.tp_doc = PyDoc_STR("Python extended object for SatSolver");
  SatSolver_Type.tp_methods = methods;
  SatSolver_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "SatSolver", &SatSolver_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が SatSolver タイプか調べる．
bool
PySatSolver::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &SatSolver_Type);
}

// @brief PyObject から SatSolver を取り出す．
SatSolver&
PySatSolver::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<SatSolver_Object*>(obj);
  return my_obj->mVal;
}

// @brief SatSolver を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatSolver::_typeobject()
{
  return &SatSolver_Type;
}

END_NAMESPACE_YM
