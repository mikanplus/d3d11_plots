#pragma once

#define _USE_MATH_DEFINES

// WTL �w�b�_
#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#include <atlwin.h>
#include <atlcrack.h>
#include <atlmisc.h>
#include <atlframe.h>
#include <atlctrls.h>
#include <atlctrlx.h>
#include <atlstr.h>
#include <atlsplit.h>
#include <atldlgs.h>


// ������֘A�w�b�_
#include <tchar.h>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <regex>
#include <random>
#include <boost/algorithm/string.hpp>
using std::istream;
using std::string;
typedef std::basic_string<TCHAR> tstring;
typedef std::basic_stringstream<TCHAR> tstringstream;
typedef std::basic_ostringstream<TCHAR> tostringstream;
typedef std::basic_istringstream<TCHAR> tistringstream;
typedef std::basic_istream<TCHAR>		tistream;
typedef std::basic_ostream<TCHAR>		tostream;
typedef std::basic_regex<TCHAR>			tregex;


// �R���e�i�֘A�w�b�_
#include <vector>
#include <map>
#include <complex>
#include <memory>


// Ini�t�@�C���T�|�[�g
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/string_path.hpp>
#include <boost/property_tree/ini_parser.hpp>

typedef boost::property_tree::string_path<
	std::string, boost::property_tree::id_translator<std::string> > basic_string_path;

// DirectX�֘A
#include <d3d11.h>

