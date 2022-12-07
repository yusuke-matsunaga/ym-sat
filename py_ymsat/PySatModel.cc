
/// @file PySatModel.cc
/// @brief Python SatModel の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PySatModel.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct SatModelObject
{
  PyObject_HEAD
  SatModel mVal;
};

// Python 用のタイプ定義
PyTypeObject SatModelType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
SatModel_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = type->tp_alloc(type, 0);
  // auto satmodel_obj = reinterpret_cast<SatModelObject*>(self);
  // 必要なら satmodel_obj->mVal の初期化を行う．
  return self;
}

// 終了関数
void
SatModel_dealloc(
  PyObject* self
)
{
  // auto satmodel_obj = reinterpret_cast<SatModelObject*>(self);
  // 必要なら satmodel_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// 初期化関数(__init__()相当)
int
SatModel_init(
  PyObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  return 0;
}

// repr() 関数
PyObject*
SatModel_repr(
  PyObject* self
)
{
  auto val = PySatModel::_get(self);
  // val から 文字列を作る．
  const char* tmp_str = nullptr;
  return Py_BuildValue("s", tmp_str);
}

// メソッド定義
PyMethodDef SatModel_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// 比較関数
PyObject*
SatModel_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PySatModel::_check(self) &&
       PySatModel::_check(other) ) {
    auto val1 = PySatModel::_get(self);
    auto val2 = PySatModel::_get(other);
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
SatModel_invert(
  PyObject* self
)
{
  if ( PySatModel::_check(self) ) {
    auto val = PySatModel::_get(self);
    return PySatModel::ToPyObject(~val);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 乗算(二項演算の例)
PyObject*
SatModel_mul(
  PyObject* self,
  PyObject* other
)
{
  if ( PySatModel::_check(self) && PySatModel::_check(other) ) {
    auto val1 = PySatModel::_get(self);
    auto val2 = PySatModel::_get(other);
    return PySatModel::ToPyObject(val1 * val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 数値演算メソッド定義
PyNumberMethods SatModel_number = {
  .nb_invert = SatModel_invert,
  .nb_mul = SatModel_mul,
  .nb_inplace_mul = SatModel_mul
};

// マップオブジェクトのサイズ
Py_ssize_t
SatModel_Size(
  PyObject* self
)
{
  auto val = PySatModel::_get(self);
  return val.size();
}

// マップオブジェクトの要素取得関数
PyObject*
SatModel_GetItem(
  PyObject* self,
  PyObject* key
)
{
  return nullptr;
}

// マップオブジェクトの要素設定関数
int
SatModel_SetItem(
  PyObject* self,
  PyObject* key,
  PyObject* v
)
{
  return -1;
}

// マップオブジェクト構造体
PyMappingMethods SatModel_mapping = {
  .mp_length = SatModel%_Size,
  .mp_subscript = SatModel_GetItem,
  .mp_ass_subscript = SatModel_SetItem
};

// ハッシュ関数
Py_hash_t
SatModel_hash(
  PyObject* self
)
{
  auto val = PySatModel::_get(self);
  return val.hash();
}

END_NONAMESPACE


// @brief 'SatModel' オブジェクトを使用可能にする．
bool
PySatModel::init(
  PyObject* m
)
{
  SatModelType.tp_name = "SatModel";
  SatModelType.tp_basicsize = sizeof(SatModelObject);
  SatModelType.tp_itemsize = 0;
  SatModelType.tp_dealloc = SatModel_dealloc;
  SatModelType.tp_flags = Py_TPFLAGS_DEFAULT;
  SatModelType.tp_doc = PyDoc_STR("SatModel objects");
  SatModelType.tp_richcompare = SatModel_richcmpfunc;
  SatModelType.tp_methods = SatModel_methods;
  SatModelType.tp_init = SatModel_init;
  SatModelType.tp_new = SatModel_new;
  SatModelType.tp_repr = SatModel_repr;
  SatModelType.tp_as_number = SatModel_number;
  SatModelType.tp_as_mapping = SatModel_mapping;
  SatModelType.tp_hash = SatModel_hash;
  if ( PyType_Ready(&SatModelType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  auto type_obj = reinterpret_cast<PyObject*>(&SatModelType);
  Py_INCREF(type_obj);
  if ( PyModule_AddObject(m, "SatModel", type_obj) < 0 ) {
    Py_DECREF(type_obj);
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject から SatModel を取り出す．
bool
PySatModel::FromPyObject(
  PyObject* obj,
  SatModel& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a SatModel type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief SatModel を PyObject に変換する．
PyObject*
PySatModel::ToPyObject(
  SatModel val
)
{
  auto obj = SatModel_new(_typeobject(), nullptr, nullptr);
  _put(obj, val);
  return obj;
}

// @brief PyObject が SatModel タイプか調べる．
bool
PySatModel::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief SatModel を表す PyObject から SatModel を取り出す．
SatModel
PySatModel::_get(
  PyObject* obj
)
{
  auto satmodel_obj = reinterpret_cast<SatModelObject*>(obj);
  return satmodel_obj->mVal;
}

// @brief SatModel を表す PyObject に値を設定する．
void
PySatModel::_put(
  PyObject* obj,
  SatModel val
)
{
  auto satmodel_obj = reinterpret_cast<SatModelObject*>(obj);
  satmodel_obj->mVal = val;
}

// @brief SatModel を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatModel::_typeobject()
{
  return &SatModelType;
}

END_NAMESPACE_YM
