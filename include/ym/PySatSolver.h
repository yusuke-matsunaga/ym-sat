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

#include "ym/PyBase.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class PySatSolver PySatSolver.h "PySatSolver.h"
/// @brief Python 用の SatSolver 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PySatSolver :
  public PyBase
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
  SatSolver&
  _get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief SatSolver を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_YM

#endif // PYSATSOLVER_H
