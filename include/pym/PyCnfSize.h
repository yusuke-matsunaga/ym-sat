#ifndef PYCNFSIZE_H
#define PYCNFSIZE_H

/// @file PyCnfSize.h
/// @brief PyCnfSize のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "ym/CnfSize.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @class PyCnfSizeConv PyCnfSize.h "PyCnfSize.h"
/// @brief CnfSize を PyObject* に変換するファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyCnfSizeConv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief CnfSize を PyObject* に変換する．
  PyObject*
  operator()(
    const CnfSize& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyCnfSizeDeconv PyCnfSize.h "PyCnfSize.h"
/// @brief CnfSize を取り出すファンクタクラス
///
/// 実はただの関数
//////////////////////////////////////////////////////////////////////
class PyCnfSizeDeconv
{
public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief PyObject* から CnfSize を取り出す．
  bool
  operator()(
    PyObject* obj,
    CnfSize& val
  );

};


//////////////////////////////////////////////////////////////////////
/// @class PyCnfSize PyCnfSize.h "PyCnfSize.h"
/// @brief Python 用の CnfSize 拡張
///
/// 複数の関数をひとまとめにしているだけなので実は名前空間として用いている．
//////////////////////////////////////////////////////////////////////
class PyCnfSize
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

  /// @brief CnfSize を表す PyObject を作る．
  /// @return 生成した PyObject を返す．
  ///
  /// 返り値は新しい参照が返される．
  static
  PyObject*
  ToPyObject(
    const CnfSize& val ///< [in] 値
  )
  {
    PyCnfSizeConv conv;
    return conv(val);
  }

  /// @brief PyObject が CnfSize タイプか調べる．
  static
  bool
  _check(
    PyObject* obj ///< [in] 対象の PyObject
  );

  /// @brief CnfSize を表す PyObject から CnfSize を取り出す．
  /// @return CnfSize を返す．
  ///
  /// Check(obj) == true であると仮定している．
  static
  CnfSize&
  _get_ref(
    PyObject* obj ///< [in] 変換元の PyObject
  );

  /// @brief CnfSize を表すオブジェクトの型定義を返す．
  static
  PyTypeObject*
  _typeobject();

};

END_NAMESPACE_YM

#endif // PYCNFSIZE_H
