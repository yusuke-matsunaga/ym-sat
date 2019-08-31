﻿#ifndef SATANALYZER_H
#define SATANALYZER_H

/// @file SatAnalyzer.h
/// @brief SatAnalyzer のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018 Yusuke Matsunaga
/// All rights reserved.


#include "YmSat.h"
#include "SatReason.h"


BEGIN_NAMESPACE_YM_SAT1

class SatClause;

//////////////////////////////////////////////////////////////////////
/// @class SatAnalyzer SatAnalyzer.h "SatAnalyzer.h"
/// @brief 矛盾の解析/学習を行うクラス
///
/// このクラスの役割は矛盾の原因となった節を解析して，矛盾の解消に必要
/// な学習節(のためのリテラル集合)を生成することである．
/// ただし，学習節の生成法は唯一ではないので，SatAnalyzer を純粋仮想
/// 基底クラスにして派生クラスでさまざまな手法を実装できるようにしてい
/// る．
/// そのため，SatAnalyzer の大きな役割は YmSat とのインターフェイスを
/// 提供することである．もう一つの仕事は，派生クラスが YmSat の
/// private メンバ関数にアクセスするための代理関数を提供することである．
//////////////////////////////////////////////////////////////////////
class SatAnalyzer
{
public:

  /// @brief コンストラクタ
  /// @param[in] solver SATソルバ
  SatAnalyzer(YmSat* solver);

  /// @brief デストラクタ
  virtual
  ~SatAnalyzer() { }


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
  decision_level(int varid) const;

  /// @brief 変数の割り当て理由を得る．
  /// @param[in] varid 対象の変数
  SatReason
  reason(int varid) const;

  /// @brief 変数のアクティビティを増加させる．
  /// @param[in] varid 対象の変数
  void
  bump_var_activity(int varid);

  /// @brief 節のアクティビティを上げる．
  /// @param[in] clause 対象の節
  void
  bump_clause_activity(SatClause* clause);


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // YmSat へのポインタ
  YmSat* mSolver;

};


//////////////////////////////////////////////////////////////////////
/// @class SaFactory SatAnalyzer.h "SatAnalyzer.h"
/// @brief SatAnalyzer(の派生クラス)を生成するファクトリ
//////////////////////////////////////////////////////////////////////
class SaFactory
{
public:

  /// @brief SatAnalyzerの派生クラスを生成する．
  /// @param[in] solver SATソルバ
  /// @param[in] option どのクラスを生成するかを決めるオプション文字列
  static
  SatAnalyzer*
  gen_analyzer(YmSat* solver,
	       const string& option = string());

};


//////////////////////////////////////////////////////////////////////
// インライン関数の定義
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] solver SATソルバ
inline
SatAnalyzer::SatAnalyzer(YmSat* solver) :
  mSolver(solver)
{
}

// 現在の decision level を取り出す．
inline
int
SatAnalyzer::decision_level() const
{
  return mSolver->decision_level();
}

// 割り当てリストの末尾を得る．
inline
int
SatAnalyzer::last_assign()
{
  return mSolver->mAssignList.size() - 1;
}

// 割り当てリストの pos 番めの要素を得る．
inline
SatLiteral
SatAnalyzer::get_assign(int pos)
{
  return mSolver->mAssignList.get(pos);
}

// 変数の decision level を得る．
inline
int
SatAnalyzer::decision_level(int varid) const
{
  return mSolver->decision_level(varid);
}

// 変数の割り当て理由を得る．
inline
SatReason
SatAnalyzer::reason(int varid) const
{
  return mSolver->reason(varid);
}

// 変数のアクティビティを増加させる．
inline
void
SatAnalyzer::bump_var_activity(int var)
{
  mSolver->bump_var_activity(var);
}

// 学習節のアクティビティを増加させる．
inline
void
SatAnalyzer::bump_clause_activity(SatClause* clause)
{
  mSolver->bump_clause_activity(clause);
}

END_NAMESPACE_YM_SAT1

#endif // SATANALYZER_H
