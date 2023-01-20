#ifndef YMSAT_REASON_H
#define YMSAT_REASON_H

/// @file Reason.h
/// @brief Reason のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "Literal.h"


BEGIN_NAMESPACE_YM_SAT

class Clause;

//////////////////////////////////////////////////////////////////////
/// @class Reason Reason.h "Reason.h"
/// @brief 含意の原因を表すクラス
///
/// (a1 + a2 + a3 + ... an + b) という節があるとする．
/// 今，a1 = a2 = a3 = ... an = 0 になるとこの節を充足させるためには
/// b = 1 が必要となる．そこで，この節(および a1 ... an の割り当て)が
/// b = 1 という割り当ての原因となる．
/// YmSat::implication() の中でこのような含意が起きたときには
/// 結果のリテラル(上の例では b)を最初のリテラルに移動しているので，
/// 通常であれば SatClause そのもので原因を表現しても構わない．
/// ただし，もともとの節が (a + b) の形なら節の代わりに ~a というリテラ
/// ルを用いて原因を表すこともできる．そこで MiniSat では GClause
/// という節とリテラルの両方を一般化したクラスを用いている．
/// ここではそれに倣い，Clause へのポインタと Literal を排他的に
/// 表現するクラスを作った．
//////////////////////////////////////////////////////////////////////
class Reason
{
public:

  /// @brief コンストラクタ
  explicit
  Reason(
    Clause* clause = nullptr ///< [in] 節へのポインタ
  ) : mBody{reinterpret_cast<ympuint>(clause)}
  {
  }

  /// @brief コンストラクタ
  explicit
  Reason(
    Literal lit ///< [in] リテラル
  ) : mBody{(lit.index() << 1) | 1UL}
  {
  }


public:

  /// @brief 内容が節の時 true を返す．
  bool
  is_clause() const
  {
    return !is_literal();
  }

  /// @brief 節を取り出す．
  Clause*
  clause() const
  {
    return reinterpret_cast<Clause*>(mBody);
  }

  /// @brief 内容がリテラルの時 true を返す．
  bool
  is_literal() const
  {
    return static_cast<bool>(mBody & 1UL);
  }

  /// @brief リテラルを取り出す．
  Literal
  literal() const
  {
    return Literal::index2literal(mBody >> 1);
  }

  /// @brief 等価比較演算子
  bool
  operator==(
    const Reason& right ///< [in] 比較対象のオペランド
  ) const
  {
    return mBody == right.mBody;
  }

  /// @brief 非等価比較演算子
  bool
  operator!=(
    const Reason& right
  )
  {
    return !operator==(right);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // Clause か Literal を保持する
  ympuint mBody;


public:
  //////////////////////////////////////////////////////////////////////
  // 定数
  //////////////////////////////////////////////////////////////////////

  /// @brief 空の Reason を表す定数
  static
  const Reason None;

};

/// @relates Reason
/// @brief Reason の内容を出力する
ostream&
operator<<(
  ostream& s,
  const Reason& c
);

END_NAMESPACE_YM_SAT

#endif // YMSAT_REASON_H
