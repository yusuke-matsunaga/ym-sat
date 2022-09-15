#ifndef YM_SATDIMACS_H
#define YM_SATDIMACS_H

/// @file ym/SatDimacs.h
/// @brief SatDimacs のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2018, 2019, 2022 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"


BEGIN_NAMESPACE_YM_SAT

//////////////////////////////////////////////////////////////////////
/// @class SatDimacs SatDimacs.h "ym/SatDimacs.h"
/// @brief DIMCAS 形式のファイルの内容を表すクラス
///
/// SatSolver とは直接関係はない．
//////////////////////////////////////////////////////////////////////
class SatDimacs
{
public:

  /// @brief コンストラクタ
  ///
  /// 空のCNFとなる．
  SatDimacs() = default;

  /// @brief デストラクタ
  ~SatDimacs() = default;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を取得する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数の数を返す．
  ///
  /// 必ずしも全ての変数が節の中で用いられているとは限らない．
  SizeType
  variable_num() const
  {
    return mVarNum;
  }

  /// @brief 節の数を返す．
  SizeType
  clause_num() const
  {
    return mClauseList.size();
  }

  /// @brief 節のリストを返す．
  ///
  /// 節は DIMACS 形式と同様に int のリストで表す．
  const vector<vector<int>>&
  clause_list() const
  {
    return mClauseList;
  }

  /// @brief 評価する．
  /// @retval true 充足した．
  /// @retval false 充足しなかった．
  ///
  /// model[i] に i 番目の変数の値 ( 0 or 1 ) を入れる．
  bool
  eval(
    const vector<int>& model ///< [in] モデル
  ) const;

  /// @brief 内容を DIMACS 形式で出力する．
  void
  write_dimacs(
    ostream& s ///< [in] 出力先のストリーム
  ) const;


public:
  //////////////////////////////////////////////////////////////////////
  // 内容を設定する関数
  //////////////////////////////////////////////////////////////////////

  /// @brief クリアする．
  ///
  /// 節を持たない状態になる．
  void
  clear()
  {
    mVarNum = 0;
    mClauseList.clear();
  }

  /// @brief 節を追加する．
  ///
  /// 正の整数が正のリテラル，負の整数が負のリテラルを表す．
  /// 変数番号は 0 から始まる．
  /// 例えば
  /// * 0 の正のリテラル -> 1
  /// * 1 の負のリテラル -> -2
  /// となる．
  void
  add_clause(
    const vector<int>& lit_list ///< [in] 節を表す int のリスト
  )
  {
    // 変数番号が mVarNum よりも大きければ更新する．
    for ( auto lit: lit_list ) {
      int var;
      int pol;
      tie(var, pol) = decode_lit(lit);
      ++ var;
      if ( mVarNum < var ) {
	mVarNum = var;
      }
    }

    // あとは lit_list を追加するだけ
    mClauseList.push_back(lit_list);
  }

  /// @brief DIMACS 形式のファイルを読んで SatDimacs に設定する．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read_dimacs(
    istream& s ///< [in] 入力元のストリーム
  );

  /// @brief DIMACS 形式のファイルを読んで SatDimacs に設定する．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read_dimacs(
    const char* filename ///< [in] ファイル名
  )
  {
    ifstream s(filename);
    if ( !s ) {
      return false;
    }

    return read_dimacs(s);
  }

  /// @brief DIMACS 形式のファイルを読んで SatDimacs に設定する．
  /// @retval true 読み込みが成功した．
  /// @retval false 読み込みが失敗した．
  bool
  read_dimacs(
    const string& filename ///< [in] ファイル名
  )
  {
    ifstream s(filename);
    if ( !s ) {
      return false;
    }

    return read_dimacs(s);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief int から変数番号と極性(0 or 1)を取り出す．
  /// @param[in] var 変数番号 ( 0 から始まる )
  /// @param[in] pol 極性 ( 0 or 1 )
  static
  tuple<int, int>
  decode_lit(
    int lit ///< [in] リテラルを表す整数
  )
  {
    ASSERT_COND( lit != 0 );

    int var;
    int pol;
    if ( lit > 0 ) {
      var = lit - 1;
      pol = 1;
    }
    else {
      var = - lit - 1;
      pol = 0;
    }

    return make_tuple(var, pol);
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数の数
  SizeType mVarNum{0};

  // 節のリスト
  vector<vector<int>> mClauseList;

};

END_NAMESPACE_YM_SAT

#endif // YM_SATDIMACS_H
