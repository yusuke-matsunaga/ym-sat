#ifndef ANALYZER_H
#define ANALYZER_H

/// @file Analyzer.h
/// @brief Analyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/sat.h"
#include "ymsat/CoreMgr.h"
#include "ymsat/SatReason.h"


BEGIN_NAMESPACE_YM_SAT

class SatClause;

/////////////////////p/////////////////////////////////////////////////
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
  /// @param[in] mgr コアマネージャ
  Analyzer(CoreMgr& mgr);

  /// @brief デストラクタ
  virtual
  ~Analyzer() { }


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief 解析を行う．
  /// @param[in] creason 矛盾の原因
  /// @param[out] learnt 学習された節を表すリテラルのベクタ
  /// @return バックトラックレベル
  virtual
  int
  analyze(SatReason creason,
	  vector<SatLiteral>& learnt) = 0;

  /// @brief 新しい変数が追加されたときに呼ばれる仮想関数
  virtual
  void
  alloc_var(int size) = 0;


protected:
  //////////////////////////////////////////////////////////////////////
  // 派生クラスに直接 YmSat をアクセスさせないための代理関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 現在の decision level を取り出す．
  int
  decision_level() const;

  /// @brief 割り当てリストの末尾の位置を得る．
  int
  last_assign();

  /// @brief 割り当てリストの pos 番めの要素を得る．
  /// @param[in] pos 位置番号
  SatLiteral
  get_assign(int pos);

  /// @brief 変数の decision level を得る．
  /// @param[in] varid 対象の変数
  int
  decision_level(SatVarId varid) const;

  /// @brief 変数の割り当て理由を得る．
  /// @param[in] varid 対象の変数
  SatReason
  reason(SatVarId varid) const;

  /// @brief 変数のアクティビティを増加させる．
  /// @param[in] varid 対象の変数
  void
  bump_var_activity(SatVarId varid);

  /// @brief 節のアクティビティを上げる．
  /// @param[in] clause 対象の節
  void
  bump_clause_activity(SatClause* clause);


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
  /// @param[in] mgr コアマネージャ
  /// @param[in] option どのクラスを生成するかを決めるオプション文字列
  static
  Analyzer*
  gen_analyzer(CoreMgr& mgr,
	       const string& option = string());

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] mgr コアマネージャ
inline
Analyzer::Analyzer(CoreMgr& mgr) :
  mMgr(mgr)
{
}

// 現在の decision level を取り出す．
inline
int
Analyzer::decision_level() const
{
  return mMgr.decision_level();
}

// 割り当てリストの末尾を得る．
inline
int
Analyzer::last_assign()
{
  return mMgr.last_assign();
}

// 割り当てリストの pos 番めの要素を得る．
inline
SatLiteral
Analyzer::get_assign(int pos)
{
  return mMgr.get_assign(pos);
}

// 変数の decision level を得る．
inline
int
Analyzer::decision_level(SatVarId varid) const
{
  return mMgr.decision_level(varid);
}

// 変数の割り当て理由を得る．
inline
SatReason
Analyzer::reason(SatVarId varid) const
{
  return mMgr.reason(varid);
}

// 変数のアクティビティを増加させる．
inline
void
Analyzer::bump_var_activity(SatVarId var)
{
  mMgr.bump_var_activity(var);
}

// 学習節のアクティビティを増加させる．
inline
void
Analyzer::bump_clause_activity(SatClause* clause)
{
  mMgr.bump_clause_activity(clause);
}

END_NAMESPACE_YM_SAT

#endif // ANALYZER_H
