#ifndef YM_SATINITPARAM_H
#define YM_SATINITPARAM_H

/// @file ym/SatInitParam.h
/// @brief SatInitParam のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include <json/json.h>


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatInitParam SatInitParam.h "ym/SatInitParam.h"
/// @brief SATソルバの種類を表すクラス
///
/// - 単純な場合はタイプを表す文字列のみを引数にとる．
/// - 詳細な設定を行う場合は Json オブジェクトを引数にとる．
/// - 値を表す JSON で決まっているのは "type" という名の属性のみ．
///   値が妥当かどうかのチェックもこのクラス内で行われる．
///   それ以外の属性は各々の実装依存で解釈される．
///   このクラスではチェックを行わない．
///
/// - 空のコンストラクタは以下のアルゴリズムで初期化する．
///  1. 環境変数 YMSAT_CONF が設定されている場合，${YMSAT_CONF} の値
///     を設定用の json ファイルとみなして読み込む．
///     ファイルの読み込みに失敗した場合スキップする．
///  2. 環境変数 YMSAT_CONFDIR が設定されている場合，
///     ${YMSAT_CONFDIR}/ymsat.json を設定用の json ファイルとみなして
///     読み込む．
///     ファイルの読み込みに失敗した場合スキップする．
///  3. カレントディレクトリに ymsat.json ファイルがあれば読み込む
///     ファイルの読み込みに失敗した場合スキップする．
///  4. ハードコードされたデフォルト値を用いる．
//////////////////////////////////////////////////////////////////////
class SatInitParam
{
public:

  /// @brief 空のコンストラクタ
  SatInitParam();

  /// @brief コンストラクタ(タイプを表す文字列)
  SatInitParam(
    const string& type ///< [in] 実装タイプを表す文字列
  );

  /// @brief コンストラクタ(タイプを表す文字列)
  SatInitParam(
    const char* type ///< [in] 実装タイプを表す文字列
  );

  /// @brief コンストラクタ(Jsonオブジェクト)
  SatInitParam(
    const Json::Value& js_obj ///< [in] オプションを表す Json オブジェクト
  );

  /// @brief JSON ファイルからの生成関数
  static
  SatInitParam
  from_json(
    const string& filename ///< [in] ファイル名
  );

  /// @brief デストラクタ
  ~SatInitParam() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 実装タイプを返す．
  string
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

  /// @brief json ファイルを読み込む．
  bool
  read(
    const string& filename ///< [in] ファイル名
  );

  /// @brief type のチェックを行う．
  ///
  /// エラー時には例外(std::invalid_argumnet)が送出される．
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

#endif // YM_SATINITPARAM_H
