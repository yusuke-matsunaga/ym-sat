
/// @file parse_lits.cc
/// @brief parse_lits の実装ファイル
/// @author Yusuke Matsunaga (松永 裕介)
///
/// Copyright (C) 2022 Yusuke Matsunaga
/// All rights reserved.

// SatLiteral のリストをパースする．
bool
parse_lits(
  PyObject* args,
  vector<SatLiteral>& lit_list
)
{
  if ( PySatLiteral::Check(args) ) {
    auto lit = PySatLiteral::_get_ref(args);
    lit_list.push_back(lit);
    return true;
  }
  if ( PySequence_Check(args) ) {
    auto n = PySequence_Size(args);
    for ( int i = 0; i < n; ++ i ) {
      auto arg = PySequence_GetItem(args, i);
      bool stat = parse_lits(arg, lit_list);
      Py_DECREF(arg);
      if ( !stat ) {
	return false;
      }
    }
    return true;
  }
  PyErr_SetString(PyExc_TypeError, "SatLiteral or sequence of SatLiterals are expected.");
  return false;
}

bool
parse_lit_lits(
  PyObject* args,
  SatLiteral& olit,
  vector<SatLiteral>& lit_list
)
{
  if ( PySequence_Check(args) ) {
    auto n = PySequence_Size(args);
    if ( n <= 1 ) {
      PyErr_SetString(PyExc_TypeError, "SatLiteral and sequence of SatLiterals are expected.");
      return false;
    }
    {
      auto arg0 = PySequence_GetItem(args, 0);
      if ( !PySatLiteral::Check(arg0) ) {
	Py_DECREF(arg0);
	PyErr_SetString(PyExc_TypeError, "1st argument should be a SatLiteral.");
	return false;
      }
      olit = PySatLiteral::_get_ref(arg0);
      Py_DECREF(arg0);
    }
    for ( int i = 1; i < n; ++ i ) {
      auto arg = PySequence_GetItem(args, i);
      bool stat = parse_lits(arg, lit_list);
      Py_DECREF(arg);
      if ( !stat ) {
	return false;
      }
    }
    return true;
  }
  PyErr_SetString(PyExc_TypeError, "SatLiteral or sequence of SatLiterals are expected.");
  return false;
}


bool
parse_int_lits(
  PyObject* args,
  int& k,
  vector<SatLiteral>& lit_list
)
{
  if ( PySequence_Check(args) ) {
    auto n = PySequence_Size(args);
    if ( n <= 1 ) {
      PyErr_SetString(PyExc_TypeError, "an integer and sequence of SatLiterals are expected.");
      return false;
    }
    {
      auto arg0 = PySequence_GetItem(args, 0);
      if ( !PyLong_Check(arg0) ) {
	Py_DECREF(arg0);
	PyErr_SetString(PyExc_TypeError, "1st argument should be an integer.");
	return false;
      }
      k = PyLong_AsLong(arg0);
      Py_DECREF(arg0);
    }
    for ( int i = 1; i < n; ++ i ) {
      auto arg = PySequence_GetItem(args, i);
      bool stat = parse_lits(arg, lit_list);
      Py_DECREF(arg);
      if ( !stat ) {
	return false;
      }
    }
    return true;
  }
  PyErr_SetString(PyExc_TypeError, "an integer and sequence of SatLiterals are expected.");
  return false;
}
