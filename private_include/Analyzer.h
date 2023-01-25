#ifndef ANALYZER_H
#define ANALYZER_H

/// @file Analyzer.h
/// @brief Analyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "SatCore.h"
#include "Reason.h"


BEGIN_NAMESPACE_YM_SAT

class Clause;

//////////////////////////////////////////////////////////////////////
/// @class Analyzer Analyzer.h "Analyzer.h"
/// @brief 矛盾の解析/学習を行うクラス
///
/// このクラスの役割は矛盾の原因となった節を解析して，矛盾の解消に必要
/// な学習節(のためのリテラル集合)を生成することである．
/// ただし，学習節の生成法は唯一ではないので，Analyzer を純粋仮想
/// 基底クラスにして派生クラスでさまざまな手法を実装できるようにしてい
/// る．
/// そのため，Analyzer の大きな役割は SatCore とのインターフェイスを
/// 提供することである．もう一つの仕事は，派生クラスが SatCore の
/// private メンバ関数にアクセスするための代理関数を提供することである．
//////////////////////////////////////////////////////////////////////
class Analyzer
{
public:

  /// @brief Analyzerの派生クラスを生成する．
  static
  Analyzer*
  new_obj(
    SatCore& core,                ///< [in] コアマネージャ
    const string& type = string{} ///< [in] タイプを表す文字列
  );

  /// @brief コンストラクタ
  Analyzer(
    SatCore& core ///< [in] コアマネージャ
  ) : mCore{core}
  {
  }

  /// @brief デストラクタ
  virtual
  ~Analyzer() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 解析を行う．
  /// @return バックトラックレベルを返す．
  virtual
  int
  analyze(
    Reason creason,              ///< [in] 矛盾の原因
    vector<Literal>& learnt_lits ///< [out] 学習された節を表すリテラルのベクタ
  ) = 0;

  /// @brief 新しい変数が追加されたときに呼ばれる仮想関数
  virtual
  void
  alloc_var(
    SizeType size ///< [in] 要求するサイズ
  ) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスに直接 SatCore をアクセスさせないための代理関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 現在の decision level を取り出す．
  int
  decision_level() const
  {
    return mCore.decision_level();
  }

  /// @brief 割り当てリストの末尾の位置を得る．
  SizeType
  last_assign()
  {
    return mCore.last_assign();
  }

  /// @brief 割り当てリストの pos 番めの要素を得る．
  Literal
  get_assign(
    SizeType pos ///< [in] 位置番号
  )
  {
    return mCore.get_assign(pos);
  }

  /// @brief 変数の decision level を得る．
  int
  decision_level(
    SatVarId varid ///< [in] 対象の変数
  ) const
  {
    return mCore.decision_level(varid);
  }

  /// @brief 変数の割り当て理由を得る．
  Reason
  reason(
    SatVarId varid ///< [in] 対象の変数
  ) const
  {
    return mCore.reason(varid);
  }

  /// @brief 変数のアクティビティを増加させる．
  void
  bump_var_activity(
    SatVarId varid ///< [in] 対象の変数
  )
  {
    mCore.bump_var_activity(varid);
  }

  /// @brief 節のアクティビティを上げる．
  void
  bump_clause_activity(
    Clause* clause ///< [in] 対象の節
  )
  {
    mCore.bump_clause_activity(clause);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コアマネージャ
  SatCore& mCore;

};

END_NAMESPACE_YM_SAT

#endif // ANALYZER_H
