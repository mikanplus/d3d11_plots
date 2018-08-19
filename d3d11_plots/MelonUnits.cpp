/**
 * MelonUnits.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "MelonUnits.h"

// コンストラクタ
CMelonUnits::CMelonUnits(char initial)
{
	SetUnit(initial);
}
//---------------------------------------------------------------------------

// コンストラクタ
CMelonUnits::CMelonUnits()
{
	mUnit = 1.0;
	mInitial = ' ';
}
//---------------------------------------------------------------------------

// デストラクタ
CMelonUnits::~CMelonUnits()
{
}
//---------------------------------------------------------------------------

// 文字列からレンジを設定します
void CMelonUnits::SetUnit(LPCTSTR pstr)
{
	using namespace boost;

	tstring tstr(pstr);
	algorithm::trim_left(tstr);
	SetUnit( (char)tstr[0] );
}
//---------------------------------------------------------------------------

// イニシャルからレンジを設定します
void CMelonUnits::SetUnit(char unit)
{
	switch ( unit ) {
		case 'f':
			mInitial = 'f';
			mUnit = 1e-15;
			break;

		case 'p':
			mInitial = 'p';
			mUnit = 1e-12;
			break;

		case 'n':
			mInitial = 'n';
			mUnit = 1e-9;
			break;

		case 'u':
			mInitial = 'u';
			mUnit = 1e-6;
			break;

		case 'm':
			mInitial = 'm';
			mUnit = 1e-3;
			break;

		case 'k': case 'K':
			mInitial = 'k';
			mUnit = 1e3;
			break;

		case 'M':
			mInitial = 'M';
			mUnit = 1e6;
			break;

		case 'G':
			mInitial = 'G';
			mUnit = 1e9;
			break;

		case 'T':
			mInitial = 'T';
			mUnit = 1e12;
			break;

		default:
			mInitial = '\0';
			mUnit = 1.0;
			break;
	}
}
//---------------------------------------------------------------------------

// レンジを自動設定します
void CMelonUnits::AutoUnit(double dest)
{
	if ( dest<1e-12 ) {
		mInitial = 'f';
		mUnit = 1e-15;
	}
	else if ( dest<1e-9 ) {
		mInitial = 'p';
		mUnit = 1e-12;
	}
	else if ( dest<1e-6 ) {
		mInitial = 'n';
		mUnit = 1e-9;
	}
	else if ( dest<1e-3 ) {
		mInitial = 'u';
		mUnit = 1e-6;
	}
	else if ( dest<1.0 ) {
		mInitial = 'm';
		mUnit = 1e-3;
	}
	else if ( dest<1e3 ) {
		mInitial = '\0';
		mUnit = 1.0;
	}
	else if ( dest<1e6 ) {
		mInitial = 'k';
		mUnit = 1e3;
	}
	else if ( dest<1e9 ) {
		mInitial = 'M';
		mUnit = 1e6;
	}
	else if ( dest<1e12 ) {
		mInitial = 'G';
		mUnit = 1e9;
	}
	else {
		mInitial = 'T';
		mUnit = 1e12;
	}
}
//---------------------------------------------------------------------------
