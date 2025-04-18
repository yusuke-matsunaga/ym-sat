
/// @file PySatBool3.cc
/// @brief PySatBool3 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatBool3.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct SatBool3_Object
{
  PyObject_HEAD
  SatBool3 mVal;
};

// Python 用のタイプ定義
PyTypeObject SatBool3_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PySatBool3::init() 中で初期化する．
};

// 定数を表すオブジェクト
PyObject* Const_false = nullptr;
PyObject* Const_true = nullptr;
PyObject* Const_x = nullptr;

// 定数の登録を行う関数
bool
reg_const_obj(
  const char* name,
  SatBool3 val,
  PyObject*& const_obj
)
{
  auto type = PySatBool3::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<SatBool3_Object*>(obj);
  my_obj->mVal = val;
  if ( PyDict_SetItemString(type->tp_dict, name, obj) < 0 ) {
    return false;
  }
  Py_INCREF(obj);
  const_obj = obj;
  return true;
}

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  Py_TYPE(self)->tp_free(self);
}

// repr 関数
PyObject*
repr_func(
  PyObject* self
)
{
  auto& val = PySatBool3::_get_ref(self);
  try {
    std::string str_val;
    switch ( val ) {
      case SatBool3::False: str_val = "false"; break;
      case SatBool3::True: str_val = "true"; break;
      case SatBool3::X: str_val = "x"; break;
    }
    return PyString::ToPyObject(str_val);
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

int
nb_bool(
  PyObject* self
)
{
  auto& val = PySatBool3::_get_ref(self);
  try {
    return val == SatBool3::True;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return -1;
  }
}

PyObject*
nb_invert(
  PyObject* self
)
{
  auto& val = PySatBool3::_get_ref(self);
  try {
    return PySatBool3::ToPyObject(~val);
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

PyObject*
nb_xor(
  PyObject* self,
  PyObject* other
)
{
  try {
    if ( PySatBool3::Check(self) ) {
      auto& val1 = PySatBool3::_get_ref(self);
      if ( PySatBool3::Check(other) ) {
        auto& val2 = PySatBool3::_get_ref(other);
        return PySatBool3::ToPyObject(val1 ^ val2);
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
  .nb_bool = nb_bool,
  .nb_invert = nb_invert,
  .nb_xor = nb_xor
};

// richcompare 関数
PyObject*
richcompare_func(
  PyObject* self,
  PyObject* other,
  int op
)
{
  auto& val = PySatBool3::_get_ref(self);
  try {
    if ( PySatBool3::Check(self) && PySatBool3::Check(other) ) {
      auto& val1 = PySatBool3::_get_ref(self);
      auto& val2 = PySatBool3::_get_ref(other);
      if ( op == Py_EQ ) {
        return PyBool_FromLong(val1 == val2);
      }
      if ( op == Py_NE ) {
        return PyBool_FromLong(val1 != val2);
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

// new 関数
PyObject*
new_func(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "",
    nullptr
  };
  PyObject* val_obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "O",
                                    const_cast<char**>(kwlist),
                                    &val_obj) ) {
    return nullptr;
  }
  SatBool3 val;
  if ( val_obj != nullptr ) {
    if ( !PySatBool3::FromPyObject(val_obj, val) ) {
      PyErr_SetString(PyExc_ValueError, "could not convert to SatBool3");
      return nullptr;
    }
  }
  try {
    return PySatBool3::ToPyObject(val);
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

END_NONAMESPACE


// @brief SatBool3 オブジェクトを使用可能にする．
bool
PySatBool3::init(
  PyObject* m
)
{
  SatBool3_Type.tp_name = "SatBool3";
  SatBool3_Type.tp_basicsize = sizeof(SatBool3_Object);
  SatBool3_Type.tp_itemsize = 0;
  SatBool3_Type.tp_dealloc = dealloc_func;
  SatBool3_Type.tp_repr = repr_func;
  SatBool3_Type.tp_as_number = &number;
  SatBool3_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  SatBool3_Type.tp_doc = PyDoc_STR("Python extended object for SatBool3");
  SatBool3_Type.tp_richcompare = richcompare_func;
  SatBool3_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "SatBool3", &SatBool3_Type) ) {
    goto error;
  }
  // 定数オブジェクトの生成・登録
  if ( !reg_const_obj("false", SatBool3::False, Const_false) ) {
    goto error;
  }
  if ( !reg_const_obj("true", SatBool3::True, Const_true) ) {
    goto error;
  }
  if ( !reg_const_obj("x", SatBool3::X, Const_x) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// SatBool3 を PyObject に変換する．
PyObject*
PySatBool3::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
    case SatBool3::False: obj = Const_false; break;
    case SatBool3::True: obj = Const_true; break;
    case SatBool3::X: obj = Const_x; break;
  }
  if ( obj == nullptr ) {
    PyErr_SetString(PyExc_ValueError, "invalid value for SatBool3");
    return nullptr;
  }
  Py_INCREF(obj);
  return obj;
}

// PyObject を SatBool3 に変換する．
bool
PySatBool3::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  std::string str_val;
  if ( PyString::FromPyObject(obj, str_val) ) {
    if ( strcasecmp(str_val.c_str(), "false") == 0 ) {
      val = SatBool3::False;
      return true;
    }
    else if ( strcasecmp(str_val.c_str(), "true") == 0 ) {
      val = SatBool3::True;
      return true;
    }
    else if ( strcasecmp(str_val.c_str(), "x") == 0 ) {
      val = SatBool3::X;
      return true;
    }
    return false;
  }
  if ( PySatBool3::Check(obj) ) {
    val = PySatBool3::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が SatBool3 タイプか調べる．
bool
PySatBool3::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &SatBool3_Type);
}

// @brief PyObject から SatBool3 を取り出す．
SatBool3&
PySatBool3::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<SatBool3_Object*>(obj);
  return my_obj->mVal;
}

// @brief SatBool3 を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatBool3::_typeobject()
{
  return &SatBool3_Type;
}

END_NAMESPACE_YM
