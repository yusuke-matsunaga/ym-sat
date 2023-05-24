#ifndef YM_SATSOLVERTYPE_H
#define YM_SATSOLVERTYPE_H

/// @file ym/SatSolverType.h
/// @brief SatSolverType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include <json/json.h>


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatSolverType SatSolverType.h "ym/SatSolverType.h"
/// @brief SATソルバの種類を表すクラス
///
/// 単純な場合はタイプを表す文字列のみを引数にとる．
/// 詳細な設定を行う場合は Json オブジェクトを引数にとる．
//////////////////////////////////////////////////////////////////////
class SatSolverType
{
public:

  /// @brief コンストラクタ(タイプを表す文字列)
  SatSolverType(
    const string& type = string{} ///< [in] 実装タイプを表す文字列
  );

  /// @brief コンストラクタ(Jsonオブジェクト)
  SatSolverType(
    const Json::Value& js_obj ///< [in] オプションを表す Json オブジェクト
  );

  /// @brief デストラクタ
  ~SatSolverType() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 実装タイプを返す．
  const string&
  type() const;

  /// @brief オプションを表す Json オブジェクトを返す．
  const Json::Value&
  js_obj() const
  {
    return mJsObj;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief type のチェックを行う．
  ///
  /// エラー時には例外が創出される．
  void
  check_type();


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 本体
  Json::Value mJsObj;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATSOLVERTYPE_H
