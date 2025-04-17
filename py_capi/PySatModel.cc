
/// @file PySatModel.cc
/// @brief PySatModel の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatModel.h"
#include "pym/PySatBool3.h"
#include "pym/PySatLiteral.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct SatModel_Object
{
  PyObject_HEAD
  SatModel mVal;
};

// Python 用のタイプ定義
PyTypeObject SatModel_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PySatModel::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<SatModel_Object*>(self);
  obj->mVal.~SatModel();
  Py_TYPE(self)->tp_free(self);
}

Py_ssize_t
mp_length(
  PyObject* self
)
{
  auto& val = PySatModel::_get_ref(self);
  try {
    auto len_val = val.size();
    return len_val;
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return -1;
  }
}

PyObject*
mp_subscript(
  PyObject* self,
  PyObject* key
)
{
  auto& val = PySatModel::_get_ref(self);
  try {
    SatLiteral lit;
    if ( !PySatLiteral::FromPyObject(key, lit) ) {
      PyErr_SetString(PyExc_TypeError, "argument 1 should be a 'SatLiteral'");
      return nullptr;
    }
    return PySatBool3::ToPyObject(val.get(lit));
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

// Mapping オブジェクト構造体
PyMappingMethods mapping = {
  .mp_length = mp_length,
  .mp_subscript = mp_subscript
};

// new 関数
PyObject*
new_func(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  PyErr_SetString(PyExc_TypeError, "instantiation of 'SatModel' is disabled");
  return nullptr;
}

END_NONAMESPACE


// @brief SatModel オブジェクトを使用可能にする．
bool
PySatModel::init(
  PyObject* m
)
{
  SatModel_Type.tp_name = "SatModel";
  SatModel_Type.tp_basicsize = sizeof(SatModel_Object);
  SatModel_Type.tp_itemsize = 0;
  SatModel_Type.tp_dealloc = dealloc_func;
  SatModel_Type.tp_as_mapping = &mapping;
  SatModel_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  SatModel_Type.tp_doc = PyDoc_STR("Python extended object for SatModel");
  SatModel_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "SatModel", &SatModel_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// SatModel を PyObject に変換する．
PyObject*
PySatModel::Conv::operator()(
  const ElemType& val ///< [in] 元の値
)
{
  auto type = PySatModel::_typeobject();
  auto obj = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<SatModel_Object*>(obj);
  new (&my_obj->mVal) SatModel(val);
  return obj;
}

// PyObject を SatModel に変換する．
bool
PySatModel::Deconv::operator()(
  PyObject* obj, ///< [in] Python のオブジェクト
  ElemType& val  ///< [out] 結果を格納する変数
)
{
  if ( PySatModel::Check(obj) ) {
    val = PySatModel::_get_ref(obj);
    return true;
  }
  return false;
}

// @brief PyObject が SatModel タイプか調べる．
bool
PySatModel::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &SatModel_Type);
}

// @brief PyObject から SatModel を取り出す．
SatModel&
PySatModel::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<SatModel_Object*>(obj);
  return my_obj->mVal;
}

// @brief SatModel を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatModel::_typeobject()
{
  return &SatModel_Type;
}

END_NAMESPACE_YM
