
/// @file PySatInitParam.cc
/// @brief PySatInitParam の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "pym/PySatInitParam.h"
#include "pym/PyJsonValue.h"
#include "pym/PyString.h"
#include "pym/PyModule.h"


BEGIN_NAMESPACE_YM

BEGIN_NONAMESPACE

// Python 用のオブジェクト定義
// この構造体は同じサイズのヒープから作られるので
// mVal のコンストラクタは起動されないことに注意．
// そのためあとでコンストラクタを明示的に起動する必要がある．
// またメモリを開放するときにも明示的にデストラクタを起動する必要がある．
struct SatInitParam_Object
{
  PyObject_HEAD
  SatInitParam mVal;
};

// Python 用のタイプ定義
PyTypeObject SatInitParam_Type = {
  PyVarObject_HEAD_INIT(nullptr, 0)
  // 残りは PySatInitParam::init() 中で初期化する．
};

// 終了関数
void
dealloc_func(
  PyObject* self
)
{
  auto obj = reinterpret_cast<SatInitParam_Object*>(self);
  obj->mVal.~SatInitParam();
  Py_TYPE(self)->tp_free(self);
}

// read JSON file and return new SatInitParam object
PyObject*
from_json(
  PyObject* Py_UNUSED(self),
  PyObject* args,
  PyObject* kwds
)
{
  static const char* kwlist[] = {
    "filename",
    nullptr
  };
  const char* filename_tmp = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "s",
                                    const_cast<char**>(kwlist),
                                    &filename_tmp) ) {
    return nullptr;
  }
  std::string filename;
  if ( filename_tmp != nullptr ) {
    filename = std::string(filename_tmp);
  }
  auto val = SatInitParam::from_json(filename);
  auto type = &SatInitParam_Type;
  auto self = type->tp_alloc(type, 0);
  auto my_obj = reinterpret_cast<SatInitParam_Object*>(self);
  new (&my_obj->mVal) SatInitParam(val);
  return self;
}

// メソッド定義
PyMethodDef methods[] = {
  {"from_json",
   reinterpret_cast<PyCFunction>(from_json),
   METH_VARARGS | METH_KEYWORDS | METH_STATIC,
   PyDoc_STR("read JSON file and return new SatInitParam object")},
  // end-marker
  {nullptr, nullptr, 0, nullptr}
};

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
  PyObject* obj = nullptr;
  if ( !PyArg_ParseTupleAndKeywords(args, kwds, "|O",
                                    const_cast<char**>(kwlist),
                                    &obj) ) {
    return nullptr;
  }
  try {
    if ( obj == nullptr ) {
      auto self = type->tp_alloc(type, 0);
      auto my_obj = reinterpret_cast<SatInitParam_Object*>(self);
      new (&my_obj->mVal) SatInitParam();
      return self;
    }
    else {
      std::string str_val;
      if ( PyString::FromPyObject(obj, str_val) ) {
        auto self = type->tp_alloc(type, 0);
        auto my_obj = reinterpret_cast<SatInitParam_Object*>(self);
        new (&my_obj->mVal) SatInitParam(str_val);
        return self;
      }
      JsonValue js_val;
      if ( PyJsonValue::FromPyObject(obj, js_val) ) {
        auto self = type->tp_alloc(type, 0);
        auto my_obj = reinterpret_cast<SatInitParam_Object*>(self);
        new (&my_obj->mVal) SatInitParam(js_val);
        return self;
      }
      PyErr_SetString(PyExc_TypeError, "invalid parameter type for 'SatInitParam'");
      return nullptr;
    }
  }
  catch ( std::invalid_argument err ) {
    std::ostringstream buf;
    buf << "invalid argument" << ": " << err.what();
    PyErr_SetString(PyExc_ValueError, buf.str().c_str());
    return nullptr;
  }
}

END_NONAMESPACE


// @brief SatInitParam オブジェクトを使用可能にする．
bool
PySatInitParam::init(
  PyObject* m
)
{
  SatInitParam_Type.tp_name = "SatInitParam";
  SatInitParam_Type.tp_basicsize = sizeof(SatInitParam_Object);
  SatInitParam_Type.tp_itemsize = 0;
  SatInitParam_Type.tp_dealloc = dealloc_func;
  SatInitParam_Type.tp_flags = Py_TPFLAGS_DEFAULT;
  SatInitParam_Type.tp_doc = PyDoc_STR("Python extended object for SatInitParam");
  SatInitParam_Type.tp_methods = methods;
  SatInitParam_Type.tp_new = new_func;
  if ( !PyModule::reg_type(m, "SatInitParam", &SatInitParam_Type) ) {
    goto error;
  }

  return true;

 error:

  return false;
}

// @brief PyObject が SatInitParam タイプか調べる．
bool
PySatInitParam::Check(
  PyObject* obj
)
{
  return Py_IS_TYPE(obj, &SatInitParam_Type);
}

// @brief PyObject から SatInitParam を取り出す．
SatInitParam&
PySatInitParam::_get_ref(
  PyObject* obj
)
{
  auto my_obj = reinterpret_cast<SatInitParam_Object*>(obj);
  return my_obj->mVal;
}

// @brief SatInitParam を表すオブジェクトの型定義を返す．
PyTypeObject*
PySatInitParam::_typeobject()
{
  return &SatInitParam_Type;
}

END_NAMESPACE_YM
