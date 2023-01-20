#ifndef YM_SATLITERAL_H
#define YM_SATLITERAL_H

/// @file ym/SatLiteral.h
/// @brief SatLiteral のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2016, 2017, 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @ingroup SatGroup
/// @class SatLiteral SatLiteral.h "ym/SatLiteral.h"
/// @brief リテラル(変数番号＋極性)を表すクラス
///
/// ユーザーは SatSolver::new_variable() の返り値としてのみ
/// このクラスのインスタンスを生成できる．
///
/// 変数番号は 0 から始まる非負の整数
/// 単純には最下位ビットで極性を表すだけだが，
/// 不正な値のときにはこのビットに1を立てない．
/// そのために neg_mask() というメンバ関数を用意している．
//////////////////////////////////////////////////////////////////////
class SatLiteral
{
  friend class SatSolver;

public:

  /// @brief デフォルトコンストラクタ
  ///
  /// 内容は不定なのであまり使わない方が良い．
  SatLiteral() = default;

  /// @brief コピーコンストラクタの変種
  SatLiteral(
    const SatLiteral& lit, ///< [in] リテラル
    bool inv = false       ///< [in] 極性フラグ
                           ///       - false: 反転なし (正極性)
                           ///       - true:  反転あり (負極性)
  ) : mIndex{lit.mIndex}
  {
    if ( inv ) {
      mIndex ^= neg_mask();
    }
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内容をセットする関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数番号と極性からの変換関数
  static
  SatLiteral
  conv_from_varid(
    int varid,       ///< [in] 変数番号
    bool inv = false ///< [in] 極性フラグ
                     ///       - false: 反転なし (正極性)
                     ///       - true:  反転あり (負極性)
  )
  {
    SatLiteral ans;
    ans.set(varid, inv);
    return ans;
  }

  /// @brief index からの変換関数
  static
  SatLiteral
  index2literal(
    SizeType index ///< [in] 変数番号と極性をエンコードしたもの
  )
  {
    SatLiteral ans;
    ans.mIndex = index;
    return ans;
  }

  /// @brief 内容を設定する．
  void
  set(
    int varid,       ///< [in] 変数番号
    bool inv = false ///< [in] 極性フラグ
		     ///       - false: 反転なし (正極性)
    		     ///       - true:  反転あり (負極性)
  )
  {
    if ( varid < 0 ) {
      mIndex = -1;
    }
    else {
      mIndex = (varid << 1) + static_cast<SizeType>(inv);
    }
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容が適正の時に true を返す．
  bool
  is_valid() const
  {
    return mIndex != -1;
  }

  /// @brief 変数番号を得る．
  /// @return 変数番号
  int
  varid() const
  {
    if ( is_valid() ) {
      return mIndex >> 1;
    }
    else {
      return -1;
    }
  }

  /// @brief 正極性のリテラルの時 true を返す．
  bool
  is_positive() const
  {
    return !is_negative();
  }

  /// @brief 負極性のリテラルの時 true を返す．
  bool
  is_negative() const
  {
    return static_cast<bool>(mIndex & 1);
  }

  /// @brief ハッシュ用の関数
  SizeType
  hash() const
  {
    return mIndex;
  }

  /// @brief 配列のインデックスとして使用可能な数を返す．
  SizeType
  index() const
  {
    return mIndex;
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 演算
  //////////////////////////////////////////////////////////////////////

  /// @brief 自身の極性を反転させる．
  /// @return 自身の参照を返す．
  ///
  /// 不正な値の場合は変化しない．
  const SatLiteral&
  invert()
  {
    mIndex ^= neg_mask();
    return *this;
  }

  /// @brief 極性の反転
  /// @return 極性を反転させたリテラルを返す．
  ///
  /// 不正な値の場合は変化しない．
  SatLiteral
  operator~() const
  {
    return index2literal(mIndex ^ neg_mask());
  }

  /// @brief 極性を作用させる
  /// @return 結果のリテラルを返す．
  SatLiteral
  operator*(
    bool inv ///< [in] 反転フラグ
  ) const
  {
    if ( inv ) {
      return operator~();
    }
    else {
      return *this;
    }
  }

  /// @brief 同じ変数の正極性リテラルを返す．
  ///
  /// 不正な値の場合は変化しない．
  SatLiteral
  make_positive() const
  {
    // 不正な値の場合でもこれはOK
    return index2literal(mIndex & ~1U);
  }

  /// @brief 同じ変数の負極性リテラルを返す．
  ///
  /// 不正な値の場合は変化しない．
  SatLiteral
  make_negative() const
  {
    return index2literal(mIndex | neg_mask());
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部でのみ用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 反転用のビットマスクを返す．
  ///
  /// 普通は 1U だが不正な値のときは 0U となる．
  SizeType
  neg_mask() const
  {
    return is_valid() ? 1U : 0U;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数番号と極性をパックしたもの
  SizeType mIndex{static_cast<SizeType>(-1)};


public:
  //////////////////////////////////////////////////////////////////////
  // 定数
  //////////////////////////////////////////////////////////////////////

  /// @brief 未定義リテラル
  static
  const SatLiteral X;

};

/// @relates SatLiteral
/// @brief 等価比較
/// @return lit1 と lit2 が等しいリテラルの時 true を返す．
inline
bool
operator==(
  SatLiteral lit1, ///< [in] オペランド1
  SatLiteral lit2  ///< [in] オペランド2
)
{
  return lit1.index() == lit2.index();
}

/// @relates SatLiteral
/// @brief 非等価比較
/// @return lit1 と lit2 が等しいリテラルでないとき true を返す．
inline
bool
operator!=(
  SatLiteral lit1, ///< [in] オペランド1
  SatLiteral lit2  ///< [in] オペランド2
)
{
  return !operator==(lit1, lit2);
}

/// @relates SatLiteral
/// @brief 小なり比較演算
inline
bool
operator<(
  SatLiteral lit1, ///< [in] オペランド1
  SatLiteral lit2  ///< [in] オペランド2
)
{
  return lit1.index() < lit2.index();
}

/// @relates SatLiteral
/// @brief 小なりイコール比較演算
inline
bool
operator<=(
  SatLiteral lit1, ///< [in] オペランド1
  SatLiteral lit2  ///< [in] オペランド2
)
{
  return !operator<(lit2, lit1);
}

/// @relates SatLiteral
/// @brief 大なり比較演算
inline
bool
operator>(
  SatLiteral lit1, ///< [in] オペランド1
  SatLiteral lit2  ///< [in] オペランド2
)
{
  return operator<(lit2, lit1);
}

/// @relates SatLiteral
/// @brief 大なりイコール比較演算
inline
bool
operator>=(
  SatLiteral lit1, ///< [in] オペランド1
  SatLiteral lit2  ///< [in] オペランド2
)
{
  return !operator<(lit1, lit2);
}

/// @relates SatLiteral
/// @brief SatLiteral の内容を ostream に出力する関数
/// @return s
inline
ostream&
operator<<(
  ostream& s,           ///< [in] 出力ストリーム
  const SatLiteral& lit ///< [in] 出力対象のリテラル
)
{
  if ( lit.is_valid() ) {
    s << lit.varid();
    if ( lit.is_negative() ) {
      s << "'";
    }
  }
  else {
    s << "-X-";
  }
  return s;
}

END_NAMESPACE_YM_SAT

BEGIN_NAMESPACE_STD

/// @brief SatLiteral をキーにしたハッシュ関数クラス
template <>
struct hash<YM_NAMESPACE::SatLiteral>
{
  SizeType
  operator()(
    YM_NAMESPACE::SatLiteral lit
  ) const
  {
    return lit.hash();
  }
};

END_NAMESPACE_STD

#endif // YM_SATLITERAL_H
