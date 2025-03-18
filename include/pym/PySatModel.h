#ifndef PYSATMODEL_H
#define PYSATMODEL_H

/// @file PySatModel.h
/// @brief PySatModel のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/SatModel.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class PySatModelConv PySatModel.h "PySatModel.h"
/// @brief SatModel を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PySatModelConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief SatModel を PyObject* に変換する．
  PyObject*
  operator()(
    const SatModel& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PySatModelDeconv PySatModel.h "PySatModel.h"
/// @brief SatModel を取り出すファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PySatModelDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* から SatModel を取り出す．
  bool
  operator()(
    PyObject* obj,
    SatModel& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PySatModel PySatModel.h "PySatModel.h"
/// @brief Python 用の SatModel 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PySatModel
{
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

  /// @brief SatModel を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const SatModel& val ///< [in] 値
  )
  {
    PySatModelConv conv;
    return conv(val);
  }

  /// @brief PyObject が SatModel タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief SatModel を表す PyObject から SatModel を取り出す．
  /// @return SatModel を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  SatModel&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief SatModel を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_YM

#endif // PYSATMODEL_H
