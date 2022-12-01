
/// @file PySatLiteral.cc
/// @brief Python SatLiteral の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PySatLiteral.h"


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

// 初期化関数(__init__()相当)
int
SatLiteral_init(
  PyObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  return 0;
}

// repr() 関数
PyObject*
SatLiteral_repr(
  PyObject* self
)
{
  auto val = PySatLiteral::_get(self);
  // val から 文字列を作る．
  const char* tmp_str = nullptr;
  return Py_BuildValue("s", tmp_str);
}

// メソッド定義
PyMethodDef SatLiteral_methods[] = {
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
    auto val2 = PySatLiteral::_get(other);
    return PySatLiteral::ToPyObject(val1 * val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 数値演算メソッド定義
PyNumberMethods SatLiteral_number = {
  .nb_invert = SatLiteral_invert,
  .nb_mul = SatLiteral_mul,
  .nb_inplace_mul = SatLiteral_mul
};

// マップオブジェクトのサイズ
Py_ssize_t
SatLiteral_Size(
  PyObject* self
)
{
  auto val = PySatLiteral::_get(self);
  return val.size();
}

// マップオブジェクトの要素取得関数
PyObject*
SatLiteral_GetItem(
  PyObject* self,
  PyObject* key
)
{
  return nullptr;
}

// マップオブジェクトの要素設定関数
int
SatLiteral_SetItem(
  PyObject* self,
  PyObject* key,
  PyObject* v
)
{
  return -1;
}

// マップオブジェクト構造体
PyMappingMethods SatLiteral_mapping = {
  .mp_length = SatLiteral%_Size,
  .mp_subscript = SatLiteral_GetItem,
  .mp_ass_subscript = SatLiteral_SetItem
};

// ハッシュ関数
Py_hash_t
SatLiteral_hash(
  PyObject* self
)
{
  auto val = PySatLiteral::_get(self);
  return val.hash();
}

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
  SatLiteralType.tp_doc = PyDoc_STR("SatLiteral objects");
  SatLiteralType.tp_richcompare = SatLiteral_richcmpfunc;
  SatLiteralType.tp_methods = SatLiteral_methods;
  SatLiteralType.tp_init = SatLiteral_init;
  SatLiteralType.tp_new = SatLiteral_new;
  SatLiteralType.tp_repr = SatLiteral_repr;
  SatLiteralType.tp_as_number = SatLiteral_number;
  SatLiteralType.tp_as_mapping = SatLiteral_mapping;
  SatLiteralType.tp_hash = SatLiteral_hash;
  if ( PyType_Ready(&SatLiteralType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  auto type_obj = reinterpret_cast<PyObject*>(&SatLiteralType);
  Py_INCREF(type_obj);
  if ( PyModule_AddObject(m, "SatLiteral", type_obj) < 0 ) {
    Py_DECREF(type_obj);
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
