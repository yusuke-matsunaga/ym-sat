#ifndef PYSATSOLVER_H
#define PYSATSOLVER_H

/// @file PySatSolver.h
/// @brief PySatSolver のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class PySatSolver PySatSolver.h "PySatSolver.h"
/// @brief Python 用の SatSolver 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PySatSolver
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

  /// @brief SatSolver を表す PyObject から SatSolver を取り出す．
  /// @return 変換が成功したら true を返す．
  ///
  /// エラーの場合にはPython例外がセットされる．
  static
  bool
  FromPyObject(
    PyObject* obj,  ///< [in] SatSolver を表す PyObject
    SatSolver& val ///< [out] 変換された SatSolver を格納する変数
  );

  /// @brief SatSolver を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    SatSolver val ///< [in] 値
  );

  /// @brief PyObject が SatSolver タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief SatSolver を表す PyObject から SatSolver を取り出す．
  /// @return SatSolver を返す．
  ///
  /// _check(obj) == true であると仮定している．
  static
  SatSolver
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief SatSolver を表す PyObject に値を設定する．
  ///
  /// _check(obj) == true であると仮定している．
  static
  void
  _put(
    PyObject* obj, ///< [in] 対象の PyObject
    SatSolver val ///< [in] 設定する値
  );

  /// @brief SatSolver を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_YM

#endif // PYSATSOLVER_H
