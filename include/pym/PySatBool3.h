#ifndef PYSATBOOL3_H
#define PYSATBOOL3_H

/// @file PySatBool3.h
/// @brief PySatBool3 のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/SatBool3.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class PySatBool3 PySatBool3.h "PySatBool3.h"
/// @brief Python 用の SatBool3 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PySatBool3
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

  /// @brief SatBool3 を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    SatBool3 val ///< [in] 値
  );

  /// @brief PyObject が SatBool3 タイプか調べる．
  static
  bool
  Check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief SatBool3 を表す PyObject から SatBool3 を取り出す．
  /// @return SatBool3 を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  SatBool3
  Get(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief SatBool3 を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_YM

#endif // PYSATBOOL3_H
