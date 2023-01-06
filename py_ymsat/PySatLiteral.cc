
/// @file PySatLiteral.cc
/// @brief Python SatLiteral の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/PySatLiteral.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct SatLiteralObject
{
  PyObject_HEAD
  SatLiteral mVal;
};

// Python 用のタイプ定義
PyTypeObject SatLiteralType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
SatLiteral_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = type->tp_alloc(type, 0);
  // auto satliteral_obj = reinterpret_cast<SatLiteralObject*>(self);
  // 必要なら satliteral_obj->mVal の初期化を行う．
  return self;
}

// 終了関数
void
SatLiteral_dealloc(
  PyObject* self
)
{
  // auto satliteral_obj = reinterpret_cast<SatLiteralObject*>(self);
  // 必要なら satliteral_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// 適正値の時に true を返す．
PyObject*
SatLiteral_is_valid(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto lit = PySatLiteral::_get(self);
  auto v = lit.is_valid();
  return PyBool_FromLong(v);
}

// 肯定のリテラルの時 true を返す．
PyObject*
SatLiteral_is_positive(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto lit = PySatLiteral::_get(self);
  auto v = lit.is_positive();
  return PyBool_FromLong(v);
}

// 否定のリテラルの時 true を返す．
PyObject*
SatLiteral_is_negative(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto lit = PySatLiteral::_get(self);
  auto v = lit.is_negative();
  return PyBool_FromLong(v);
}

// 肯定のリテラルを返す．
PyObject*
SatLiteral_make_positive(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto lit = PySatLiteral::_get(self);
  auto ans = lit.make_positive();
  return PySatLiteral::ToPyObject(ans);
}

// 否定のリテラルを返す．
PyObject*
SatLiteral_make_negative(
  PyObject* self,
  PyObject* Py_UNUSED(args)
)
{
  auto lit = PySatLiteral::_get(self);
  auto ans = lit.make_negative();
  return PySatLiteral::ToPyObject(ans);
}

// メソッド定義
PyMethodDef SatLiteral_methods[] = {
  {"is_valid", SatLiteral_is_valid, METH_NOARGS,
   PyDoc_STR("return True if valid")},
  {"is_positive", SatLiteral_is_positive, METH_NOARGS,
   PyDoc_STR("return True if the literal is positive")},
  {"is_negative", SatLiteral_is_positive, METH_NOARGS,
   PyDoc_STR("return True if the literal is negative")},
  {"make_positive", SatLiteral_make_positive, METH_NOARGS,
   PyDoc_STR("return the positive literal")},
  {"make_negative", SatLiteral_make_negative, METH_NOARGS,
   PyDoc_STR("return the negative literal")},
  {nullptr, nullptr, 0, nullptr}
};

// 比較関数
PyObject*
SatLiteral_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PySatLiteral::_check(self) &&
       PySatLiteral::_check(other) ) {
    auto val1 = PySatLiteral::_get(self);
    auto val2 = PySatLiteral::_get(other);
    if ( op == Py_EQ ) {
      return PyBool_FromLong(val1 == val2);
    }
    if ( op == Py_NE ) {
      return PyBool_FromLong(val1 != val2);
    }
  }
  Py_INCREF(Py_NotImplemented);
  return Py_NotImplemented;
}

// 否定演算(単項演算の例)
PyObject*
SatLiteral_invert(
  PyObject* self
)
{
  if ( PySatLiteral::_check(self) ) {
    auto val = PySatLiteral::_get(self);
    return PySatLiteral::ToPyObject(~val);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 乗算(二項演算の例)
PyObject*
SatLiteral_mul(
  PyObject* self,
  PyObject* other
)
{
  if ( PySatLiteral::_check(self) && PySatLiteral::_check(other) ) {
    auto val1 = PySatLiteral::_get(self);
    auto val2 = PyObject_IsTrue(other);
    return PySatLiteral::ToPyObject(val1 * val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 数値演算メソッド定義
PyNumberMethods SatLiteral_number = {
  .nb_multiply = SatLiteral_mul,
  .nb_invert = SatLiteral_invert,
  .nb_inplace_multiply = SatLiteral_mul
};

// ハッシュ関数
Py_hash_t
SatLiteral_hash(
  PyObject* self
)
{
  auto lit = PySatLiteral::_get(self);
  return lit.hash();
}

// 変数番号を返す．
PyObject*
SatLiteral_varid(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto lit = PySatLiteral::_get(self);
  int v = lit.varid();
  return PyLong_FromLong(v);
}

// インデックス値を返す．
PyObject*
SatLiteral_index(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto lit = PySatLiteral::_get(self);
  int v = lit.index();
  return PyLong_FromLong(v);
}

// getset メソッド定義
PyGetSetDef SatLiteral_getsetters[] = {
  {"varid", SatLiteral_varid, nullptr, PyDoc_STR("Variable ID")},
  {"index", SatLiteral_index, nullptr, PyDoc_STR("index")},
  {nullptr, nullptr, nullptr, nullptr}
};

END_NONAMESPACE


// @brief 'SatLiteral' オブジェクトを使用可能にする．
bool
PySatLiteral::init(
  PyObject* m
)
{
  SatLiteralType.tp_name = "SatLiteral";
  SatLiteralType.tp_basicsize = sizeof(SatLiteralObject);
  SatLiteralType.tp_itemsize = 0;
  SatLiteralType.tp_dealloc = SatLiteral_dealloc;
  SatLiteralType.tp_flags = Py_TPFLAGS_DEFAULT;
  SatLiteralType.tp_doc = PyDoc_STR("SatLiteral object");
  SatLiteralType.tp_richcompare = SatLiteral_richcmpfunc;
  SatLiteralType.tp_methods = SatLiteral_methods;
  SatLiteralType.tp_getset = SatLiteral_getsetters;
  SatLiteralType.tp_new = SatLiteral_new;
  SatLiteralType.tp_as_number = &SatLiteral_number;
  SatLiteralType.tp_hash = SatLiteral_hash;

  // 型オブジェクトの登録
  if ( !reg_type(m, "SatLiteral", &SatLiteralType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject から SatLiteral を取り出す．
bool
PySatLiteral::FromPyObject(
  PyObject* obj,
  SatLiteral& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a SatLiteral type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief SatLiteral を PyObject に変換する．
PyObject*
PySatLiteral::ToPyObject(
  SatLiteral val
)
{
  auto obj = SatLiteral_new(_typeobject(), nullptr, nullptr);
  _put(obj, val);
  return obj;
}

// @brief PyObject が SatLiteral タイプか調べる．
bool
PySatLiteral::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief SatLiteral を表す PyObject から SatLiteral を取り出す．
SatLiteral
PySatLiteral::_get(
  PyObject* obj
)
{
  auto satliteral_obj = reinterpret_cast<SatLiteralObject*>(obj);
  return satliteral_obj->mVal;
}

// @brief SatLiteral を表す PyObject に値を設定する．
void
PySatLiteral::_put(
  PyObject* obj,
  SatLiteral val
)
{
  auto satliteral_obj = reinterpret_cast<SatLiteralObject*>(obj);
  satliteral_obj->mVal = val;
}

// @brief SatLiteral を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatLiteral::_typeobject()
{
  return &SatLiteralType;
}

END_NAMESPACE_YM
