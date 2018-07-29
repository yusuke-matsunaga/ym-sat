#ifndef YM_SATSOLVERTYPE_H
#define YM_SATSOLVERTYPE_H

/// @file ym/SatSolverType.h
/// @brief SatSolverType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018 Yusuke Matsunaga
/// All rights reserved.

#include "ym/ym_sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatSolverType SatSolverType.h "SatSolverType.h"
/// @brief SATソルバの種類を表すクラス
//////////////////////////////////////////////////////////////////////
class SatSolverType
{
public:

  /// @brief コンストラクタ
  /// @param[in] type 実装タイプを表す文字列
  /// @param[in] option オプション文字列
  /// @param[in] log_out ログを記録するストリームへのポインタ
  SatSolverType(const string& type = string(),
		const string& option = string(),
		ostream* log_out = nullptr);

  /// @brief デストラクタ
  ~SatSolverType();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をセットする．
  /// @param[in] type 実装タイプを表す文字列
  /// @param[in] option オプション文字列
  /// @param[in] log_out ログを記録するストリームへのポインタ
  void
  set(const string& type,
      const string& option = string(),
      ostream* log_out = nullptr);

  /// @brief 実装タイプを返す．
  const string&
  type() const;

  /// @brief オプションを返す．
  const string&
  option() const;

  /// @brief ログ出力のストリームを返す．
  ostream*
  log_out() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 実装タイプ
  string mType;

  // オプション
  string mOption;

  // ログ出力先
  ostream* mLogOut;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] type 実装タイプを表す文字列
// @param[in] option オプション文字列
// @param[in] log_out ログを記録するストリームへのポインタ
inline
SatSolverType::SatSolverType(const string& type,
			     const string& option,
			     ostream* log_out) :
  mType(type),
  mOption(option),
  mLogOut(log_out)
{
}

// @brief デストラクタ
inline
SatSolverType::~SatSolverType()
{
}

// @brief 内容をセットする．
// @param[in] type 実装タイプを表す文字列
// @param[in] option オプション文字列
// @param[in] log_out ログを記録するストリームへのポインタ
inline
void
SatSolverType::set(const string& type,
		   const string& option,
		   ostream* log_out)
{
  mType = type;
  mOption = option;
  mLogOut = log_out;
}

// @brief 実装タイプを返す．
inline
const string&
SatSolverType::type() const
{
  return mType;
}

// @brief オプションを返す．
inline
const string&
SatSolverType::option() const
{
  return mOption;
}

// @brief ログ出力のストリームを返す．
inline
ostream*
SatSolverType::log_out() const
{
  return mLogOut;
}

END_NAMESPACE_YM_SAT

#endif // YM_SATSOLVERTYPE_H
