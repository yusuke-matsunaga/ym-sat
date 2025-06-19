#ifndef AIG2CNF_H
#define AIG2CNF_H

/// @file Aig2Cnf.h
/// @brief Aig2Cnf のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2025 Yusuke Matsunaga
/// All rights reserved.

#include "ym/SatSolver.h"
#include "ym/AigHandle.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class Aig2Cnf Aig2Cnf.h "Aig2Cnf.h"
/// @brief AIG を CNF に変換する補助クラス
//////////////////////////////////////////////////////////////////////
class Aig2Cnf
{
public:

  /// @brief AIG の入力番号とリテラルの対応関係を表す辞書の型
  using LitMap = SatSolver::LitMap;

public:

  /// @brief コンストラクタ
  Aig2Cnf(
    SatSolver& solver,    ///< [in] SATソルバ
    const LitMap& lit_map ///< [in] AIG の入力番号とリテラルの対応関係を表す辞書
  ) : mSolver{solver},
      mLitMap{lit_map}
  {
  }

  /// @brief デストラクタ
  ~Aig2Cnf() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief AIG を CNF に変換する．
  /// @return リテラルを返す．
  ///
  /// - 与えられた AIG の値が 1 となる条件を表すリテラルを返す．
  std::vector<SatLiteral>
  make_cnf(
    const AigHandle& aig
  );


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  SatLiteral
  _cnf_sub(
    const AigHandle& aig
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

  // 入力番号とリテラルの対応関係を表す辞書
  LitMap mLitMap;

  // AigHandle をキーにして対応する SatLiteral を記憶する辞書
  std::unordered_map<AigHandle, SatLiteral> mAigDict;

};

END_NAMESPACE_YM_SAT

#endif // AIG2CNF_H
