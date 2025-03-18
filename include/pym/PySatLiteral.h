#ifndef PYSATLITERAL_H
#define PYSATLITERAL_H

/// @file PySatLiteral.h
/// @brief PySatLiteral のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class PySatLiteralConv PySatLiteral.h "PySatLiteral.h"
/// @brief SatLiteral を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PySatLiteralConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief SatLiteral を PyObject* に変換する．
  PyObject*
  operator()(
    const SatLiteral& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PySatLiteralDeconv PySatLiteral.h "PySatLiteral.h"
/// @brief SatLiteral を取り出すファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PySatLiteralDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* から SatLiteral を取り出す．
  bool
  operator()(
    PyObject* obj,
    SatLiteral& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PySatLiteral PySatLiteral.h "PySatLiteral.h"
/// @brief Python 用の SatLiteral 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PySatLiteral
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

  /// @brief SatLiteral を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const SatLiteral& val ///< [in] 値
  )
  {
    PySatLiteralConv conv;
    return conv(val);
  }

  /// @brief PyObject が SatLiteral タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief SatLiteral を表す PyObject から SatLiteral を取り出す．
  /// @return SatLiteral を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  SatLiteral&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief SatLiteral を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_YM

#endif // PYSATLITERAL_H
