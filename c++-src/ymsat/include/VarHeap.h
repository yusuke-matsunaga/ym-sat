#ifndef VARHEAP_H
#define VARHEAP_H

/// @file VarHeap.h
/// @brief VarHeap のヘッダファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2005-2011, 2014, 2016, 2018, 2023 Yusuke Matsunaga
/// All rights reserved.

#include "ym/sat.h"


#define DOUT cout

BEGIN_NAMESPACE_YM_SAT

BEGIN_NONAMESPACE

static const bool debug_varheap = false;

END_NONAMESPACE

//////////////////////////////////////////////////////////////////////
/// @class VarHeap VarHeap.h "VarHeap.h"
/// @brief 変数のヒープ木
///
/// mActivity[] の値に基づいて整列させる．
//////////////////////////////////////////////////////////////////////
class VarHeap
{
public:

  /// @brief コンストラクタ
  VarHeap();

  /// @brief デストラクタ
  ~VarHeap();


public:
  //////////////////////////////////////////////////////////////////////
  // 外部インターフェイス
  //////////////////////////////////////////////////////////////////////

  /// @brief アクティビティの低減率を設定する．
  void
  set_decay(
    double decay ///< [in] 低減率
  )
  {
    mVarDecay = decay;
  }

  /// @brief 変数のアクティビティを増加させる．
  void
  bump_var_activity(
    SatVarId var ///< [in] 変数番号
  )
  {
    if ( debug_varheap ) {
      DOUT << "VarHeap::bump_var_activity(" << var << ")" << endl;
    }
    double& act = mActivity[var];
    act += mVarBump;
    if ( act > 1e+100 ) {
      // アクティビティがオーバーフローしたので低減率を増やす．
      for ( SizeType var1 = 0; var1 < mActivity.size(); ++ var1 ) {
	mActivity[var1] *= 1e-100;
      }
      mVarBump *= 1e-100;
    }
    auto pos = get_pos(var);
    // pos != -1 と pos != 0 を同時に行うための hack
    if ( pos > 0 ) {
      move_up(pos);
    }
  }

  /// @brief 変数のアクティビティを定率で減少させる．
  void
  decay_var_activity()
  {
    if ( debug_varheap ) {
      DOUT << "VarHeap::decay_var_activity()" << endl;
    }
    mVarBump *= (1 / mVarDecay);
  }

  /// @brief 空にする．
  void
  clear()
  {
    mHeapNum = 0;
  }

  /// @brief size 個の要素を格納出来るだけの領域を確保する．
  void
  alloc_var(
    SizeType size ///< [in] 必要なサイズ
  );

  /// @brief ヒープの要素数を返す．
  SizeType
  size() const
  {
    return mHeapNum;
  }

  /// @brief 要素が空の時 true を返す．
  bool
  empty() const
  {
    return size() == 0;
  }

  /// @brief ヒープの要素を取り出す．
  SatVarId
  get(
    SizeType pos ///< [in] 位置 ( 0 <= pos < size() )
  ) const
  {
    if ( pos < 0 || size() <= pos ) {
      throw std::out_of_range{"pos is out of range"};
    }
    return mHeap[pos];
  }


  /// @brief 変数をヒープに追加する．
  void
  add_var(
    SatVarId var ///< [in] 追加する変数
  )
  {
    _add_var(var);
    mActivity[var] = 0.0;
  }

  /// @brief 変数を再びヒープに追加する．
  void
  push(
    SatVarId var ///< [in] 追加する変数
  )
  {
    if ( mHeapPos[var] == -1 ) {
      auto pos = _add_var(var);
      move_up(pos);
    }
  }

  /// @brief アクティビティ最大の変数番号を取り出す．
  ///
  /// 該当の変数はヒープから取り除かれる．
  SatVarId
  pop_top()
  {
    auto var = mHeap[0];
    unset(var);
    -- mHeapNum;
    if ( mHeapNum > 0 ) {
      auto var1 = mHeap[mHeapNum];
      set(var1, 0);
      move_down(0);
    }
    return var;
  }

  /// @brief 変数のアクティビティを返す．
  double
  activity(
    SatVarId var ///< [in] 対象の変数
  ) const
  {
    return mActivity[var];
  }

  /// @brief 変数のアクティビティを初期化する．
  void
  reset_activity();

  /// @brief 与えられた変数のリストからヒープ木を構成する．
  void
  build(
    const vector<SatVarId>& var_list
  );

  /// @brief 内容を出力する
  void
  dump(
    ostream& s ///< [in] 出力先のストリーム
  ) const;


private:
  //////////////////////////////////////////////////////////////////////
  // 内部で用いられる関数
  //////////////////////////////////////////////////////////////////////

  /// @brief 変数をヒープの末尾に追加する．
  /// @return 追加した位置を返す．
  SizeType
  _add_var(
    SatVarId var ///< [in] 追加する変数
  )
  {
    auto pos = mHeapNum;
    ++ mHeapNum;
    set(var, pos);
    return pos;
  }

  /// @brief 引数の位置にある要素を適当な位置まで沈めてゆく
  void
  move_down(
    SizeType pos ///< [in] 対象の要素の位置
  )
  {
    auto var_p = mHeap[pos];
    auto val_p = mActivity[var_p];
    for ( ; ; ) {
      // ヒープ木の性質から親から子の位置がわかる
      auto pos_l = left(pos);
      auto pos_r = pos_l + 1;
      if ( pos_r > mHeapNum ) {
	// 左右の子どもを持たない場合
	break;
      }
      // 左右の子供のうちアクティビティの大きい方を pos_c とする．
      // 同点なら左を選ぶ．
      auto pos_c = pos_l;
      auto var_c = mHeap[pos_c];
      auto val_c = mActivity[var_c];
      if ( pos_r < mHeapNum ) {
	auto var_r = mHeap[pos_r];
	auto val_r = mActivity[var_r];
	if ( val_c < val_r ) {
	  pos_c = pos_r;
	  var_c = var_r;
	  val_c = val_r;
	}
      }
      // 子供のアクティビティが親を上回らなければ終わり
      if ( val_c <= val_p ) {
	break;
      }
      // 逆転
      set(var_p, pos_c);
      set(var_c, pos);
      pos = pos_c;
    }
  }

  /// @brief 引数の位置にある要素を適当な位置まで上げてゆく
  void
  move_up(
    SizeType pos ///< [in] 対象の要素の位置
  )
  {
    auto var = mHeap[pos];
    auto val = mActivity[var];
    while ( pos > 0 ) {
      auto pos_p = parent(pos);
      auto var_p = mHeap[pos_p];
      auto val_p = mActivity[var_p];
      if ( val_p >= val ) {
	break;
      }
      set(var, pos_p);
      set(var_p, pos);
      pos = pos_p;
    }
  }

  /// @brief 変数を配列にセットする．
  ///
  /// mHeap と mHeapPos の一貫性を保つためにはこの関数を使うこと．
  void
  set(
    SatVarId var, ///< [in] 対象の変数番号
    SizeType pos  ///< [in] 位置
  )
  {
    mHeap[pos] = var;
    mHeapPos[var] = pos;
  }

  /// @brief 変数を取り除く．
  void
  unset(
    SatVarId var ///< [in] 対象の変数番号
  )
  {
    mHeapPos[var] = -1;
  }

  /// @brief 変数の場所を取り出す．
  int
  get_pos(
    SatVarId var ///< [in] 対象の変数番号
  )
  {
    return mHeapPos[var];
  }

  /// @brief 左の子供の位置を計算する
  static
  SizeType
  left(
    SizeType pos ///< [in] 親の位置
  )
  {
    return pos + pos + 1;
  }

  /// @brief 右の子供の位置を計算する．
  static
  SizeType
  right(
    SizeType pos ///< [in] 親の位置
  )
  {
    return pos + pos + 2;
  }

  /// @brief 親の位置を計算する．
  ///
  /// 左の子供でも右の子供でも同じ
  static
  SizeType
  parent(
    SizeType pos ///< [in] 子供の位置
  )
  {
    return (pos - 1) >> 1;
  }


private:
  //////////////////////////////////////////////////////////////////////
  // データメンバ
  //////////////////////////////////////////////////////////////////////

  // 変数のアクティビティの増加量
  double mVarBump{1.0};

  // 変数のアクティビティの減衰量
  double mVarDecay{0.95};

  // ヒープ上の位置の配列
  // キーは変数番号
  // ヒープ上にない場合は -1 となる．
  vector<int> mHeapPos;

  // アクティビティ
  // キーは変数番号
  vector<double> mActivity;

  // ヒープ用の配列
  vector<SatVarId> mHeap;

  // ヒープの要素数
  SizeType mHeapNum{0};

};

END_NAMESPACE_YM_SAT

#endif // VARHEAP_H
