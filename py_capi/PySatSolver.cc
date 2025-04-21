
/// @file PySatSolver.cc
/// @brief PySatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatSolver.h"
#include "pym/PySatInitParam.h"
#include "pym/PySatBool3.h"
#include "pym/PySatLiteral.h"
#include "pym/PySatModel.h"
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
#include "parse_lits.cc"

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
  PyObject* args
)
{
  std::vector<SatLiteral> lits_list;
  if ( !parse_lits(args, lits_list) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  val.set_conditional_literals(lits_list);
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
  PyObject* args
)
{
  std::vector<SatLiteral> lits_list;
  if ( !parse_lits(args, lits_list) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_clause(lits_list);
  Py_RETURN_NONE;
}

// add clauses representing 'BUF' gate
PyObject*
add_buffgate(
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

// add clauses representing 'NOT' gate
PyObject*
add_notgate(
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
  val.add_notgate(lit1, lit2);
  Py_RETURN_NONE;
}

// add clauses representing 'AND' gate
PyObject*
add_andgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  std::vector<SatLiteral> lits_list;
  if ( !parse_lit_lits(args, olit, lits_list) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_andgate(olit, lits_list);
  Py_RETURN_NONE;
}

// add clauses representing 'NAND' gate
PyObject*
add_nandgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  std::vector<SatLiteral> lits_list;
  if ( !parse_lit_lits(args, olit, lits_list) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_nandgate(olit, lits_list);
  Py_RETURN_NONE;
}

// add clauses representing 'OR' gate
PyObject*
add_orgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  std::vector<SatLiteral> lits_list;
  if ( !parse_lit_lits(args, olit, lits_list) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_orgate(olit, lits_list);
  Py_RETURN_NONE;
}

// add clauses representing 'NOR' gate
PyObject*
add_norgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  std::vector<SatLiteral> lits_list;
  if ( !parse_lit_lits(args, olit, lits_list) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_norgate(olit, lits_list);
  Py_RETURN_NONE;
}

// add clauses representing 'XOR' gate
PyObject*
add_xorgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  std::vector<SatLiteral> lits_list;
  if ( !parse_lit_lits(args, olit, lits_list) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_xorgate(olit, lits_list);
  Py_RETURN_NONE;
}

// add clauses representing 'XNOR' gate
PyObject*
add_xnorgate(
  PyObject* self,
  PyObject* args
)
{
  SatLiteral olit;
  std::vector<SatLiteral> lits_list;
  if ( !parse_lit_lits(args, olit, lits_list) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_xnorgate(olit, lits_list);
  Py_RETURN_NONE;
}

// add clauses representing 'HALF ADDER'
PyObject*
add_half_adder(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "a",
    "b",
    "s",
    "c",
    nullptr
  };
  PyObject* alit_obj = nullptr;
  PyObject* blit_obj = nullptr;
  PyObject* slit_obj = nullptr;
  PyObject* clit_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!O!O!",
                                    const_cast<char**>(kwlist),
                                    PySatLiteral::_typeobject(), &alit_obj,
                                    PySatLiteral::_typeobject(), &blit_obj,
                                    PySatLiteral::_typeobject(), &slit_obj,
                                    PySatLiteral::_typeobject(), &clit_obj) ) {
    return nullptr;
  }
  SatLiteral alit;
  if ( alit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(alit_obj, alit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral blit;
  if ( blit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(blit_obj, blit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral slit;
  if ( slit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(slit_obj, slit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral clit;
  if ( clit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(clit_obj, clit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_half_adder(alit, blit, slit, clit);
  Py_RETURN_NONE;
}

// add clauses representing 'FULL ADDER'
PyObject*
add_full_adder(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "a",
    "b",
    "i",
    "s",
    "c",
    nullptr
  };
  PyObject* alit_obj = nullptr;
  PyObject* blit_obj = nullptr;
  PyObject* ilit_obj = nullptr;
  PyObject* slit_obj = nullptr;
  PyObject* clit_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!O!O!O!O!",
                                    const_cast<char**>(kwlist),
                                    PySatLiteral::_typeobject(), &alit_obj,
                                    PySatLiteral::_typeobject(), &blit_obj,
                                    PySatLiteral::_typeobject(), &ilit_obj,
                                    PySatLiteral::_typeobject(), &slit_obj,
                                    PySatLiteral::_typeobject(), &clit_obj) ) {
    return nullptr;
  }
  SatLiteral alit;
  if ( alit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(alit_obj, alit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral blit;
  if ( blit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(blit_obj, blit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral ilit;
  if ( ilit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(ilit_obj, ilit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral slit;
  if ( slit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(slit_obj, slit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral clit;
  if ( clit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(clit_obj, clit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  auto& val = PySatSolver::_get_ref(self);
  val.add_full_adder(alit, blit, ilit, slit, clit);
  Py_RETURN_NONE;
}

// add clauses representing 'ADDER'
PyObject*
add_adder(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "a",
    "b",
    "i",
    "s",
    "c",
    nullptr
  };
  PyObject* alits_obj = nullptr;
  PyObject* blits_obj = nullptr;
  PyObject* ilit_obj = nullptr;
  PyObject* slits_obj = nullptr;
  PyObject* clit_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "OOO!OO!",
                                    const_cast<char**>(kwlist),
                                    &alits_obj,
                                    &blits_obj,
                                    PySatLiteral::_typeobject(), &ilit_obj,
                                    &slits_obj,
                                    PySatLiteral::_typeobject(), &clit_obj) ) {
    return nullptr;
  }
  SatLiteral ilit;
  if ( ilit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(ilit_obj, ilit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  SatLiteral clit;
  if ( clit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(clit_obj, clit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  auto& val = PySatSolver::_get_ref(self);
  std::vector<SatLiteral> alits;
  if ( !PyList<SatLiteral, PySatLiteral>::FromPyObject(alits_obj, alits) ) {
    PyErr_SetString(PyExc_TypeError, "argument 1 should be a sequence of 'SatLiteral'");
    return nullptr;
  }
  std::vector<SatLiteral> blits;
  if ( !PyList<SatLiteral, PySatLiteral>::FromPyObject(blits_obj, blits) ) {
    PyErr_SetString(PyExc_TypeError, "argument 2 should be a sequence of 'SatLiteral'");
    return nullptr;
  }
  std::vector<SatLiteral> slits;
  if ( !PyList<SatLiteral, PySatLiteral>::FromPyObject(slits_obj, slits) ) {
    PyErr_SetString(PyExc_TypeError, "argument 4 should be a sequence of 'SatLiteral'");
    return nullptr;
  }
  val.add_adder(alits, blits, ilit, slits, clit);
  Py_RETURN_NONE;
}

// solve SAT problem
PyObject*
solve(
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
                                    &obj1,
                                    &time_limit) ) {
    return nullptr;
  }
  auto& val = PySatSolver::_get_ref(self);
  std::vector<SatLiteral> assumptions;
  if ( obj1 != nullptr ) {
    if ( PySatLiteral::Check(obj1) ) {
      auto& lit = PySatLiteral::_get_ref(obj1);
      assumptions.push_back(lit);
    }
    else if ( PyList<SatLiteral, PySatLiteral>::Check(obj1) ) {
      assumptions = PyList<SatLiteral, PySatLiteral>::Get(obj1);
    }
    else {
      PyErr_SetString(PyExc_TypeError, "'SatLiteral' or sequence of 'SatLiteral' is expected");
      return nullptr;
    }
  }
  auto ans = val.solve(assumptions, time_limit);
  return PySatBool3::ToPyObject(ans);
}

// return the model for SAT problem
PyObject*
model(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatSolver::_get_ref(self);
  auto& model = val.model();
  return PySatModel::ToPyObject(model);
}

// read model value for specified literal
PyObject*
read_model(
  PyObject* self,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "lit",
    nullptr
  };
  PyObject* lit_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O!",
                                    const_cast<char**>(kwlist),
                                    PySatLiteral::_typeobject(), &lit_obj) ) {
    return nullptr;
  }
  SatLiteral lit;
  if ( lit_obj != nullptr ) {
    if ( !PySatLiteral::FromPyObject(lit_obj, lit) ) {
      PyErr_SetString(PyExc_TypeError, "could not convert to SatLiteral");
      return nullptr;
    }
  }
  auto& val = PySatSolver::_get_ref(self);
  auto& model = val.model();
  auto ans = model[lit];
  return PySatBool3::ToPyObject(ans);
}

// return conflict literals of the last SAT problem
PyObject*
conflict_literals(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatSolver::_get_ref(self);
  auto& lits = val.conflict_literals();
  return PyList<SatLiteral, PySatLiteral>::ToPyObject(lits);
}

// メソッド定義
PyMethodDef methods[] = {
  {"new_variable",
   reinterpret_cast<PyCFunction>(new_variable),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("make new variable")},
  {"set_conditional_literals",
   set_conditional_literals,
   METH_VARARGS,
   PyDoc_STR("set 'conditional literals'")},
  {"clear_conditional_literals",
   clear_conditional_literals,
   METH_NOARGS,
   PyDoc_STR("clear 'conditional literals'")},
  {"add_clause",
   add_clause,
   METH_VARARGS,
   PyDoc_STR("add clause")},
  {"add_buffgate",
   reinterpret_cast<PyCFunction>(add_buffgate),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add clauses representing 'BUF' gate")},
  {"add_notgate",
   reinterpret_cast<PyCFunction>(add_notgate),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add clauses representing 'NOT' gate")},
  {"add_andgate",
   add_andgate,
   METH_VARARGS,
   PyDoc_STR("add clauses representing 'AND' gate")},
  {"add_nandgate",
   add_nandgate,
   METH_VARARGS,
   PyDoc_STR("add clauses representing 'NAND' gate")},
  {"add_orgate",
   add_orgate,
   METH_VARARGS,
   PyDoc_STR("add clauses representing 'OR' gate")},
  {"add_norgate",
   add_norgate,
   METH_VARARGS,
   PyDoc_STR("add clauses representing 'NOR' gate")},
  {"add_xorgate",
   add_xorgate,
   METH_VARARGS,
   PyDoc_STR("add clauses representing 'XOR' gate")},
  {"add_xnorgate",
   add_xnorgate,
   METH_VARARGS,
   PyDoc_STR("add clauses representing 'XNOR' gate")},
  {"add_half_adder",
   reinterpret_cast<PyCFunction>(add_half_adder),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add clauses representing 'HALF ADDER'")},
  {"add_full_adder",
   reinterpret_cast<PyCFunction>(add_full_adder),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add clauses representing 'FULL ADDER'")},
  {"add_adder",
   reinterpret_cast<PyCFunction>(add_adder),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("add clauses representing 'ADDER'")},
  {"solve",
   reinterpret_cast<PyCFunction>(solve),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("solve SAT problem")},
  {"model",
   model,
   METH_NOARGS,
   PyDoc_STR("return the model for SAT problem")},
  {"read_model",
   reinterpret_cast<PyCFunction>(read_model),
   METH_VARARGS | METH_KEYWORDS,
   PyDoc_STR("read model value for specified literal")},
  {"conflict_literals",
   conflict_literals,
   METH_NOARGS,
   PyDoc_STR("return conflict literals of the last SAT problem")},
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
  static const char* kwlist[] = {
    "init_param",
    nullptr
  };
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O!",
                                    const_cast<char**>(kwlist),
                                    PySatInitParam::_typeobject(), &obj) ) {
    return nullptr;
  }
  try {
    if ( obj == nullptr ) {
      auto self = type->tp_alloc(type, 0);
      auto my_obj = reinterpret_cast<SatSolver_Object*>(self);
      new (&my_obj->mVal) SatSolver();
      return self;
    }
    else {
      auto& init_param = PySatInitParam::_get_ref(obj);
      auto self = type->tp_alloc(type, 0);
      auto my_obj = reinterpret_cast<SatSolver_Object*>(self);
      new (&my_obj->mVal) SatSolver(init_param);
      return self;
    }
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
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
