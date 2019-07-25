# Do not delete this file
#
# この CMakeLists.txt はダミー
# このファイルがないと UseCython がうまく動かない．

# ===================================================================
# インクルードパスの設定
# ===================================================================
include_directories (
  # *.pxd, *.pxi, *.pyx のあるディレクトリ
  ${YM_SUBMODULE_CYTHON_LIST}
  )