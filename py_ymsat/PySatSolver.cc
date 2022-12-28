
/// @file PySatSolver.cc
/// @brief Python SatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/PySatSolver.h"
#include "ym/PySatBool3.h"
#include "ym/PySatLiteral.h"
#include "ym/SatSolverType.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct SatSolverObject
{
  PyObject_HEAD
  SatSolver* mPtr;
};

// Python 用のタイプ定義
PyTypeObject PySatSolverType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
SatSolver_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  const char* type_str = nullptr;
  if ( !PyArg_ParseTuple(args, "!s", &type_str) ) {
    return nullptr;
  }
  SatSolverType solver_type;
  if ( type_str != nullptr ) {
    solver_type = SatSolverType{type_str};
  }
  auto self = type->tp_alloc(type, 0);
  auto satsolver_obj = reinterpret_cast<SatSolverObject*>(self);
  satsolver_obj->mPtr = new SatSolver{solver_type};
  return self;
}

// 終了関数
void
SatSolver_dealloc(
  PyObject* self
)
{
  auto satsolver_obj = reinterpret_cast<SatSolverObject*>(self);
  delete satsolver_obj->mPtr;
  Py_TYPE(self)->tp_free(self);
}

PyObject*
SatSolver_new_variable(
  PyObject* self,
  PyObject* args
)
{
  bool decision = true;
  if ( !PyArg_ParseTuple(args, "|b", &decision) ) {
    PyErr_SetString(PyExc_TypeError, "1st argument should be Boolean.");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  auto lit = solver.new_variable(decision);
  return PySatLiteral::ToPyObject(lit);
}

bool
parse_args(
  PyObject* args,
  vector<SatLiteral>& lit_list
)
{
  if ( PySatLiteral::_check(args) ) {
    auto lit = PySatLiteral::_get(args);
    lit_list.push_back(lit);
    return true;
  }
  if ( PySequence_Check(args) ) {
    auto n = PySequence_Size(args);
    for ( int i = 0; i < n; ++ i ) {
      auto arg = PySequence_GetItem(args, i);
      bool stat = parse_args(arg, lit_list);
      Py_DECREF(arg);
      if ( !stat ) {
	return false;
      }
    }
    return true;
  }
  PyErr_SetString(PyExc_TypeError, "SatLiteral or sequence of SatLiterals are expected.");
  return false;
}

PyObject*
SatSolver_set_conditional_literals(
  PyObject* self,
  PyObject* args
)
{
  // おおまかには SatLiteral のリストを引数に取る．
  // ただし，複数の SatLiteral の引数の場合もある．
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.set_conditional_literals(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_clear_conditional_literals(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& solver = PySatSolver::_get(self);
  solver.clear_conditional_literals();
  Py_RETURN_NONE;
}

PyObject*
SatSolver_freeze_literal(
  PyObject* self,
  PyObject* args
)
{
  // おおまかには SatLiteral のリストを引数に取る．
  // ただし，複数の SatLiteral の引数の場合もある．
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.freeze_literal(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_clause(
  PyObject* self,
  PyObject* args
)
{
  // おおまかには SatLiteral のリストを引数に取る．
  // ただし，複数の SatLiteral の引数の場合もある．
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_clause(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_solve(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "",
    "time_limit",
    nullptr
  };
  PyObject* obj1 = nullptr;
  int time_limit = 0;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O$i",
				    const_cast<char**>(kwlist),
				    &obj1, &time_limit) ) {
    return nullptr;
  }
  vector<SatLiteral> assumptions;
  if ( obj1 != nullptr ) {
    if ( !parse_args(obj1, assumptions) ) {
      return nullptr;
    }
  }
  auto& solver = PySatSolver::_get(self);
  auto ans = solver.solve(assumptions, time_limit);
  return PySatBool3::ToPyObject(ans);
}

PyObject*
SatSolver_model_size(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& solver = PySatSolver::_get(self);
  auto n = solver.model_size();
  return PyLong_FromLong(n);
}

PyObject*
SatSolver_read_model(
  PyObject* self,
  PyObject* args
)
{
  PyObject* lit_obj;
  if ( !PyArg_ParseTuple(args, "O!", PySatLiteral::_typeobject(), &lit_obj) ) {
    return nullptr;
  }
  auto lit = PySatLiteral::_get(lit_obj);
  auto& solver = PySatSolver::_get(self);
  auto val = solver.read_model(lit);
  return PySatBool3::ToPyObject(val);
}

// メソッド定義
PyMethodDef SatSolver_methods[] = {
  {"new_variable", SatSolver_new_variable, METH_VARARGS,
   PyDoc_STR("allocate a new variable(return SatLiteral)")},
  {"set_conditional_literals", SatSolver_set_conditional_literals, METH_VARARGS,
   PyDoc_STR("set conditional literals")},
  {"clear_conditional_literals", SatSolver_clear_conditional_literals, METH_NOARGS,
   PyDoc_STR("set conditional literals")},
  {"freeze_literal", SatSolver_freeze_literal, METH_VARARGS,
   PyDoc_STR("make the literal 'frozen'")},
  {"add_clause", SatSolver_add_clause, METH_VARARGS,
   PyDoc_STR("add clause")},
  {"solve", reinterpret_cast<PyCFunction>(SatSolver_solve), METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("solve the SAT problem")},
  {"model_size", SatSolver_model_size, METH_NOARGS,
   PyDoc_STR("return the size of the model")},
  {"read_model", SatSolver_read_model, METH_VARARGS,
   PyDoc_STR("return the value of the model")},
  {nullptr, nullptr, 0, nullptr}
};

END_NONAMESPACE


// @brief 'SatSolver' オブジェクトを使用可能にする．
bool
PySatSolver::init(
  PyObject* m
)
{
  PySatSolverType.tp_name = "sat.SatSolver";
  PySatSolverType.tp_basicsize = sizeof(SatSolverObject);
  PySatSolverType.tp_itemsize = 0;
  PySatSolverType.tp_dealloc = SatSolver_dealloc;
  PySatSolverType.tp_flags = Py_TPFLAGS_DEFAULT;
  PySatSolverType.tp_doc = PyDoc_STR("SatSolver objects");
  PySatSolverType.tp_methods = SatSolver_methods;
  PySatSolverType.tp_new = SatSolver_new;
  if ( PyType_Ready(&PySatSolverType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  auto type_obj = reinterpret_cast<PyObject*>(&PySatSolverType);
  Py_INCREF(type_obj);
  if ( PyModule_AddObject(m, "SatSolver", type_obj) < 0 ) {
    Py_DECREF(type_obj);
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が SatSolver タイプか調べる．
bool
PySatSolver::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief SatSolver を表す PyObject から SatSolver を取り出す．
SatSolver&
PySatSolver::_get(
  PyObject* obj
)
{
  auto satsolver_obj = reinterpret_cast<SatSolverObject*>(obj);
  return *satsolver_obj->mPtr;
}

// @brief SatSolver を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatSolver::_typeobject()
{
  return &PySatSolverType;
}

END_NAMESPACE_YM
