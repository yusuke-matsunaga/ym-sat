
/// @file SatSolver.cc
/// @brief SatSolver の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016 2018 Yusuke Matsunaga
/// All rights reserved.


#include "ym/SatSolver.h"

#include "ymsat/YmSatMS2.h"
#include "ymsat/YmSat1.h"
#include "ymsat1/YmSat.h"
#include "ymsat_old/YmSatMS2.h"
#include "MiniSat/SatSolverMiniSat.h"
#include "MiniSat2/SatSolverMiniSat2.h"
#include "glueminisat-2.2.8/SatSolverGlueMiniSat2.h"
#include "lingeling/SatSolverLingeling.h"

#include "SatLogger.h"

#include "ym/CombiGen.h"
#include "ym/Range.h"


BEGIN_NAMESPACE_YM

const SatVarId kSatVarIdIllegal;
const SatLiteral kSatLiteralX;

END_NAMESPACE_YM

BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
// SatSolverImpl
//////////////////////////////////////////////////////////////////////

// @brief 継承クラスを作るクラスメソッド
// @param[in] solver_type SATソルバのタイプ
unique_ptr<SatSolverImpl>
SatSolverImpl::new_impl(const SatSolverType& solver_type)
{
  const string& type = solver_type.type();
  const string& option = solver_type.option();
  if ( type == "minisat" ) {
    // minisat-1.4
    return unique_ptr<SatSolverImpl>(new SatSolverMiniSat(option));
  }
  else if ( type == "minisat2" ) {
    // minisat-2.2
    return unique_ptr<SatSolverImpl>(new SatSolverMiniSat2(option));
  }
  else if ( type == "glueminisat2" ) {
    // glueminisat-2.2.8
    return unique_ptr<SatSolverImpl>(new SatSolverGlueMiniSat2(option));
  }
  else if ( type == string() || type == "lingeling" ) {
    // lingeling 今はデフォルトにしている．
    return unique_ptr<SatSolverImpl>(new SatSolverLingeling(option));
  }
  else if ( type == "ymsat1" ) {
    return unique_ptr<SatSolverImpl>(new YmSat1(option));
  }
  else if ( type == "ymsat2" ) {
    return unique_ptr<SatSolverImpl>(new YmSatMS2(option));
  }
  else if ( type == "ymsat2old" ) {
    return unique_ptr<SatSolverImpl>(new nsSatold::YmSatMS2(option));
  }
  else if ( type == "ymsat1_old" ) {
    return unique_ptr<SatSolverImpl>(new nsSat1::YmSat(option));
  }
  else {
    cerr << "SatSolver: unknown type '" << type << "', ymsat2 is used, instead." << endl;
    return unique_ptr<SatSolverImpl>(new YmSatMS2(option));
  }
}


//////////////////////////////////////////////////////////////////////
// SatSolver
//////////////////////////////////////////////////////////////////////

// @brief コンストラクタ
// @param[in] solver_type 実装タイプ
SatSolver::SatSolver(const SatSolverType& solver_type) :
  mImpl(SatSolverImpl::new_impl(solver_type)),
  mLogger(SatLogger::new_impl(solver_type))
{
}

// @brief デストラクタ
SatSolver::~SatSolver()
{
}

// @brief 変数を追加する．
// @param[in] decision 決定変数の時に true とする．
// @return 新しい変数番号を返す．
// @note 変数番号は 0 から始まる．
SatVarId
SatSolver::new_variable(bool decision)
{
  SatVarId id = mImpl->new_variable(decision);

  mLogger->new_variable(id);

  return id;
}

// @brief 条件リテラルを設定する．
// @param[in] lit_list 条件リテラルのリスト
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
void
SatSolver::set_conditional_literals(const vector<SatLiteral>& lit_list)
{
  mImpl->set_conditional_literals(lit_list);

  mLogger->set_conditional_literals(lit_list);
}

// @brief 条件リテラルを設定する．
// @param[in] lit1 条件リテラル
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
void
SatSolver::set_conditional_literals(SatLiteral lit1)
{
  mImpl->set_conditional_literals(1, &lit1);

  mLogger->set_conditional_literals(1, &lit1);
}

// @brief 条件リテラルを設定する．
// @param[in] lit1, lit2 条件リテラル
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
void
SatSolver::set_conditional_literals(SatLiteral lit1,
				    SatLiteral lit2)
{
  SatLiteral tmp_lits[] = {lit1, lit2};

  mImpl->set_conditional_literals(2, tmp_lits);

  mLogger->set_conditional_literals(2, tmp_lits);
}

// @brief 条件リテラルを設定する．
// @param[in] lit1, lit2, lit3 条件リテラル
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
void
SatSolver::set_conditional_literals(SatLiteral lit1,
				    SatLiteral lit2,
				    SatLiteral lit3)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3};

  mImpl->set_conditional_literals(3, tmp_lits);

  mLogger->set_conditional_literals(3, tmp_lits);
}

// @brief 条件リテラルを設定する．
// @param[in] lit1, lit2, lit3, lit4 条件リテラル
//
// 以降の add_clause() にはこのリテラルの否定が追加される．
// 条件リテラルを無効化するには clear_conditional_literals() を呼ぶ．
void
SatSolver::set_conditional_literals(SatLiteral lit1,
				    SatLiteral lit2,
				    SatLiteral lit3,
				    SatLiteral lit4)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4};

  mImpl->set_conditional_literals(4, tmp_lits);

  mLogger->set_conditional_literals(4, tmp_lits);
}

// @brief 条件リテラルをクリアする．
void
SatSolver::clear_conditional_literals()
{
  mImpl->set_conditional_literals(0, nullptr);

  mLogger->set_conditional_literals(0, nullptr);
}

// @brief リテラルを 'フリーズ' する．
//
// lingeling 以外は無効
void
SatSolver::freeze_literal(SatLiteral lit)
{
  mImpl->freeze_literal(lit);
}

// @brief リテラルを 'フリーズ' する．
//
// lingeling 以外は無効
void
SatSolver::freeze_literal(const vector<SatLiteral>& lits)
{
  for ( auto lit: lits ) {
    mImpl->freeze_literal(lit);
  }
}

// @brief 節を追加する．
// @param[in] lits リテラルのベクタ
void
SatSolver::add_clause(const vector<SatLiteral>& lits)
{
  mImpl->add_clause(lits);

  mLogger->add_clause(lits);
}

// @brief 1項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1)
{
  mImpl->add_clause(1, &lit1);

  mLogger->add_clause(1, &lit1);
}

// @brief 2項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2)
{
  SatLiteral tmp_lits[] = {lit1, lit2};

  mImpl->add_clause(2, tmp_lits);

  mLogger->add_clause(2, tmp_lits);
}

// @brief 3項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3};

  mImpl->add_clause(3, tmp_lits);

  mLogger->add_clause(3, tmp_lits);
}

// @brief 4項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4};

  mImpl->add_clause(4, tmp_lits);

  mLogger->add_clause(4, tmp_lits);
}

// @brief 5項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4,
		      SatLiteral lit5)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4, lit5};

  mImpl->add_clause(5, tmp_lits);

  mLogger->add_clause(5, tmp_lits);
}

// @brief 6項の節を追加する．
void
SatSolver::add_clause(SatLiteral lit1,
		      SatLiteral lit2,
		      SatLiteral lit3,
		      SatLiteral lit4,
		      SatLiteral lit5,
		      SatLiteral lit6)
{
  SatLiteral tmp_lits[] = {lit1, lit2, lit3, lit4, lit5, lit6};

  mImpl->add_clause(6, tmp_lits);

  mLogger->add_clause(6, tmp_lits);
}

// @brief n入力ANDゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_andgate_rel(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits(n + 1);
  for ( int i: Range<>(n) ) {
    SatLiteral ilit = lit_list[i];
    tmp_lits[i] = ~ilit;
    add_clause(ilit, ~olit);
  }
  tmp_lits[n] = olit;
  add_clause(tmp_lits);
}

// @brief n入力ORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_orgate_rel(SatLiteral olit,
			  const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits(n + 1);
  for ( int i: Range<>(n) ) {
    SatLiteral ilit = lit_list[i];
    add_clause(~ilit, olit);
    tmp_lits[i] = ilit;
  }
  tmp_lits[n] = ~olit;
  add_clause(tmp_lits);
}

// @brief n入力XORゲートの入出力の関係を表す条件を追加する．
// @param[in] olit 出力のリテラル
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_xorgate_rel(SatLiteral olit,
			   const vector<SatLiteral> lit_list)
{
  int n = lit_list.size();
  int n_exp = 1 << n;
  vector<SatLiteral> tmp_lits(n + 1);
  for ( int p: Range<>(n_exp) ) {
    int c = 0;
    for ( int i: Range<>(n) ) {
      SatLiteral ilit = lit_list[i];
      if ( p & (1 << i) ) {
	tmp_lits[i] = ~ilit;
	++ c;
      }
      else {
	tmp_lits[i] =  ilit;
      }
    }
    if ( (c % 2) == 1 ) {
      tmp_lits[n] =  olit;
    }
    else {
      tmp_lits[n] = ~olit;
    }
    add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうち1つしか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_at_most_one(const vector<SatLiteral>& lit_list)
{
  int n = lit_list.size();
  if ( n <= 1 ) {
    // はじめから条件は満たされている．
    return;
  }
  for ( int i1: Range<>(n - 1) ) {
    SatLiteral lit1 = lit_list[i1];
    for ( int i2: Range<>(i1 + 1, n) ) {
      SatLiteral lit2 = lit_list[i2];
      add_clause(~lit1, ~lit2);
    }
  }
}

// @brief 与えられたリテラルのうち2つしか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_at_most_two(const vector<SatLiteral>& lit_list)
{
  int n = lit_list.size();
  if ( n <= 2 ) {
    // はじめから条件は満たされている．
    return;
  }
  for ( int i1: Range<>(n - 1) ) {
    SatLiteral lit1 = lit_list[i1];
    for ( int i2: Range<>(i1 + 1, n) ) {
      SatLiteral lit2 = lit_list[i2];
      for ( int i3: Range<>(i2 + 1, n) ) {
	SatLiteral lit3 = lit_list[i3];
	add_clause(~lit1, ~lit2, ~lit3);
      }
    }
  }
}

// @brief 与えられたリテラルのうちk個しか true にならない条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatSolver::add_at_most_k(const vector<SatLiteral>& lit_list,
			 int k)
{
  int n = lit_list.size();
  if ( n <= k) {
    // はじめから条件は満たされている．
    return;
  }

  vector<SatLiteral> tmp_lits(k + 1);
  for ( CombiGen cg(n, k + 1) ; !cg.is_end(); ++ cg ) {
    for ( int i: Range<>(k + 1) ) {
      tmp_lits[i] = ~lit_list[cg(i)];
    }
    add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうち2つ以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
void
SatSolver::add_at_least_two(const vector<SatLiteral>& lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits;
  tmp_lits.reserve(n - 1);
  for ( int i: Range<>(n) ) {
    tmp_lits.clear();
    for ( int j: Range<>(n) ) {
      SatLiteral lit = lit_list[j];
      if ( i != j ) {
	tmp_lits.push_back(lit);
      }
    }
    add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうちk個以上は true になる条件を追加する．
// @param[in] lit_list 入力のリテラルのリスト
// @param[in] k しきい値
void
SatSolver::add_at_least_k(const vector<SatLiteral>& lit_list,
			  int k)
{
  int n = lit_list.size();
  if ( k == 0 ) {
    // はじめから条件は満たされている．
    return;
  }

  int nk = n - k;
  vector<SatLiteral> tmp_lits(nk + 1);
  for ( CombiGen cg(n, nk + 1); !cg.is_end(); ++ cg ) {
    for ( int i: Range<>(nk + 1) ) {
      tmp_lits[i] = lit_list[cg(i)];
    }
    add_clause(tmp_lits);
  }
}

// @brief 与えられたリテラルのうちtrueになっている個数が1でない条件を追加する．
// @param[in] lit_lit 入力のリテラルのリスト
void
SatSolver::add_not_one(const vector<SatLiteral>& lit_list)
{
  int n = lit_list.size();
  vector<SatLiteral> tmp_lits(n);
  for ( int i: Range<>(n) ) {
    for ( int j: Range<>(n) ) {
      SatLiteral lit = lit_list[j];
      if ( j == i ) {
	tmp_lits[j] = ~lit;
      }
      else {
	tmp_lits[j] =  lit;
      }
    }
    add_clause(tmp_lits);
  }
}

// @brief assumption 付きの SAT 問題を解く．
// @param[in] assumptions あらかじめ仮定する変数の値割り当てリスト
// @param[out] model 充足するときの値の割り当てを格納する配列．
// @param[out] conflicts 充足不能の場合に原因となっている仮定を入れる配列．
// @retval kSat 充足した．
// @retval kUnsat 充足不能が判明した．
// @retval kUndet わからなかった．
// @note i 番めの変数の割り当て結果は model[i] に入る．
SatBool3
SatSolver::solve(const vector<SatLiteral>& assumptions,
		 vector<SatBool3>& model,
		 vector<SatLiteral>& conflicts)
{
  mLogger->solve(assumptions);

  return mImpl->solve(assumptions, model, conflicts);
}

// @brief 探索を中止する．
//
// 割り込みハンドラや別スレッドから非同期に呼ばれることを仮定している．
void
SatSolver::stop()
{
  mImpl->stop();
}

// @brief 正しい状態のときに true を返す．
bool
SatSolver::sane() const
{
  return mImpl->sane();
}

// @brief 現在の内部状態を得る．
// @param[out] stats 状態を格納する構造体
void
SatSolver::get_stats(SatStats& stats) const
{
  mImpl->get_stats(stats);
}

// @brief 変数の数を得る．
int
SatSolver::variable_num() const
{
  return mImpl->variable_num();
}

// @brief 制約節の数を得る．
int
SatSolver::clause_num() const
{
  return mImpl->clause_num();
}

// @brief 制約節のリテラルの総数を得る．
int
SatSolver::literal_num() const
{
  return mImpl->literal_num();
}

// @brief DIMACS 形式で制約節を出力する．
// @param[in] s 出力先のストリーム
void
SatSolver::write_DIMACS(ostream& s) const
{
  mImpl->write_DIMACS(s);
}

// @brief conflict_limit の最大値
// @param[in] val 設定する値
// @return 以前の設定値を返す．
int
SatSolver::set_max_conflict(int val)
{
  return mImpl->set_max_conflict(val);
}

// @brief solve() 中のリスタートのたびに呼び出されるメッセージハンドラの登録
// @param[in] msg_handler 登録するメッセージハンドラ
void
SatSolver::reg_msg_handler(SatMsgHandler* msg_handler)
{
  mImpl->reg_msg_handler(msg_handler);
}

// @brief 時間計測機能を制御する
void
SatSolver::timer_on(bool enable)
{
  mImpl->timer_on(enable);
}

END_NAMESPACE_YM_SAT
