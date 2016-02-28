#ifndef YM_SATLITERAL_H
#define YM_SATLITERAL_H

/// @file ym/SatLiteral.h
/// @brief SatLiteral のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2016 Yusuke Matsunaga
/// All rights reserved.


#include "ym_config.h"
#include "ym/SatVarId.h"
#include "ym/HashFunc.h"


BEGIN_NAMESPACE_YM

//////////////////////////////////////////////////////////////////////
/// @ingroup LogicGroup
/// @class SatLiteral SatLiteral.h "ym/SatLiteral.h"
/// @brief リテラル(変数番号＋極性)を表すクラス
/// @sa SatVarId
//////////////////////////////////////////////////////////////////////
class SatLiteral
{
public:

  /// @brief デフォルトコンストラクタ
  /// 内容は不定なのであまり使わない方が良い．
  SatLiteral();

  /// @brief 変数番号と極性を指定したコンストラクタ
  /// @param[in] varid 変数番号
  /// @param[in] inv 極性
  ///                - false: 反転なし (正極性)
  ///                - true:  反転あり (負極性)
  explicit
  SatLiteral(SatVarId varid,
	     bool inv = false);

  /// @brief index からの変換関数
  /// @param[in] index 変数番号を極性をエンコードしたもの
  static
  SatLiteral
  index2literal(ymuint index);

  // コピーコンストラクタ,代入演算子,デストラクタはデフォルト
  // のものがそのまま使える．


public:
  //////////////////////////////////////////////////////////////////////
  // 内容をセットする関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内容を設定する．
  /// @param[in] varid 変数番号
  /// @param[in] inv 極性
  ///                - false: 反転なし (正極性)
  ///                - true:  反転あり (負極性)
  void
  set(SatVarId varid,
      bool inv = false);


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数番号を得る．
  /// @return 変数番号
  SatVarId
  varid() const;

  /// @brief 正極性のリテラルの時 true を返す．
  bool
  is_positive() const;

  /// @brief 負極性のリテラルの時 true を返す．
  bool
  is_negative() const;

  /// @brief 極性の反転
  /// @return 極性を反転させたリテラルを返す．
  SatLiteral
  operator~() const;

  /// @brief 同じ変数の正極性リテラルを返す．
  SatLiteral
  make_positive() const;

  /// @brief 同じ変数の負極性リテラルを返す．
  SatLiteral
  make_negative() const;

  /// @brief ハッシュ用の関数
  ymuint
  hash() const;

  /// @brief 配列のインデックスとして使用可能な数を返す．
  ymuint
  index() const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部でのみ用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 内部でのみ用いるコンストラクタ
  explicit
  SatLiteral(ymuint body);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数番号と極性をパックしたもの
  ymuint mBody;

};

/// @relates SatLiteral
/// @brief 未定義リテラル
extern
const SatLiteral kSatLiteralX;

/// @relates SatLiteral
/// @brief 比較関数
/// @param[in] lit1, lit2 比較対象のリテラル
/// @retval -1 lit1 < lit2
/// @retval  0 lit1 = lit2
/// @retval  1 lit1 > lit2
int
compare(SatLiteral lit1,
	SatLiteral lit2);

/// @relates SatLiteral
/// @brief 等価比較
/// @param[in] lit1, lit2 比較するリテラル
/// @return lit1 と lit2 が等しいリテラルの時 true を返す．
bool
operator==(SatLiteral lit1,
	   SatLiteral lit2);

/// @relates SatLiteral
/// @brief 非等価比較
/// @param[in] lit1, lit2 比較するリテラル
/// @return lit1 と lit2 が等しいリテラルでないとき true を返す．
bool
operator!=(SatLiteral lit1,
	   SatLiteral lit2);

/// @relates SatLiteral
/// @brief 小なり比較
/// @param[in] lit1, lit2 比較するリテラル
/// @return lit1 が lit2 より小さいとき true を返す．
bool
operator<(SatLiteral lit1,
	  SatLiteral lit2);

/// @relates SatLiteral
/// @brief 大なり比較
/// @param[in] lit1, lit2 比較するリテラル
/// @return lit1 が lit2 より大きいとき true を返す．
bool
operator>(SatLiteral lit1,
	  SatLiteral lit2);

/// @relates SatLiteral
/// @brief 小なりイコール比較
/// @param[in] lit1, lit2 比較するリテラル
/// @return lit1 が lit2 より小さいか等しいとき true を返す．
bool
operator<=(SatLiteral lit1,
	   SatLiteral lit2);

/// @relates SatLiteral
/// @brief 大なりイコール比較
/// @param[in] lit1, lit2 比較するリテラル
/// @return lit1 が lit2 より大きいか等しいとき true を返す．
bool
operator>=(SatLiteral lit1,
	   SatLiteral lit2);

/// @relates SatLiteral
/// @brief SatLiteral の内容を ostream に出力する関数
/// @param[in] s 出力ストリーム
/// @param[in] lit 出力対象のリテラル
/// @return s
ostream&
operator<<(ostream& s,
	   const SatLiteral& lit);


//////////////////////////////////////////////////////////////////////
// 上記のクラスを要素とするコンテナクラス
//////////////////////////////////////////////////////////////////////

/// @brief リテラルのベクタ
typedef vector<SatLiteral> SatLiteralVector;

/// @brief リテラルのリスト
typedef list<SatLiteral> SatLiteralList;

// SatLiteral をキーにしたハッシュ関数クラス
template <>
struct HashFunc<SatLiteral>
{
  ymuint
  operator()(SatLiteral lit) const
  {
    return lit.hash();
  }
};


//////////////////////////////////////////////////////////////////////
// inline 関数の定義
//////////////////////////////////////////////////////////////////////

// 内容を設定する．
inline
void
SatLiteral::set(SatVarId varid,
	     bool inv)
{
  mBody = (varid.val() << 1) + static_cast<ymuint>(inv);
}

// デフォルトコンストラクタ
inline
SatLiteral::SatLiteral() :
  mBody(0xfffffffe)
{
}

// 変数番号と極性を指定したコンストラクタ
inline
SatLiteral::SatLiteral(SatVarId varid,
		 bool inv)
{
  set(varid, inv);
}

// 内部でのみ用いるコンストラクタ
inline
SatLiteral::SatLiteral(ymuint32 body) :
  mBody(body)
{
}

// @brief index からの変換関数
inline
SatLiteral
SatLiteral::index2literal(ymuint32 index)
{
  return SatLiteral(index);
}

// 変数番号を得る．
inline
SatVarId
SatLiteral::varid() const
{
  return SatVarId(mBody >> 1);
}

// @brief 正極性のリテラルの時 true を返す．
inline
bool
SatLiteral::is_positive() const
{
  return !is_negative();
}

// @brief 負極性のリテラルの時 true を返す．
inline
bool
SatLiteral::is_negative() const
{
  return static_cast<bool>(mBody & 1U);
}

// 極性を反転させたリテラルを返す．
inline
SatLiteral
SatLiteral::operator~() const
{
  return SatLiteral(mBody ^ 1U);
}

// @brief 同じ変数の正極性リテラルを返す．
inline
SatLiteral
SatLiteral::make_positive() const
{
  return SatLiteral(mBody & ~1U);
}

// @brief 同じ変数の負極性リテラルを返す．
inline
SatLiteral
SatLiteral::make_negative() const
{
  return SatLiteral(mBody | 1U);
}

// @brief 比較関数
// @param[in] lit1, lit2 比較対象のリテラル
// @retval -1 lit1 < lit2
// @retval  0 lit1 = lit2
// @retval  1 lit1 > lit2
inline
int
compare(SatLiteral lit1,
	SatLiteral lit2)
{
  if ( lit1.index() < lit2.index() ) {
    return -1;
  }
  if ( lit1.index() > lit2.index() ) {
    return 1;
  }
  return 0;
}

// 等価比較
inline
bool
operator==(SatLiteral lit1,
	   SatLiteral lit2)
{
  return compare(lit1, lit2) == 0;
}
inline
bool
operator!=(SatLiteral lit1,
	   SatLiteral lit2)
{
  return !operator==(lit1, lit2);
}

// @brief 小なり比較
inline
bool
operator<(SatLiteral lit1,
	  SatLiteral lit2)
{
  return compare(lit1, lit2) == -1;
}

// @brief 大なり比較
inline
bool
operator>(SatLiteral lit1,
	  SatLiteral lit2)
{
  return operator<(lit2, lit1);
}

// @brief 小なりイコール比較
inline
bool
operator<=(SatLiteral lit1,
	   SatLiteral lit2)
{
  return !operator<(lit2, lit1);
}

// @brief 大なりイコール比較
inline
bool
operator>=(SatLiteral lit1,
	   SatLiteral lit2)
{
  return !operator<(lit1, lit2);
}

// ハッシュ用の関数
inline
ymuint
SatLiteral::hash() const
{
  return mBody;
}

// @brief 配列のインデックスとして使用可能な数を返す．
inline
ymuint
SatLiteral::index() const
{
  return mBody;
}

// @relates SatLiteral
// @brief SatLiteral の内容を ostream に出力する関数
// @param[in] s 出力ストリーム
// @param[in] lit 出力対象のリテラル
// @return s
inline
ostream&
operator<<(ostream& s,
	   const SatLiteral& lit)
{
  if ( lit == kSatLiteralX ) {
    s << "-X-";
  }
  else {
    s << "v_" << lit.varid();
    if ( lit.is_negative() ) {
      s << "'";
    }
  }
  return s;
}

END_NAMESPACE_YM


BEGIN_NAMESPACE_STD

template <>
struct equal_to<nsYm::SatLiteral>
{
  bool
  operator()(nsYm::SatLiteral n1,
	     nsYm::SatLiteral n2) const
  {
    return n1 == n2;
  }
};

END_NAMESPACE_STD

#endif // YM_SATLITERAL_H
