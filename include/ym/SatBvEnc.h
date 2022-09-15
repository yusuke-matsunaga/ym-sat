#ifndef YM_SATBVENC_H
#define YM_SATBVENC_H

/// @file ym/SatBvEnc.h
/// @brief SatBvEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatBvEnc SatBvEnc.h "SatBvEnc.h"
/// @brief 'bit vector' encoder
///
/// 複数の命題変数のベクタを2進表現の整数とみなして
/// 整数演算に関する制約を生成する符号化器
//////////////////////////////////////////////////////////////////////
class SatBvEnc
{
public:

  /// @brief コンストラクタ
  SatBvEnc(
    SatSolver& solver ///< [in] SATソルバ
  );

  /// @brief デストラクタ
  ~SatBvEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief A == B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_eq(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  );

  /// @brief A == B という条件を追加する．
  void
  add_eq(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val                        ///< [in] Bの値
  );

  /// @brief A != B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_ne(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  );

  /// @brief A != B という条件を追加する．
  void
  add_ne(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  /// @brief A < B という条件を追加する．
  /// @param[in] a_vec, b_vec A,Bを表すビットベクタ
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_lt(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  );

  /// @brief A < B という条件を追加する．
  void
  add_lt(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  /// @brief A <= B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_le(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  );

  /// @brief A <= B という条件を追加する．
  void
  add_le(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  /// @brief A > B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_gt(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  )
  {
    add_lt(b_vec, a_vec);
  }

  /// @brief A > B という条件を追加する．
  void
  add_gt(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );

  /// @brief A >= B という条件を追加する．
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_ge(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    const vector<SatLiteral>& b_vec  ///< [in] B を表すビットベクタ
  )
  {
    add_le(b_vec, a_vec);
  }

  /// @brief A >= B という条件を追加する．
  void
  add_ge(
    const vector<SatLiteral>& a_vec, ///< [in] A を表すビットベクタ
    int b_val			     ///< [in] Bの値
  );


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATBVENC_H
