
/// @file PySatLiteral.cc
/// @brief PySatLiteral の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatLiteral.h"
#include "pym/PyLong.h"
#include "pym/PyBool.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct SatLiteral_Object
{
  PyObject_HEAD
  SatLiteral mVal;
};

// Python 用のタイプ定義
PyTypeObject SatLiteral_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PySatLiteral::init() 中で初期化する．
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
nb_multiply(
  PyObject* self,
  PyObject* other
)
{
  try {
    if ( PySatLiteral::Check(self) ) {
      auto& val1 = PySatLiteral::_get_ref(self);
      if ( PyBool::Check(other) ) {
        auto val2 = PyBool::Get(other);
        return PySatLiteral::ToPyObject(val1 * val2);
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
nb_invert(
  PyObject* self
)
{
  auto& val = PySatLiteral::_get_ref(self);
  try {
    return PySatLiteral::ToPyObject(~val);
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
nb_inplace_multiply(
  PyObject* self,
  PyObject* other
)
{
  try {
    if ( PySatLiteral::Check(self) ) {
      auto& val1 = PySatLiteral::_get_ref(self);
      if ( PyBool::Check(other) ) {
        auto val2 = PyBool::Get(other);
        val1 = val1 * val2;
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
  .nb_multiply = nb_multiply,
  .nb_invert = nb_invert,
  .nb_inplace_multiply = nb_inplace_multiply
};

// hash 関数
Py_hash_t
hash_func(
  PyObject* self
)
{
  auto& val = PySatLiteral::_get_ref(self);
  try {
    return val.hash();
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return 0;
  }
}

// str 関数
PyObject*
str_func(
  PyObject* self
)
{
  auto& val = PySatLiteral::_get_ref(self);
  try {
    std::ostringstream buf;
    buf << val;
    return PyString::ToPyObject(buf.str());
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// richcompare 関数
PyObject*
richcompare_func(
  PyObject* self,
  PyObject* other,
  int op
)
{
  auto& val = PySatLiteral::_get_ref(self);
  try {
    if ( PySatLiteral::Check(other) ) {
      auto& val2 = PySatLiteral::_get_ref(other);
      Py_RETURN_RICHCOMPARE(val, val2, op);
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

// make a copy
PyObject*
copy(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatLiteral::_get_ref(self);
  auto type = &SatLiteral_Type;
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<SatLiteral_Object*>(self);
  new (&my_obj->mVal) SatLiteral(val);
  return obj;
}

// True if valid
PyObject*
is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatLiteral::_get_ref(self);
  return PyBool_FromLong(val.is_valid());
}

// True if positive literal
PyObject*
is_positive(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatLiteral::_get_ref(self);
  return PyBool_FromLong(val.is_positive());
}

// True if negative literal
PyObject*
is_negative(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatLiteral::_get_ref(self);
  return PyBool_FromLong(val.is_negative());
}

// return the positive literal of the same variable
PyObject*
make_positive(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatLiteral::_get_ref(self);
  return PySatLiteral::ToPyObject(val.make_positive());
}

// return the negative literal of the same variable
PyObject*
make_negative(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto& val = PySatLiteral::_get_ref(self);
  return PySatLiteral::ToPyObject(val.make_negative());
}

// メソッド定義
PyMethodDef methods[] = {
  {"copy",
   copy,
   METH_NOARGS,
   PyDoc_STR("make a copy")},
  {"is_valid",
   is_valid,
   METH_NOARGS,
   PyDoc_STR("True if valid")},
  {"is_positive",
   is_positive,
   METH_NOARGS,
   PyDoc_STR("True if positive literal")},
  {"is_negative",
   is_negative,
   METH_NOARGS,
   PyDoc_STR("True if negative literal")},
  {"make_positive",
   make_positive,
   METH_NOARGS,
   PyDoc_STR("return the positive literal of the same variable")},
  {"make_negative",
   make_negative,
   METH_NOARGS,
   PyDoc_STR("return the negative literal of the same variable")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
get_varid(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PySatLiteral::_get_ref(self);
  return PyLong::ToPyObject(val.varid());
}

PyObject*
get_index(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& val = PySatLiteral::_get_ref(self);
  return PyLong::ToPyObject(val.index());
}

// getter/setter定義
PyGetSetDef getsets[] = {
  {"varid", get_varid, nullptr, PyDoc_STR("variable ID"), nullptr},
  {"index", get_index, nullptr, PyDoc_STR("unique index"), nullptr},
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
  PyErr_SetString(PyExc_TypeError, "Instantiation of 'SatLiteral' is disabled");
  return nullptr;
}

END_NONAMESPACE


// @brief SatLiteral オブジェクトを使用可能にする．
bool
PySatLiteral::init(
  PyObject* m
)
{
  SatLiteral_Type.tp_name = "SatLiteral";
  SatLiteral_Type.tp_basicsize = sizeof(SatLiteral_Object);
  SatLiteral_Type.tp_itemsize = 0;
  SatLiteral_Type.tp_dealloc = dealloc_func;
  SatLiteral_Type.tp_as_number = &number;
  SatLiteral_Type.tp_hash = hash_func;
  SatLiteral_Type.tp_str = str_func;
  SatLiteral_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  SatLiteral_Type.tp_doc = PyDoc_STR("Python extended object for SatLiteral");
  SatLiteral_Type.tp_richcompare = richcompare_func;
  SatLiteral_Type.tp_methods = methods;
  SatLiteral_Type.tp_getset = getsets;
  SatLiteral_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "SatLiteral", &SatLiteral_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// SatLiteral を PyObject に変換する．
PyObject*
PySatLiteral::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PySatLiteral::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<SatLiteral_Object*>(obj);
  new (&my_obj->mVal) SatLiteral(val);
  return obj;
}

// PyObject を SatLiteral に変換する．
bool
PySatLiteral::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PySatLiteral::Check(obj) ) {
    val = PySatLiteral::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が SatLiteral タイプか調べる．
bool
PySatLiteral::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &SatLiteral_Type);
}

// @brief PyObject から SatLiteral を取り出す．
SatLiteral&
PySatLiteral::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<SatLiteral_Object*>(obj);
  return my_obj->mVal;
}

// @brief SatLiteral を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatLiteral::_typeobject()
{
  return &SatLiteral_Type;
}

END_NAMESPACE_YM
