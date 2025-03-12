#ifndef EXPR2CNF_H
#define EXPR2CNF_H

/// @file Expr2Cnf.h
/// @brief Expr2Cnf のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolver.h"
#include "ym/Expr.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class Expr2Cnf Expr2Cnf.h "Expr2Cnf.h"
/// @brief Expr を CNF に変換するための補助クラス
//////////////////////////////////////////////////////////////////////
class Expr2Cnf
{
public:

  /// @brief コンストラクタ
  Expr2Cnf(
    SatSolver& solver, ///< [in] SATソルバ
    std::unordered_map<SizeType, SatLiteral>& lit_map
                       ///< [in] 変数とリテラルの対応を表す辞書
  ) : mSolver{solver},
      mLitMap{lit_map}
  {
  }

  /// @brief デストラクタ
  ~Expr2Cnf() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief Expr を充足する条件を追加する．
  vector<SatLiteral>
  add_expr(
    const Expr& expr ///< [in] 対象の論理式
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

  // 変数とリテラルの対応を表す辞書
  std::unordered_map<SizeType, SatLiteral>& mLitMap;

};

END_NAMESPACE_YM_SAT

#endif // EXPR2CNF_H
