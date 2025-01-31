
/// @file PyCnfSize.cc
/// @brief Python CnfSize の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PyCnfSize.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
struct CnfSizeObject
{
  PyObject_HEAD
  CnfSize mVal;
};

// Python 用のタイプ定義
PyTypeObject CnfSizeType = {
  PyVarObject_HEAD_INIT(nullptr, 0)
};

// 生成関数
PyObject*
CnfSize_new(
  PyTypeObject* type,
  PyObject* args,
  PyObject* kwds
)
{
  // 引数を取らないことを確認する．
  static const char* kw_list[] = {
    nullptr,
  };

  if ( PyArg_ParseTupleAndKeywords(args, kwds, "",
				   const_cast<char**>(kw_list)) ) {
    return nullptr;
  }

  auto self = type->tp_alloc(type, 0);
  auto cnfsize_obj = reinterpret_cast<CnfSizeObject*>(self);
  new (&cnfsize_obj->mVal) CnfSize{};
  return self;
}

// 終了関数
void
CnfSize_dealloc(
  PyObject* self
)
{
  Py_TYPE(self)->tp_free(self);
}

// メソッド定義
PyMethodDef CnfSize_methods[] = {
  {nullptr, nullptr, 0, nullptr}
};

PyObject*
CnfSize_clause_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& size = PyCnfSize::_Get(self);
  return Py_BuildValue("k", size.clause_num);
}

PyObject*
CnfSize_literal_num(
  PyObject* self,
  void* Py_UNUSED(closure)
)
{
  auto& size = PyCnfSize::_Get(self);
  return Py_BuildValue("k", size.literal_num);
}

// get/set 関数定義
PyGetSetDef CnfSize_getset[] = {
  {"clause_num", CnfSize_clause_num, nullptr, PyDoc_STR("clause num"), nullptr},
  {"literal_num", CnfSize_literal_num, nullptr, PyDoc_STR("literal num"), nullptr},
  {nullptr, nullptr, nullptr, nullptr, nullptr},
};

// 比較関数
PyObject*
CnfSize_richcmpfunc(
  PyObject* self,
  PyObject* other,
  int op
)
{
  if ( PyCnfSize::Check(self) &&
       PyCnfSize::Check(other) ) {
    auto& val1 = PyCnfSize::_Get(self);
    auto& val2 = PyCnfSize::_Get(other);
    if ( op == Py_EQ ) {
      return PyBool_FromLong(val1 == val2);
    }
    if ( op == Py_NE ) {
      return PyBool_FromLong(val1 != val2);
    }
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 加算
PyObject*
CnfSize_add(
  PyObject* self,
  PyObject* other
)
{
  if ( PyCnfSize::Check(self) && PyCnfSize::Check(other) ) {
    auto& val1 = PyCnfSize::_Get(self);
    auto& val2 = PyCnfSize::_Get(other);
    return PyCnfSize::ToPyObject(val1 + val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 減算
PyObject*
CnfSize_sub(
  PyObject* self,
  PyObject* other
)
{
  if ( PyCnfSize::Check(self) && PyCnfSize::Check(other) ) {
    auto& val1 = PyCnfSize::_Get(self);
    auto& val2 = PyCnfSize::_Get(other);
    return PyCnfSize::ToPyObject(val1 - val2);
  }
  Py_RETURN_NOTIMPLEMENTED;
}

// 数値演算メソッド定義
PyNumberMethods CnfSize_number = {
  .nb_add = CnfSize_add,
  //.nb_sub = CnfSize_sub,
  .nb_inplace_add = CnfSize_add,
  //.nb_inplace_sub = CnfSize_sub
};

END_NONAMESPACE


// @brief 'CnfSize' オブジェクトを使用可能にする．
bool
PyCnfSize::init(
  PyObject* m
)
{
  CnfSizeType.tp_name = "CnfSize";
  CnfSizeType.tp_basicsize = sizeof(CnfSizeObject);
  CnfSizeType.tp_itemsize = 0;
  CnfSizeType.tp_dealloc = CnfSize_dealloc;
  CnfSizeType.tp_flags = Py_TPFLAGS_DEFAULT;
  CnfSizeType.tp_doc = PyDoc_STR("CnfSize object");
  CnfSizeType.tp_richcompare = CnfSize_richcmpfunc;
  CnfSizeType.tp_methods = CnfSize_methods;
  CnfSizeType.tp_getset = CnfSize_getset;
  CnfSizeType.tp_new = CnfSize_new;
  CnfSizeType.tp_as_number = &CnfSize_number;

  // 型オブジェクトの登録
  if ( !PyModule::reg_type(m, "CnfSize", &CnfSizeType) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief CnfSize を PyObject に変換する．
PyObject*
PyCnfSize::ToPyObject(
  const CnfSize& val
)
{
  auto obj = CnfSizeType.tp_alloc(&CnfSizeType, 0);
  auto cnfsize_obj = reinterpret_cast<CnfSizeObject*>(obj);
  cnfsize_obj->mVal = val;
  return obj;
}

// @brief PyObject が CnfSize タイプか調べる．
bool
PyCnfSize::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, _typeobject());
}

// @brief CnfSize を表す PyObject から CnfSize を取り出す．
CnfSize
PyCnfSize::Get(
  PyObject* obj
)
{
  auto cnfsize_obj = reinterpret_cast<CnfSizeObject*>(obj);
  return cnfsize_obj->mVal;
}

// @brief CnfSize を表す PyObject から CnfSize を取り出す．
const CnfSize&
PyCnfSize::_Get(
  PyObject* obj
)
{
  auto cnfsize_obj = reinterpret_cast<CnfSizeObject*>(obj);
  return cnfsize_obj->mVal;
}

// @brief CnfSize を表すオブジェクトの型定義を返す．
PyTypeObject*
PyCnfSize::_typeobject()
{
  return &CnfSizeType;
}

END_NAMESPACE_YM
