#ifndef YM_SATSOLVERTYPE_H
#define YM_SATSOLVERTYPE_H

/// @file ym/SatSolverType.h
/// @brief SatSolverType のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatSolverType SatSolverType.h "ym/SatSolverType.h"
/// @brief SATソルバの種類を表すクラス
//////////////////////////////////////////////////////////////////////
class SatSolverType
{
public:

  /// @brief コンストラクタ
  SatSolverType(
    const string& type = string(),   ///< [in] 実装タイプを表す文字列
    const string& option = string(), ///< [in] オプション文字列
    ostream* log_out = nullptr       ///< [in] ログを記録するストリームへのポインタ
  ) : mType{type},
      mOption{option},
      mLogOut{log_out}
  {
  }

  /// @brief デストラクタ
  ~SatSolverType() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容をセットする．
  void
  set(
    const string& type,               ///< [in] 実装タイプを表す文字列
    const string& option = string(),  ///< [in] オプション文字列
    ostream* log_out = nullptr	      ///< [in] ログを記録するストリームへのポインタ
  )
  {
    mType = type;
    mOption = option;
    mLogOut = log_out;
  }

  /// @brief 実装タイプを返す．
  const string&
  type() const
  {
    return mType;
  }

  /// @brief オプションを返す．
  const string&
  option() const
  {
    return mOption;
  }

  /// @brief ログ出力のストリームを返す．
  ostream*
  log_out() const
  {
    return mLogOut;
  }


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

END_NAMESPACE_YM_SAT

#endif // YM_SATSOLVERTYPE_H
