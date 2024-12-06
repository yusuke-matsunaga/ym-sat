
/// @file PySatModel.cc
/// @brief Python SatModel の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatModel.h"
#include "pym/PySatBool3.h"
#include "pym/PySatLiteral.h"
#include "pym/PyModule.h"


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
  // 明示的なインスタンス化は禁止
  PyErr_SetString(PyExc_TypeError, "Instantiation of 'SatModel' is disabled");
  return nullptr;
}

// 終了関数
void
SatModel_dealloc(
  PyObject* self
)
{
  auto model_obj = reinterpret_cast<SatModelObject*>(self);
  model_obj->mVal.~SatModel();
  Py_TYPE(self)->tp_free(self);
}

// メソッド定義
PyMethodDef SatModel_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

Py_ssize_t
SatModel_length(
  PyObject* self
)
{
  auto& model = PySatModel::Get(self);
  auto n = model.size();
  return n;
}

PyObject*
SatModel_subscript(
  PyObject* self,
  PyObject* arg
)
{
  auto& model = PySatModel::Get(self);
  if ( !PySatLiteral::Check(arg) ) {
    PyErr_SetString(PyExc_TypeError, "argument 1 should be a SatLiteral");
    return nullptr;
  }
  auto lit = PySatLiteral::Get(arg);
  auto val = model.get(lit);
  return PySatBool3::ToPyObject(val);
}

// マップオブジェクトメソッド定義
PyMappingMethods SatModel_mapping = {
  .mp_length = SatModel_length,
  .mp_subscript = SatModel_subscript
};

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
  SatModelType.tp_doc = PyDoc_STR("SatModel object");
  SatModelType.tp_new = SatModel_new;
  SatModelType.tp_methods = SatModel_methods;
  SatModelType.tp_as_mapping = &SatModel_mapping;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "SatModel", &SatModelType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief SatModel を PyObject に変換する．
PyObject*
PySatModel::ToPyObject(
  const SatModel& val
)
{
  auto obj = SatModelType.tp_alloc(&SatModelType, 0);
  auto model_obj = reinterpret_cast<SatModelObject*>(obj);
  new (&model_obj->mVal) SatModel{val};
  return obj;
}

// @brief PyObject が SatModel タイプか調べる．
bool
PySatModel::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief SatModel を表す PyObject から SatModel を取り出す．
const SatModel&
PySatModel::Get(
  PyObject* obj
)
{
  auto model_obj = reinterpret_cast<SatModelObject*>(obj);
  return model_obj->mVal;
}

// @brief SatModel を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatModel::_typeobject()
{
  return &SatModelType;
}

END_NAMESPACE_YM
