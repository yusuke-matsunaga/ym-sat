#ifndef PYSATBOOL3_H
#define PYSATBOOL3_H

/// @file PySatBool3.h
/// @brief PySatBool3 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class PySatBool3 PySatBool3.h "PySatBool3.h"
/// @brief SatBool3 を Python から使用するための拡張
///
/// 実際には static メンバ関数しか持たないのでクラスではない．
//////////////////////////////////////////////////////////////////////
class PySatBool3
{
public:

  using ElemType = SatBool3;

public:

  /// @brief SatBool3 を PyObject* に変換するファンクタクラス
  struct Conv {
    PyObject*
    operator()(
      const ElemType& val ///< [in] 元の値
    );
  };

  /// @brief PyObject* から SatBool3 を取り出すファンクタクラス
  struct Deconv {
    bool
    operator()(
      PyObject* obj, ///< [in] Python のオブジェクト
      ElemType& val  ///< [out] 結果を格納する変数
    );
  };


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 初期化する．
  /// @return 初期化が成功したら true を返す．
  static
  bool
  init(
    PyObject* m ///< [in] 親のモジュールを表す PyObject
  );

  /// @brief SatBool3 を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const ElemType& val ///< [in] 元の値
  )
  {
    Conv conv;
    return conv(val);
  }

  /// @brief PyObject から SatBool3 を取り出す．
  /// @return 正しく変換できた時に true を返す．
  static
  bool
  FromPyObject(
    PyObject* obj, ///< [in] Python のオブジェクト
    ElemType& val  ///< [out] 結果を格納する変数
  )
  {
    Deconv deconv;
    return deconv(obj, val);
  }

  /// @brief PyObject が SatBool3 タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief PyObject から SatBool3 を取り出す．
  static
  ElemType
  Get(
    PyObject* obj ///< [in] 対象の Python オブジェクト
  )
  {
    ElemType val;
    if ( PySatBool3::FromPyObject(obj, val) ) {
      return val;
    }
    PyErr_SetString(PyExc_TypeError, "Could not convert to SatBool3");
    return val;
  }

  /// @brief SatBool3 を表す PyObject から SatBool3 を取り出す．
  /// @return SatBool3 を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  ElemType&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief SatBool3 を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_YM

#endif // PYSATBOOL3_H
