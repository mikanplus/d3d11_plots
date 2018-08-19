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

// ���l���擾���܂�
bool IParamInterface::GetVariable(IListItem& item, bool& var)
{
	tistringstream is( item.GetText(1) );
	is >> var;
	return !is.fail();
}
//---------------------------------------------------------------------------

// ���X�g�A�C�e���ɐ��l��ݒ肵�܂�
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

// ���l���擾���܂�
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

// ���X�g�A�C�e���ɐ��l��ݒ肵�܂�
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

// ���f���l���擾���܂�
bool IParamInterface::GetVariable(IListItem& item, std::complex<double>& var)
{
	double real(0.0), imag(0.0);
	tistringstream is( item.GetText(1) );
	is >> real;
	if ( is.fail() ) return false;

	// �����̓X�y�[�X��؂�
	is >> imag;

	// �����ǂݎ�莸�s��or�����̏ꍇ�́A�����l��0.0����������
	var.real(real);
	var.imag(imag);
	return true;
}
//---------------------------------------------------------------------------

// ���X�g�A�C�e���ɕ��f���l��ݒ肵�܂�
void IParamInterface::SetItem(IListItem& item, const std::complex<double>& var)
{
	tostringstream os;

	// ������0�̏ꍇ�́A�����݂̂��o�͂��܂�
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
// �F���擾���܂�
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

// ���X�g�A�C�e���ɐF��ݒ肵�܂�
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
// �P�ʕt���Ő��l���擾���܂�
bool IParamInterface::GetVariable(IListItem& item, double& var, CMelonUnits& unit)
{
	double d;
	tistringstream is( item.GetText(1) );
	tstring buff;

	is >> d;
	if ( !is.fail() ) {

		is >> buff;
		boost::algorithm::trim(buff);
		
		// �P�ʂ����l�ɋL�q����Ă���΁A�P�ʍX�V
		if ( !buff.empty() )
			unit.SetUnit( buff.c_str() );

		// �P�ʂ��L�q����Ă��Ȃ���΁A�Z�������g�p
		else
			unit.SetUnit( item.GetText(2) );
		
		// ���g�������X�V
		var = d * unit.GetUnit();
	}

	return !is.fail();
}
//---------------------------------------------------------------------------

// ���X�g�A�C�e���ɒP�ʕt���Ő��l��ݒ肵�܂�
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
