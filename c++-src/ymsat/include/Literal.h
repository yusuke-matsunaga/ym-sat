#ifndef YMSAT_LITERAL_H
#define YMSAT_LITERAL_H

/// @file Literal.h
/// @brief Literal のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "ym/SatLiteral.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class Literal Literal.h "Literal.h"
/// @brief リテラル(変数番号＋極性)を表すクラス
///
/// 変数番号は 0 から始まる非負の整数
/// 単純には最下位ビットで極性を表すだけだが，
/// 不正な値のときにはこのビットに1を立てない．
/// そのために neg_mask() というメンバ関数を用意している．
///
/// 中身はほぼ SatLiteral と同一
//////////////////////////////////////////////////////////////////////
class Literal
{
public:

  /// @brief デフォルトコンストラクタ
  ///
  /// 内容は不定なのであまり使わない方が良い．
  Literal() = default;

  /// @brief SatLiteral からの変換コンストラクタ
  explicit
  Literal(
    const SatLiteral& src ///< [in] もととなるリテラル
  )
  {
    auto varid = src.varid();
    bool inv = src.is_negative();
    set(varid, inv);
  }

  /// @brief 変数番号と極性からの変換関数
  static
  Literal
  conv_from_varid(
    SatVarId varid,  ///< [in] 変数番号
    bool inv = false ///< [in] 極性フラグ
                     ///       - false: 反転なし (正極性)
                     ///       - true:  反転あり (負極性)
  )
  {
    Literal ans;
    ans.set(varid, inv);
    return ans;
  }

  /// @brief index からの変換関数
  static
  Literal
  index2literal(
    SizeType index ///< [in] 変数番号と極性をエンコードしたもの
  )
  {
    Literal ans;
    ans.mIndex = index;
    return ans;
  }

  // コピーコンストラクタ,代入演算子,デストラクタはデフォルト
  // のものがそのまま使える．


public:
  //////////////////////////////////////////////////////////////////////
  // 内容をセットする関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  void
  set(
    SatVarId varid,  ///< [in] 変数番号
    bool inv = false ///< [in] 極性フラグ
		     ///       - false: 反転なし (正極性)
    		     ///       - true:  反転あり (負極性)
  )
  {
    mIndex = (varid << 1) + static_cast<int>(inv);
  }


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容が適正の時に true を返す．
  bool
  is_valid() const
  {
    return mIndex != BAD_SATVARID;
  }

  /// @brief 変数番号を得る．
  /// @return 変数番号
  SatVarId
  varid() const
  {
    if ( is_valid() ) {
      return mIndex >> 1;
    }
    else {
      return BAD_SATVARID;
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
  const Literal&
  invert()
  {
    mIndex ^= neg_mask();
    return *this;
  }

  /// @brief 極性の反転
  /// @return 極性を反転させたリテラルを返す．
  ///
  /// 不正な値の場合は変化しない．
  Literal
  operator~() const
  {
    return index2literal(mIndex ^ neg_mask());
  }

  /// @brief 極性を作用させる
  /// @return 結果のリテラルを返す．
  Literal
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
  Literal
  make_positive() const
  {
    // 不正な値の場合でもこれはOK
    return index2literal(mIndex & ~1U);
  }

  /// @brief 同じ変数の負極性リテラルを返す．
  ///
  /// 不正な値の場合は変化しない．
  Literal
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
  SizeType mIndex{static_cast<SizeType>(-1)};;


public:
  //////////////////////////////////////////////////////////////////////
  // 定数
  //////////////////////////////////////////////////////////////////////

  /// @brief 未定義リテラル
  static
  const Literal X;

};

/// @relates Literal
/// @brief 等価比較
/// @return lit1 と lit2 が等しいリテラルの時 true を返す．
inline
bool
operator==(
  Literal lit1, ///< [in] オペランド1
  Literal lit2  ///< [in] オペランド2
)
{
  return lit1.index() == lit2.index();
}

/// @relates Literal
/// @brief 非等価比較
/// @return lit1 と lit2 が等しいリテラルでないとき true を返す．
inline
bool
operator!=(
  Literal lit1, ///< [in] オペランド1
  Literal lit2  ///< [in] オペランド2
)
{
  return !operator==(lit1, lit2);
}

/// @relates Literal
/// @brief 小なり比較演算
inline
bool
operator<(
  Literal lit1, ///< [in] オペランド1
  Literal lit2  ///< [in] オペランド2
)
{
  return lit1.index() < lit2.index();
}

/// @relates Literal
/// @brief 小なりイコール比較演算
inline
bool
operator<=(
  Literal lit1, ///< [in] オペランド1
  Literal lit2  ///< [in] オペランド2
)
{
  return !operator<(lit2, lit1);
}

/// @relates Literal
/// @brief 大なり比較演算
inline
bool
operator>(
  Literal lit1, ///< [in] オペランド1
  Literal lit2  ///< [in] オペランド2
)
{
  return operator<(lit2, lit1);
}

/// @relates Literal
/// @brief 大なりイコール比較演算
inline
bool
operator>=(
  Literal lit1, ///< [in] オペランド1
  Literal lit2  ///< [in] オペランド2
)
{
  return !operator<(lit1, lit2);
}

/// @relates Literal
/// @brief Literal の内容を ostream に出力する関数
/// @return s
inline
ostream&
operator<<(
  ostream& s,           ///< [in] 出力ストリーム
  const Literal& lit ///< [in] 出力対象のリテラル
)
{
  if ( lit.is_valid() ) {
    s << "v_" << lit.varid();
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

/// @brief Literal をキーにしたハッシュ関数クラス
template <>
struct hash<YM_NAMESPACE::nsSat::Literal>
{
  SizeType
  operator()(
    YM_NAMESPACE::nsSat::Literal lit
  ) const
  {
    return lit.hash();
  }
};

END_NAMESPACE_STD

#endif // YMSAT_LITERAL_H
