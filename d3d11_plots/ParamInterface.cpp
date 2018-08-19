/**
 * ParamInterface.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "ParamInterface.h"
#include "ListItem.h"
#include "MyListView.h"
#include "MelonUnits.h"

// 数値を取得します
bool IParamInterface::GetVariable(IListItem& item, bool& var)
{
	tistringstream is( item.GetText(1) );
	is >> var;
	return !is.fail();
}
//---------------------------------------------------------------------------

// リストアイテムに数値を設定します
void IParamInterface::SetItem(IListItem& item, bool var)
{
	tostringstream os;
	os << var;
	item.SetText(1, os.str().c_str());
	item.SetText(2, _T(""));
	
	item.SetEditType(0, EDIT_DISABLED);
	item.SetEditType(1, EDIT_TEXTBOX);
	item.SetEditType(2, EDIT_DISABLED);
}
//---------------------------------------------------------------------------

// 数値を取得します
bool IParamInterface::GetVariable(IListItem& item, float& var)
{
	tistringstream is( item.GetText(1) );
	is >> var;
	return !is.fail();
}
//---------------------------------------------------------------------------
bool IParamInterface::GetVariable(IListItem& item, double& var)
{
	tistringstream is( item.GetText(1) );
	is >> var;
	return !is.fail();
}
//---------------------------------------------------------------------------

// リストアイテムに数値を設定します
void IParamInterface::SetItem(IListItem& item, double var)
{
	tostringstream os;
	os << var;
	item.SetText(1, os.str().c_str());
	item.SetText(2, _T(""));
	
	item.SetEditType(0, EDIT_DISABLED);
	item.SetEditType(1, EDIT_TEXTBOX);
	item.SetEditType(2, EDIT_DISABLED);
}
//---------------------------------------------------------------------------

// 複素数値を取得します
bool IParamInterface::GetVariable(IListItem& item, std::complex<double>& var)
{
	double real(0.0), imag(0.0);
	tistringstream is( item.GetText(1) );
	is >> real;
	if ( is.fail() ) return false;

	// 虚部はスペース区切り
	is >> imag;

	// 虚部読み取り失敗時or実数の場合は、初期値の0.0が代入される
	var.real(real);
	var.imag(imag);
	return true;
}
//---------------------------------------------------------------------------

// リストアイテムに複素数値を設定します
void IParamInterface::SetItem(IListItem& item, const std::complex<double>& var)
{
	tostringstream os;

	// 虚部が0の場合は、実部のみを出力します
	os << var.real();
	if ( abs(var.imag())>std::numeric_limits<double>::epsilon() ) {
		os << _T(" ") << var.imag();
	}
	item.SetText(1, os.str().c_str());
	item.SetText(2, _T(""));
	
	item.SetEditType(0, EDIT_DISABLED);
	item.SetEditType(1, EDIT_TEXTBOX);
	item.SetEditType(2, EDIT_DISABLED);
}
//---------------------------------------------------------------------------
/*
// 色を取得します
bool IParamInterface::GetVariable(IListItem& item, Gdiplus::Color& color)
{
	DWORD ref;
	tistringstream is( item.GetText(1) );
	
	is >> std::hex >> ref;
	if ( !is.fail() )
		color.SetFromCOLORREF(ref);

	return !is.fail();
}
//---------------------------------------------------------------------------

// リストアイテムに色を設定します
void IParamInterface::SetItem(IListItem& item, const Gdiplus::Color& color)
{
	tostringstream os;
	os << std::hex << std::showbase << color.ToCOLORREF();
	item.SetText(1, os.str().c_str());
	item.SetText(2, _T(""));

	item.SetEditType(0, EDIT_DISABLED);
	item.SetEditType(1, EDIT_COLORDLG);
	item.SetEditType(2, EDIT_DISABLED);
}
//---------------------------------------------------------------------------
*/
// 単位付きで数値を取得します
bool IParamInterface::GetVariable(IListItem& item, double& var, CMelonUnits& unit)
{
	double d;
	tistringstream is( item.GetText(1) );
	tstring buff;

	is >> d;
	if ( !is.fail() ) {

		is >> buff;
		boost::algorithm::trim(buff);
		
		// 単位が数値に記述されていれば、単位更新
		if ( !buff.empty() )
			unit.SetUnit( buff.c_str() );

		// 単位が記述されていなければ、セル情報を使用
		else
			unit.SetUnit( item.GetText(2) );
		
		// 周波数情報を更新
		var = d * unit.GetUnit();
	}

	return !is.fail();
}
//---------------------------------------------------------------------------

// リストアイテムに単位付きで数値を設定します
void IParamInterface::SetItem(IListItem& item, double var, const CMelonUnits& unit, LPCTSTR ext)
{
	tostringstream os;
	os << unit( var );
	item.SetText(1, os.str().c_str());
	
	os.str(_T(""));
	os << unit.GetInitial() << ext;
	item.SetText(2, os.str().c_str());
	
	item.SetEditType(0, EDIT_DISABLED);
	item.SetEditType(1, EDIT_TEXTBOX);
	item.SetEditType(2, EDIT_TEXTBOX);
}
//---------------------------------------------------------------------------
