
/// @file PySatSolver.cc
/// @brief Python SatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#include "PySatSolver.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct SatSolverObject
{
  PyObject_HEAD
  SatSolver mVal;
};

// Python 用のタイプ定義
PyTypeObject SatSolverType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
SatSolver_new(
  PyTypeObject* type,
  PyObject* Py_UNUSED(args),
  PyObject* Py_UNUSED(kwds)
)
{
  auto self = type->tp_alloc(type, 0);
  // auto satsolver_obj = reinterpret_cast<SatSolverObject*>(self);
  // 必要なら satsolver_obj->mVal の初期化を行う．
  return self;
}

// 終了関数
void
SatSolver_dealloc(
  PyObject* self
)
{
  // auto satsolver_obj = reinterpret_cast<SatSolverObject*>(self);
  // 必要なら satsolver_obj->mVal の終了処理を行う．
  Py_TYPE(self)->tp_free(self);
}

// 初期化関数(__init__()相当)
int
SatSolver_init(
  PyObject* self,
  PyObject* args,
  PyObject* Py_UNUSED(kwds)
)
{
  return 0;
}

// repr() 関数
PyObject*
SatSolver_repr(
  PyObject* self
)
{
  auto val = PySatSolver::_get(self);
  // val から 文字列を作る．
  const char* tmp_str = nullptr;
  return Py_BuildValue("s", tmp_str);
}

// メソッド定義
PyMethodDef SatSolver_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

// 比較関数
PyObject*
SatSolver_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PySatSolver::_check(self) &&
       PySatSolver::_check(other) ) {
    auto val1 = PySatSolver::_get(self);
    auto val2 = PySatSolver::_get(other);
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
SatSolver_invert(
  PyObject* self
)
{
  if ( PySatSolver::_check(self) ) {
    auto val = PySatSolver::_get(self);
    return PySatSolver::ToPyObject(~val);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 乗算(二項演算の例)
PyObject*
SatSolver_mul(
  PyObject* self,
  PyObject* other
)
{
  if ( PySatSolver::_check(self) && PySatSolver::_check(other) ) {
    auto val1 = PySatSolver::_get(self);
    auto val2 = PySatSolver::_get(other);
    return PySatSolver::ToPyObject(val1 * val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 数値演算メソッド定義
PyNumberMethods SatSolver_number = {
  .nb_invert = SatSolver_invert,
  .nb_mul = SatSolver_mul,
  .nb_inplace_mul = SatSolver_mul
};

// マップオブジェクトのサイズ
Py_ssize_t
SatSolver_Size(
  PyObject* self
)
{
  auto val = PySatSolver::_get(self);
  return val.size();
}

// マップオブジェクトの要素取得関数
PyObject*
SatSolver_GetItem(
  PyObject* self,
  PyObject* key
)
{
  return nullptr;
}

// マップオブジェクトの要素設定関数
int
SatSolver_SetItem(
  PyObject* self,
  PyObject* key,
  PyObject* v
)
{
  return -1;
}

// マップオブジェクト構造体
PyMappingMethods SatSolver_mapping = {
  .mp_length = SatSolver%_Size,
  .mp_subscript = SatSolver_GetItem,
  .mp_ass_subscript = SatSolver_SetItem
};

// ハッシュ関数
Py_hash_t
SatSolver_hash(
  PyObject* self
)
{
  auto val = PySatSolver::_get(self);
  return val.hash();
}

END_NONAMESPACE


// @brief 'SatSolver' オブジェクトを使用可能にする．
bool
PySatSolver::init(
  PyObject* m
)
{
  SatSolverType.tp_name = "SatSolver";
  SatSolverType.tp_basicsize = sizeof(SatSolverObject);
  SatSolverType.tp_itemsize = 0;
  SatSolverType.tp_dealloc = SatSolver_dealloc;
  SatSolverType.tp_flags = Py_TPFLAGS_DEFAULT;
  SatSolverType.tp_doc = PyDoc_STR("SatSolver objects");
  SatSolverType.tp_richcompare = SatSolver_richcmpfunc;
  SatSolverType.tp_methods = SatSolver_methods;
  SatSolverType.tp_init = SatSolver_init;
  SatSolverType.tp_new = SatSolver_new;
  SatSolverType.tp_repr = SatSolver_repr;
  SatSolverType.tp_as_number = SatSolver_number;
  SatSolverType.tp_as_mapping = SatSolver_mapping;
  SatSolverType.tp_hash = SatSolver_hash;
  if ( PyType_Ready(&SatSolverType) < 0 ) {
    return false;
  }

  // 型オブジェクトの登録
  auto type_obj = reinterpret_cast<PyObject*>(&SatSolverType);
  Py_INCREF(type_obj);
  if ( PyModule_AddObject(m, "SatSolver", type_obj) < 0 ) {
    Py_DECREF(type_obj);
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject から SatSolver を取り出す．
bool
PySatSolver::FromPyObject(
  PyObject* obj,
  SatSolver& val
)
{
  if ( !_check(obj) ) {
    PyErr_SetString(PyExc_TypeError, "object is not a SatSolver type");
    return false;
  }
  val = _get(obj);
  return true;
}

// @brief SatSolver を PyObject に変換する．
PyObject*
PySatSolver::ToPyObject(
  SatSolver val
)
{
  auto obj = SatSolver_new(_typeobject(), nullptr, nullptr);
  _put(obj, val);
  return obj;
}

// @brief PyObject が SatSolver タイプか調べる．
bool
PySatSolver::_check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief SatSolver を表す PyObject から SatSolver を取り出す．
SatSolver
PySatSolver::_get(
  PyObject* obj
)
{
  auto satsolver_obj = reinterpret_cast<SatSolverObject*>(obj);
  return satsolver_obj->mVal;
}

// @brief SatSolver を表す PyObject に値を設定する．
void
PySatSolver::_put(
  PyObject* obj,
  SatSolver val
)
{
  auto satsolver_obj = reinterpret_cast<SatSolverObject*>(obj);
  satsolver_obj->mVal = val;
}

// @brief SatSolver を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatSolver::_typeobject()
{
  return &SatSolverType;
}

END_NAMESPACE_YM
