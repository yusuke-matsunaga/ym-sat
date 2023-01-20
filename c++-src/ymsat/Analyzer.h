#ifndef ANALYZER_H
#define ANALYZER_H

/// @file Analyzer.h
/// @brief Analyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"
#include "CoreMgr.h"
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
/// そのため，Analyzer の大きな役割は YmSat とのインターフェイスを
/// 提供することである．もう一つの仕事は，派生クラスが YmSat の
/// private メンバ関数にアクセスするための代理関数を提供することである．
//////////////////////////////////////////////////////////////////////
class Analyzer
{
public:

  /// @brief コンストラクタ
  Analyzer(
    CoreMgr& mgr ///< [in] コアマネージャ
  ) : mMgr{mgr}
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
  /// @return バックトラックレベルと学習された節を表すリテラルのベクタを返す．
  virtual
  tuple<int, vector<Literal>>
  analyze(
    Reason creason ///< [in] 矛盾の原因
  ) = 0;

  /// @brief 新しい変数が追加されたときに呼ばれる仮想関数
  virtual
  void
  alloc_var(
    SizeType size ///< [in] 要求するサイズ
  ) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスに直接 YmSat をアクセスさせないための代理関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 現在の decision level を取り出す．
  int
  decision_level() const
  {
    return mMgr.decision_level();
  }

  /// @brief 割り当てリストの末尾の位置を得る．
  int
  last_assign()
  {
    return mMgr.last_assign();
  }

  /// @brief 割り当てリストの pos 番めの要素を得る．
  Literal
  get_assign(
    int pos ///< [in] 位置番号
  )
  {
    return mMgr.get_assign(pos);
  }

  /// @brief 変数の decision level を得る．
  int
  decision_level(
    int varid ///< [in] 対象の変数
  ) const
  {
    return mMgr.decision_level(varid);
  }

  /// @brief 変数の割り当て理由を得る．
  Reason
  reason(
    int varid ///< [in] 対象の変数
  ) const
  {
    return mMgr.reason(varid);
  }

  /// @brief 変数のアクティビティを増加させる．
  void
  bump_var_activity(
    int varid ///< [in] 対象の変数
  )
  {
    mMgr.bump_var_activity(varid);
  }

  /// @brief 節のアクティビティを上げる．
  void
  bump_clause_activity(
    Clause* clause ///< [in] 対象の節
  )
  {
    mMgr.bump_clause_activity(clause);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // コアマネージャ
  CoreMgr& mMgr;

};


//////////////////////////////////////////////////////////////////////
/// @class SaFactory Analyzer.h "Analyzer.h"
/// @brief Analyzer(の派生クラス)を生成するファクトリ
//////////////////////////////////////////////////////////////////////
class SaFactory
{
public:

  /// @brief Analyzerの派生クラスを生成する．
  static
  Analyzer*
  gen_analyzer(
    CoreMgr& mgr,                   ///< [in] コアマネージャ
    const string& option = string{} ///< [in] どのクラスを生成するかを決めるオプション文字列
  );

};

END_NAMESPACE_YM_SAT

#endif // ANALYZER_H
