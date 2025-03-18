
/// @file PySatBool3.cc
/// @brief Python SatBool3 の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatBool3.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct SatBool3Object
{
  PyObject_HEAD
  SatBool3 mVal;
};

// Python 用のタイプ定義
PyTypeObject SatBool3Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 各定数を表す PyObject
PyObject* SatBool3_True = nullptr;
PyObject* SatBool3_False = nullptr;
PyObject* SatBool3_X = nullptr;

// 生成関数
PyObject*
SatBool3_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  if ( type != &SatBool3Type ) {
    PyErr_SetString(PyExc_TypeError, "SatBool3 cannot be overloaded");
    return nullptr;
  }
  static const char* kwlist[] = {
    "name",
    nullptr
  };
  const char* val_str = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s",
				    const_cast<char**>(kwlist),
				    &val_str) ) {
    return nullptr;
  }
  SatBool3 val;
  if ( strcasecmp(val_str, "true") == 0 ) {
    val = SatBool3::True;
  }
  else if ( strcasecmp(val_str, "false") == 0 ) {
    val = SatBool3::False;
  }
  else if ( strcasecmp(val_str, "x") == 0 ) {
    val = SatBool3::X;
  }
  else {
    // エラー
    ostringstream buf;
    buf << "argument 1 must be one of "
	<< "\"true\""
	<< ", \"false\""
	<< ", or \"x\"";
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
  return PySatBool3::ToPyObject(val);
}

// 終了関数
void
SatBool3_dealloc(
  PyObject* self
)
{
  // auto satbool3_obj = reinterpret_cast<SatBool3Object*>(self);
  // 必要なら satbool3_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// repr() 関数
PyObject*
SatBool3_repr(
  PyObject* self
)
{
  auto val = PySatBool3::_get_ref(self);
  // val から 文字列を作る．
  const char* tmp_str = nullptr;
  switch ( val ) {
  case SatBool3::True:  tmp_str = "true"; break;
  case SatBool3::False: tmp_str = "false"; break;
  case SatBool3::X:     tmp_str = "x"; break;
  }
  return Py_BuildValue("s", tmp_str);
}

// メソッド定義
PyMethodDef SatBool3_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// 比較関数
PyObject*
SatBool3_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PySatBool3::_check(self) &&
       PySatBool3::_check(other) ) {
    auto val1 = PySatBool3::_get_ref(self);
    auto val2 = PySatBool3::_get_ref(other);
    if ( op == Py_EQ ) {
      return PyBool_FromLong(val1 == val2);
    }
    if ( op == Py_NE ) {
      return PyBool_FromLong(val1 != val2);
    }
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 否定演算
PyObject*
SatBool3_invert(
  PyObject* self
)
{
  if ( PySatBool3::_check(self) ) {
    auto val = PySatBool3::_get_ref(self);
    return PySatBool3::ToPyObject(~val);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// XOR演算
PyObject*
SatBool3_xor(
  PyObject* self,
  PyObject* other
)
{
  if ( PySatBool3::_check(self) &&
       PySatBool3::_check(other) ) {
    auto val1 = PySatBool3::_get_ref(self);
    auto val2 = PySatBool3::_get_ref(other);
    return PySatBool3::ToPyObject(val1 ^ val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// bool 値への変換
int
SatBool3_bool(
  PyObject* self
)
{
  auto val = PySatBool3::_get_ref(self);
  return val == SatBool3::True;
}

// 数値演算メソッド定義
PyNumberMethods SatBool3_number = {
  .nb_bool = SatBool3_bool,
  .nb_invert = SatBool3_invert,
  .nb_xor = SatBool3_xor,
  .nb_inplace_xor = SatBool3_xor
};

// 定数オブジェクトの構築
PyObject*
new_const(
  SatBool3 val
)
{
  auto obj = SatBool3Type.tp_alloc(&SatBool3Type, 0);
  auto satbool3_obj = reinterpret_cast<SatBool3Object*>(obj);
  satbool3_obj->mVal = val;
  return obj;
}

// 定数オブジェクトの登録
bool
reg_obj(
  const char* name,
  PyObject* obj
)
{
  Py_INCREF(obj);
  if ( PyDict_SetItemString(SatBool3Type.tp_dict, name, obj) < 0 ) {
    return false;
  }
  return true;
}

END_NONAMESPACE


// @brief 'SatBool3' オブジェクトを使用可能にする．
bool
PySatBool3::init(
  PyObject* m
)
{
  SatBool3Type.tp_name = "SatBool3";
  SatBool3Type.tp_basicsize = sizeof(SatBool3Object);
  SatBool3Type.tp_itemsize = 0;
  SatBool3Type.tp_dealloc = SatBool3_dealloc;
  SatBool3Type.tp_flags = Py_TPFLAGS_DEFAULT;
  SatBool3Type.tp_doc = PyDoc_STR("SatBool3 object");
  SatBool3Type.tp_richcompare = SatBool3_richcmpfunc;
  SatBool3Type.tp_methods = SatBool3_methods;
  SatBool3Type.tp_new = SatBool3_new;
  SatBool3Type.tp_repr = SatBool3_repr;
  SatBool3Type.tp_as_number = &SatBool3_number;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "SatBool3", &SatBool3Type) ) {
    goto error;
  }

  // 定数オブジェクトの生成
  SatBool3_True = new_const(SatBool3::True);
  SatBool3_False = new_const(SatBool3::False);
  SatBool3_X = new_const(SatBool3::X);

  // 定数オブジェクトの登録
  if ( !reg_obj("true", SatBool3_True) ) {
    goto error;
  }
  if ( !reg_obj("false", SatBool3_False) ) {
    goto error;
  }
  if ( !reg_obj("x", SatBool3_X) ) {
    goto error;
  }

  return true;

 error:

  Py_XDECREF(SatBool3_True);
  Py_XDECREF(SatBool3_False);
  Py_XDECREF(SatBool3_X);

  return false;
}

// @brief SatBool3 を PyObject に変換する．
PyObject*
PySatBool3Conv::operator()(
  const SatBool3& val
)
{
  PyObject* obj = nullptr;
  switch ( val ) {
  case SatBool3::True:  obj = SatBool3_True; break;
  case SatBool3::False: obj = SatBool3_False; break;
  case SatBool3::X:     obj = SatBool3_X; break;
  }
  Py_INCREF(obj);
  return obj;
}

// @brief PyObject* から SatBool3 を取り出す．
bool
PySatBool3Deconv::operator()(
  PyObject* obj,
  SatBool3& val
)
{
  if ( PySatBool3::_check(obj) ) {
    val = PySatBool3::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が SatBool3 タイプか調べる．
bool
PySatBool3::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief SatBool3 を表す PyObject から SatBool3 を取り出す．
SatBool3&
PySatBool3::_get_ref(
  PyObject* obj
)
{
  auto satbool3_obj = reinterpret_cast<SatBool3Object*>(obj);
  return satbool3_obj->mVal;
}

// @brief SatBool3 を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatBool3::_typeobject()
{
  return &SatBool3Type;
}

END_NAMESPACE_YM
