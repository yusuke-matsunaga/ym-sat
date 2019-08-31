#ifndef YM_SATBVENC_H
#define YM_SATBVENC_H

/// @file ym/SatBvEnc.h
/// @brief SatBvEnc のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2019 Yusuke Matsunaga
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
  /// @param[in] solver SATソルバ
  SatBvEnc(SatSolver& solver);

  /// @brief デストラクタ
  ~SatBvEnc();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief A == B という条件を追加する．
  /// @param[in] a_vec, b_vec A,Bを表すビットベクタ
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_eq(const vector<SatLiteral>& a_vec,
	 const vector<SatLiteral>& b_vec);

  /// @brief A == B という条件を追加する．
  /// @param[in] a_vec Aを表すビットベクタ
  /// @param[in] b_val Bの値
  void
  add_eq(const vector<SatLiteral>& a_vec,
	 int b_val);

  /// @brief A != B という条件を追加する．
  /// @param[in] a_vec, b_vec A,Bを表すビットベクタ
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_ne(const vector<SatLiteral>& a_vec,
	 const vector<SatLiteral>& b_vec);

  /// @brief A != B という条件を追加する．
  /// @param[in] a_vec Aを表すビットベクタ
  /// @param[in] b_val Bの値
  void
  add_ne(const vector<SatLiteral>& a_vec,
	 int b_val);

  /// @brief A < B という条件を追加する．
  /// @param[in] a_vec, b_vec A,Bを表すビットベクタ
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_lt(const vector<SatLiteral>& a_vec,
	 const vector<SatLiteral>& b_vec);

  /// @brief A < B という条件を追加する．
  /// @param[in] a_vec Aを表すビットベクタ
  /// @param[in] b_val Bの値
  void
  add_lt(const vector<SatLiteral>& a_vec,
	 int b_val);

  /// @brief A <= B という条件を追加する．
  /// @param[in] a_vec, b_vec A,Bを表すビットベクタ
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_le(const vector<SatLiteral>& a_vec,
	 const vector<SatLiteral>& b_vec);

  /// @brief A <= B という条件を追加する．
  /// @param[in] a_vec Aを表すビットベクタ
  /// @param[in] b_val Bの値
  void
  add_le(const vector<SatLiteral>& a_vec,
	 int b_val);

  /// @brief A > B という条件を追加する．
  /// @param[in] a_vec, b_vec A,Bを表すビットベクタ
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_gt(const vector<SatLiteral>& a_vec,
	 const vector<SatLiteral>& b_vec);

  /// @brief A > B という条件を追加する．
  /// @param[in] a_vec Aを表すビットベクタ
  /// @param[in] b_val Bの値
  void
  add_gt(const vector<SatLiteral>& a_vec,
	 int b_val);

  /// @brief A >= B という条件を追加する．
  /// @param[in] a_vec, b_vec A,Bを表すビットベクタ
  ///
  /// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
  /// 0と仮定する．
  void
  add_ge(const vector<SatLiteral>& a_vec,
	 const vector<SatLiteral>& b_vec);

  /// @brief A >= B という条件を追加する．
  /// @param[in] a_vec Aを表すビットベクタ
  /// @param[in] b_val Bの値
  void
  add_ge(const vector<SatLiteral>& a_vec,
	 int b_val);


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // SATソルバ
  SatSolver& mSolver;

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief A > B という条件を追加する．
// @param[in] a_vec, b_vec A,Bを表すビットベクタ
//
// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
// 0と仮定する．
inline
void
SatBvEnc::add_gt(const vector<SatLiteral>& a_vec,
		 const vector<SatLiteral>& b_vec)
{
  add_lt(b_vec, a_vec);
}

// @brief A >= B という条件を追加する．
// @param[in] a_vec, b_vec A,Bを表すビットベクタ
//
// a_vec と b_vec のビット長が異なるときは短い方の上位ビットを
// 0と仮定する．
inline
void
SatBvEnc::add_ge(const vector<SatLiteral>& a_vec,
		 const vector<SatLiteral>& b_vec)
{
  add_le(b_vec, a_vec);
}

END_NAMESPACE_YM_SAT

#endif // YM_SATBVENC_H
