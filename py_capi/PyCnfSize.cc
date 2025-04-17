
/// @file PyCnfSize.cc
/// @brief PyCnfSize の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyCnfSize.h"
#include "pym/PyLong.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct CnfSize_Object
{
  PyObject_HEAD
  CnfSize mVal;
};

// Python 用のタイプ定義
PyTypeObject CnfSize_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PyCnfSize::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  Py_TYPE(self)->tp_free(self);
}

PyObject*
nb_add(
  PyObject* self,
  PyObject* other
)
{
  try {
    if ( PyCnfSize::Check(self) ) {
      auto& val1 = PyCnfSize::_get_ref(self);
      if ( PyCnfSize::Check(other) ) {
        auto& val2 = PyCnfSize::_get_ref(other);
        return PyCnfSize::ToPyObject(val1 + val2);
      }
    }
    Py_RETURN_NOTIMPLEMENTED;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
nb_subtract(
  PyObject* self,
  PyObject* other
)
{
  try {
    if ( PyCnfSize::Check(self) ) {
      auto& val1 = PyCnfSize::_get_ref(self);
      if ( PyCnfSize::Check(other) ) {
        auto& val2 = PyCnfSize::_get_ref(other);
        return PyCnfSize::ToPyObject(val1 - val2);
      }
    }
    Py_RETURN_NOTIMPLEMENTED;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
nb_inplace_add(
  PyObject* self,
  PyObject* other
)
{
  try {
    if ( PyCnfSize::Check(self) ) {
      auto& val1 = PyCnfSize::_get_ref(self);
      if ( PyCnfSize::Check(other) ) {
        auto& val2 = PyCnfSize::_get_ref(other);
        val1 += val2;
        Py_XINCREF(self);
        return self;
      }
    }
    Py_RETURN_NOTIMPLEMENTED;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
nb_inplace_subtract(
  PyObject* self,
  PyObject* other
)
{
  try {
    if ( PyCnfSize::Check(self) ) {
      auto& val1 = PyCnfSize::_get_ref(self);
      if ( PyCnfSize::Check(other) ) {
        auto& val2 = PyCnfSize::_get_ref(other);
        val1 -= val2;
        Py_XINCREF(self);
        return self;
      }
    }
    Py_RETURN_NOTIMPLEMENTED;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// Numberオブジェクト構造体
PyNumberMethods number = {
  .nb_add = nb_add,
  .nb_subtract = nb_subtract,
  .nb_inplace_add = nb_inplace_add,
  .nb_inplace_subtract = nb_inplace_subtract
};

// richcompare 関数
PyObject*
richcompare_func(
  PyObject* self,
  PyObject* other,
  int op
)
{
  auto& val = PyCnfSize::_get_ref(self);
  try {
    if ( PyCnfSize::Check(other) ) {
      auto& val2 = PyCnfSize::_get_ref(other);
      if ( op == Py_EQ ) {
        return PyBool_FromLong(val == val2);
      }
      if ( op == Py_NE ) {
        return PyBool_FromLong(val != val2);
      }
    }
    Py_RETURN_NOTIMPLEMENTED;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
get_clause_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyCnfSize::_get_ref(self);
  return PyLong::ToPyObject(val.clause_num);
}

PyObject*
get_literal_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PyCnfSize::_get_ref(self);
  return PyLong::ToPyObject(val.literal_num);
}

// getter/setter定義
PyGetSetDef getsets[] = {
  {"clause_num", get_clause_num, nullptr, PyDoc_STR("number of clauses"), nullptr},
  {"literal_num", get_literal_num, nullptr, PyDoc_STR("number of literals"), nullptr},
  // end-marker
  {nullptr, nullptr, nullptr, nullptr}
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
    nullptr
  };
  // 余分な引数を取らないことを確認しておく．
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "", const_cast<char**>(kwlist)) ) {
    return nullptr;
  }
  try {
    auto self = type->tp_alloc(type, 0);
    return self;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

END_NONAMESPACE


// @brief CnfSize オブジェクトを使用可能にする．
bool
PyCnfSize::init(
  PyObject* m
)
{
  CnfSize_Type.tp_name = "CnfSize";
  CnfSize_Type.tp_basicsize = sizeof(CnfSize_Object);
  CnfSize_Type.tp_itemsize = 0;
  CnfSize_Type.tp_dealloc = dealloc_func;
  CnfSize_Type.tp_as_number = &number;
  CnfSize_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  CnfSize_Type.tp_doc = PyDoc_STR("Python extended object for CnfSize");
  CnfSize_Type.tp_richcompare = richcompare_func;
  CnfSize_Type.tp_getset = getsets;
  CnfSize_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "CnfSize", &CnfSize_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// CnfSize を PyObject に変換する．
PyObject*
PyCnfSize::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PyCnfSize::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<CnfSize_Object*>(obj);
  new (&my_obj->mVal) CnfSize(val);
  return obj;
}

// PyObject を CnfSize に変換する．
bool
PyCnfSize::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PyCnfSize::Check(obj) ) {
    val = PyCnfSize::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が CnfSize タイプか調べる．
bool
PyCnfSize::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &CnfSize_Type);
}

// @brief PyObject から CnfSize を取り出す．
CnfSize&
PyCnfSize::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<CnfSize_Object*>(obj);
  return my_obj->mVal;
}

// @brief CnfSize を表すオブジェクトの型定義を返す．
PyTypeObject*
PyCnfSize::_typeobject()
{
  return &CnfSize_Type;
}

END_NAMESPACE_YM
