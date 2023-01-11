
/// @file SatTestFixture.h
/// @brief SatTestFixture の定義ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "gtest/gtest.h"
#include "ym/SatSolver.h"


BEGIN_NAMESPACE_YM

class SatTestFixture :
  public ::testing::TestWithParam<string>
{
public:

  /// @brief コンストラクタ
  SatTestFixture();

  /// @brief デストラクタ
  ~SatTestFixture() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 設定されたCNFが vals[] で示された真理値表と等しいか調べる．
  void
  check(
    int ni,
    const vector<int>& vals
  );

  /// @brief check の条件リテラル付き版
  ///
  /// 最初の変数が1のときのみ意味を持つ．
  void
  check_with_cond1(
    int ni,
    const vector<int>& vals
  );

  /// @brief 論理ゲートの真理値表からチェック用のベクタを作る．
  static
  vector<int>
  make_vals(
    int ni,
    const vector<int>& tv,
    bool inv
  );


public:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver mSolver;

  // 変数の数
  int mVarNum;

  // 変数のリスト
  vector<SatLiteral> mVarList;

  // 条件変数のリスト
  vector<SatLiteral> mCondVarList;

};

END_NAMESPACE_YM
