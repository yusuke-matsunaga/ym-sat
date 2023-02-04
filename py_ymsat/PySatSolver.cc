
/// @file PySatSolver.cc
/// @brief Python SatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatSolver.h"
#include "pym/PySatBool3.h"
#include "pym/PySatLiteral.h"
#include "pym/PyModule.h"
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
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "type",
    nullptr
  };
  const char* type_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|s",
				    const_cast<char**>(kwlist),
				    &type_str) ) {
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
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "decision",
    nullptr
  };
  int decision = true;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|p",
				    const_cast<char**>(kwlist),
				    &decision) ) {
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
SatSolver_add_buffgate(
  PyObject* self,
  PyObject* args
)
{
  PyObject* lit1_obj = nullptr;
  PyObject* lit2_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!O!",
			 PySatLiteral::_typeobject(), &lit1_obj,
			 PySatLiteral::_typeobject(), &lit2_obj) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  auto lit1 = PySatLiteral::_get(lit1_obj);
  auto lit2 = PySatLiteral::_get(lit2_obj);
  solver.add_buffgate(lit1, lit2);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_notgate(
  PyObject* self,
  PyObject* args
)
{
  PyObject* lit1_obj = nullptr;
  PyObject* lit2_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!O!",
			 PySatLiteral::_typeobject(), &lit1_obj,
			 PySatLiteral::_typeobject(), &lit2_obj) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  auto lit1 = PySatLiteral::_get(lit1_obj);
  auto lit2 = PySatLiteral::_get(lit2_obj);
  solver.add_notgate(lit1, lit2);
  Py_RETURN_NONE;
}

bool
parse_args2(
  PyObject* args,
  SatLiteral& olit,
  vector<SatLiteral>& lit_list
)
{
  if ( PySequence_Check(args) ) {
    auto n = PySequence_Size(args);
    if ( n <= 1 ) {
      PyErr_SetString(PyExc_TypeError, "SatLiteral and sequence of SatLiterals are expected.");
      return false;
    }
    {
      auto arg0 = PySequence_GetItem(args, 0);
      if ( !PySatLiteral::_check(arg0) ) {
	Py_DECREF(arg0);
	PyErr_SetString(PyExc_TypeError, "1st argument should be a SatLiteral.");
	return false;
      }
      olit = PySatLiteral::_get(arg0);
      Py_DECREF(arg0);
    }
    for ( int i = 1; i < n; ++ i ) {
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
SatSolver_add_andgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  vector<SatLiteral> lit_list;
  if ( !parse_args2(args, olit, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_andgate(olit, lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_nandgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  vector<SatLiteral> lit_list;
  if ( !parse_args2(args, olit, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_nandgate(olit, lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_orgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  vector<SatLiteral> lit_list;
  if ( !parse_args2(args, olit, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_orgate(olit, lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_norgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  vector<SatLiteral> lit_list;
  if ( !parse_args2(args, olit, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_norgate(olit, lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_xorgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  vector<SatLiteral> lit_list;
  if ( !parse_args2(args, olit, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_xorgate(olit, lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_xnorgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  vector<SatLiteral> lit_list;
  if ( !parse_args2(args, olit, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_xnorgate(olit, lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_half_adder(
  PyObject* self,
  PyObject* args
)
{
  PyObject* alit_obj = nullptr;
  PyObject* blit_obj = nullptr;
  PyObject* slit_obj = nullptr;
  PyObject* clit_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!O!O!O!",
			 PySatLiteral::_typeobject(), &alit_obj,
			 PySatLiteral::_typeobject(), &blit_obj,
			 PySatLiteral::_typeobject(), &slit_obj,
			 PySatLiteral::_typeobject(), &clit_obj) ) {
    return nullptr;
  }
  auto alit = PySatLiteral::_get(alit_obj);
  auto blit = PySatLiteral::_get(blit_obj);
  auto slit = PySatLiteral::_get(slit_obj);
  auto clit = PySatLiteral::_get(clit_obj);
  auto& solver = PySatSolver::_get(self);
  solver.add_half_adder(alit, blit, slit, clit);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_full_adder(
  PyObject* self,
  PyObject* args
)
{
  PyObject* alit_obj = nullptr;
  PyObject* blit_obj = nullptr;
  PyObject* ilit_obj = nullptr;
  PyObject* slit_obj = nullptr;
  PyObject* clit_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O!O!O!O!O!",
			 PySatLiteral::_typeobject(), &alit_obj,
			 PySatLiteral::_typeobject(), &blit_obj,
			 PySatLiteral::_typeobject(), &ilit_obj,
			 PySatLiteral::_typeobject(), &slit_obj,
			 PySatLiteral::_typeobject(), &clit_obj) ) {
    return nullptr;
  }
  auto alit = PySatLiteral::_get(alit_obj);
  auto blit = PySatLiteral::_get(blit_obj);
  auto ilit = PySatLiteral::_get(ilit_obj);
  auto slit = PySatLiteral::_get(slit_obj);
  auto clit = PySatLiteral::_get(clit_obj);
  auto& solver = PySatSolver::_get(self);
  solver.add_full_adder(alit, blit, ilit, slit, clit);
  Py_RETURN_NONE;
}

bool
conv_to_lit_list(
  PyObject* obj,
  vector<SatLiteral>& lit_list
)
{
  lit_list.clear();
  if ( !PySequence_Check(obj) ) {
    return false;
  }
  SizeType n = PySequence_Size(obj);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto obj1 = PySequence_GetItem(obj, i);
    if ( !PySatLiteral::_check(obj1) ) {
      Py_DECREF(obj1);
      return false;
    }
    auto lit = PySatLiteral::_get(obj1);
    lit_list.push_back(lit);
    Py_DECREF(obj1);
  }
  return true;
}

PyObject*
SatSolver_add_adder(
  PyObject* self,
  PyObject* args
)
{
  PyObject* alits_obj = nullptr;
  PyObject* blits_obj = nullptr;
  PyObject* ilit_obj = nullptr;
  PyObject* slits_obj = nullptr;
  PyObject* clit_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "OOO!OO!",
			 &alits_obj, &blits_obj,
			 PySatLiteral::_typeobject(), &ilit_obj,
			 &slits_obj,
			 PySatLiteral::_typeobject(), &clit_obj) ) {
    return nullptr;
  }
  vector<SatLiteral> alits;
  if ( !conv_to_lit_list(alits_obj, alits) ) {
    PyErr_SetString(PyExc_TypeError, "1st argument should be a list of SatLiteral");
    return nullptr;
  }
  vector<SatLiteral> blits;
  if ( !conv_to_lit_list(blits_obj, blits) ) {
    PyErr_SetString(PyExc_TypeError, "2nd argument should be a list of SatLiteral");
    return nullptr;
  }
  auto ilit = PySatLiteral::_get(ilit_obj);
  vector<SatLiteral> slits;
  if ( !conv_to_lit_list(slits_obj, slits) ) {
    PyErr_SetString(PyExc_TypeError, "4th argument should be a list of SatLiteral");
    return nullptr;
  }
  auto clit = PySatLiteral::_get(clit_obj);
  auto& solver = PySatSolver::_get(self);
  solver.add_adder(alits, blits, ilit, slits, clit);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_counter(
  PyObject* self,
  PyObject* args
)
{
  PyObject* ilits_obj = nullptr;
  if ( !PyArg_ParseTuple(args, "O", &ilits_obj) ) {
    return nullptr;
  }
  vector<SatLiteral> ilits;
  if ( !conv_to_lit_list(ilits_obj, ilits) ) {
    PyErr_SetString(PyExc_TypeError, "1st argument should be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  auto olits = solver.add_counter(ilits);
  SizeType n = olits.size();
  PyObject* ans_obj = PyTuple_New(n);
  for ( SizeType i = 0; i < n; ++ i ) {
    auto lit_obj = PySatLiteral::ToPyObject(olits[i]);
    PyTuple_SetItem(ans_obj, i, lit_obj);
  }
  return ans_obj;
}

PyObject*
SatSolver_add_at_most_one(
  PyObject* self,
  PyObject* args
)
{
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_at_most_one(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_at_most_two(
  PyObject* self,
  PyObject* args
)
{
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_at_most_two(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_at_most_k(
  PyObject* self,
  PyObject* args
)
{
  PyObject* ilits_obj = nullptr;
  int k;
  if ( !PyArg_ParseTuple(args, "Oi", &ilits_obj, &k) ) {
    return nullptr;
  }
  vector<SatLiteral> lit_list;
  if ( !conv_to_lit_list(ilits_obj, lit_list) ) {
    PyErr_SetString(PyExc_TypeError, "1st argument should be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_at_most_k(lit_list, k);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_at_least_one(
  PyObject* self,
  PyObject* args
)
{
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_at_least_one(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_at_least_two(
  PyObject* self,
  PyObject* args
)
{
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_at_least_two(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_at_least_k(
  PyObject* self,
  PyObject* args
)
{
  PyObject* ilits_obj = nullptr;
  int k;
  if ( !PyArg_ParseTuple(args, "Oi", &ilits_obj, &k) ) {
    return nullptr;
  }
  vector<SatLiteral> lit_list;
  if ( !conv_to_lit_list(ilits_obj, lit_list) ) {
    PyErr_SetString(PyExc_TypeError, "1st argument should be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_at_least_k(lit_list, k);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_exact_one(
  PyObject* self,
  PyObject* args
)
{
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_exact_one(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_exact_two(
  PyObject* self,
  PyObject* args
)
{
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_exact_two(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_exact_k(
  PyObject* self,
  PyObject* args
)
{
  PyObject* ilits_obj = nullptr;
  int k;
  if ( !PyArg_ParseTuple(args, "Oi", &ilits_obj, &k) ) {
    return nullptr;
  }
  vector<SatLiteral> lit_list;
  if ( !conv_to_lit_list(ilits_obj, lit_list) ) {
    PyErr_SetString(PyExc_TypeError, "1st argument should be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_exact_k(lit_list, k);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_not_one(
  PyObject* self,
  PyObject* args
)
{
  vector<SatLiteral> lit_list;
  if ( !parse_args(args, lit_list) ) {
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  solver.add_not_one(lit_list);
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_eq(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "OO", &obj1, &obj2) ) {
    return nullptr;
  }
  vector<SatLiteral> lit1_list;
  if ( !conv_to_lit_list(obj1, lit1_list) ) {
    PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  if ( PyLong_Check(obj2) ) {
    int bval = PyLong_AsLong(obj2);
    solver.add_eq(lit1_list, bval);
  }
  else {
    vector<SatLiteral> lit2_list;
    if ( !conv_to_lit_list(obj2, lit2_list) ) {
      PyErr_SetString(PyExc_TypeError, "argument 1 must be an int or a list of SatLiteral");
      return nullptr;
    }
    solver.add_eq(lit1_list, lit2_list);
  }
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_ne(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "OO", &obj1, &obj2) ) {
    return nullptr;
  }
  vector<SatLiteral> lit1_list;
  if ( !conv_to_lit_list(obj1, lit1_list) ) {
    PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  if ( PyLong_Check(obj2) ) {
    int bval = PyLong_AsLong(obj2);
    solver.add_ne(lit1_list, bval);
  }
  else {
    vector<SatLiteral> lit2_list;
    if ( !conv_to_lit_list(obj2, lit2_list) ) {
      PyErr_SetString(PyExc_TypeError, "argument 2 must be an int or a list of SatLiteral");
      return nullptr;
    }
    solver.add_ne(lit1_list, lit2_list);
  }
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_lt(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "OO", &obj1, &obj2) ) {
    return nullptr;
  }
  vector<SatLiteral> lit1_list;
  if ( !conv_to_lit_list(obj1, lit1_list) ) {
    PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  if ( PyLong_Check(obj2) ) {
    int bval = PyLong_AsLong(obj2);
    solver.add_lt(lit1_list, bval);
  }
  else {
    vector<SatLiteral> lit2_list;
    if ( !conv_to_lit_list(obj2, lit2_list) ) {
      PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
      return nullptr;
    }
    solver.add_lt(lit1_list, lit2_list);
  }
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_le(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "OO", &obj1, &obj2) ) {
    return nullptr;
  }
  vector<SatLiteral> lit1_list;
  if ( !conv_to_lit_list(obj1, lit1_list) ) {
    PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  if ( PyLong_Check(obj2) ) {
    int bval = PyLong_AsLong(obj2);
    solver.add_le(lit1_list, bval);
  }
  else {
    vector<SatLiteral> lit2_list;
    if ( !conv_to_lit_list(obj2, lit2_list) ) {
      PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
      return nullptr;
    }
    solver.add_le(lit1_list, lit2_list);
  }
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_gt(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "OO", &obj1, &obj2) ) {
    return nullptr;
  }
  vector<SatLiteral> lit1_list;
  if ( !conv_to_lit_list(obj1, lit1_list) ) {
    PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  if ( PyLong_Check(obj2) ) {
    int bval = PyLong_AsLong(obj2);
    solver.add_gt(lit1_list, bval);
  }
  else {
    vector<SatLiteral> lit2_list;
    if ( !conv_to_lit_list(obj2, lit2_list) ) {
      PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
      return nullptr;
    }
    solver.add_gt(lit1_list, lit2_list);
  }
  Py_RETURN_NONE;
}

PyObject*
SatSolver_add_ge(
  PyObject* self,
  PyObject* args
)
{
  PyObject* obj1 = nullptr;
  PyObject* obj2 = nullptr;
  if ( !PyArg_ParseTuple(args, "OO", &obj1, &obj2) ) {
    return nullptr;
  }
  vector<SatLiteral> lit1_list;
  if ( !conv_to_lit_list(obj1, lit1_list) ) {
    PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
    return nullptr;
  }
  auto& solver = PySatSolver::_get(self);
  if ( PyLong_Check(obj2) ) {
    int bval = PyLong_AsLong(obj2);
    solver.add_ge(lit1_list, bval);
  }
  else {
    vector<SatLiteral> lit2_list;
    if ( !conv_to_lit_list(obj2, lit2_list) ) {
      PyErr_SetString(PyExc_TypeError, "argument 1 must be a list of SatLiteral");
      return nullptr;
    }
    solver.add_ge(lit1_list, lit2_list);
  }
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
    "assumptions",
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
  {"new_variable", reinterpret_cast<PyCFunction>(SatSolver_new_variable),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("allocate a new variable(return SatLiteral)")},
  {"set_conditional_literals", SatSolver_set_conditional_literals, METH_VARARGS,
   PyDoc_STR("set conditional literals")},
  {"clear_conditional_literals", SatSolver_clear_conditional_literals, METH_NOARGS,
   PyDoc_STR("set conditional literals")},
  {"add_clause", SatSolver_add_clause, METH_VARARGS,
   PyDoc_STR("add clause")},
  {"add_buffgate", SatSolver_add_buffgate, METH_VARARGS,
   PyDoc_STR("add clause representing BUFF gate")},
  {"add_notgate", SatSolver_add_notgate, METH_VARARGS,
   PyDoc_STR("add clause representing NOT gate")},
  {"add_andgate", SatSolver_add_andgate, METH_VARARGS,
   PyDoc_STR("add clause representing AND gate")},
  {"add_nandgate", SatSolver_add_nandgate, METH_VARARGS,
   PyDoc_STR("add clause representing NAND gate")},
  {"add_orgate", SatSolver_add_orgate, METH_VARARGS,
   PyDoc_STR("add clause representing OR gate")},
  {"add_norgate", SatSolver_add_norgate, METH_VARARGS,
   PyDoc_STR("add clause representing NOR gate")},
  {"add_xorgate", SatSolver_add_xorgate, METH_VARARGS,
   PyDoc_STR("add clause representing XOR gate")},
  {"add_xnorgate", SatSolver_add_xnorgate, METH_VARARGS,
   PyDoc_STR("add clause representing XNOR gate")},
  {"add_half_adder", SatSolver_add_half_adder, METH_VARARGS,
   PyDoc_STR("add clause representing HALF-ADDER")},
  {"add_full_adder", SatSolver_add_full_adder, METH_VARARGS,
   PyDoc_STR("add clause representing FULL-ADDER")},
  {"add_adder", SatSolver_add_adder, METH_VARARGS,
   PyDoc_STR("add clause representing ADDER")},
  {"add_counter", SatSolver_add_counter, METH_VARARGS,
   PyDoc_STR("add clause representing COUNTER")},
  {"add_at_most_one", SatSolver_add_at_most_one, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-most-one'")},
  {"add_at_most_two", SatSolver_add_at_most_two, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-most-two'")},
  {"add_at_most_k", SatSolver_add_at_most_k, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-most-K'")},
  {"add_at_least_one", SatSolver_add_at_least_one, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-least-one'")},
  {"add_at_least_two", SatSolver_add_at_least_two, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-least-two'")},
  {"add_at_least_k", SatSolver_add_at_least_k, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-least-K'")},
  {"add_exact_one", SatSolver_add_exact_one, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-least-one'")},
  {"add_exact_two", SatSolver_add_exact_two, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-least-two'")},
  {"add_exact_k", SatSolver_add_exact_k, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'at-least-K'")},
  {"add_not_one", SatSolver_add_not_one, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'not-one'")},
  {"add_eq", SatSolver_add_eq, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'A = B'")},
  {"add_ne", SatSolver_add_ne, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'A != B'")},
  {"add_lt", SatSolver_add_lt, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'A < B'")},
  {"add_le", SatSolver_add_le, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'A <= B'")},
  {"add_gt", SatSolver_add_gt, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'A > B'")},
  {"add_ge", SatSolver_add_ge, METH_VARARGS,
   PyDoc_STR("add clause representing the condition 'A >= B'")},
  {"solve", reinterpret_cast<PyCFunction>(SatSolver_solve),
   METH_VARARGS | METH_KEYWORDS,
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
  PySatSolverType.tp_name = "SatSolver";
  PySatSolverType.tp_basicsize = sizeof(SatSolverObject);
  PySatSolverType.tp_itemsize = 0;
  PySatSolverType.tp_dealloc = SatSolver_dealloc;
  PySatSolverType.tp_flags = Py_TPFLAGS_DEFAULT;
  PySatSolverType.tp_doc = PyDoc_STR("SatSolver object");
  PySatSolverType.tp_methods = SatSolver_methods;
  PySatSolverType.tp_new = SatSolver_new;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "SatSolver", &PySatSolverType) ) {
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
