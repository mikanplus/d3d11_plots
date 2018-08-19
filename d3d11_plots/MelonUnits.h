/**
 * MelonUnits.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once

//---------------------------------------------------------------------------
//
//	�P�ʑ��ݕϊ��N���X
//
//---------------------------------------------------------------------------
class CMelonUnits
{
private:

	double	mUnit;
	char	mInitial;

public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	CMelonUnits();
	CMelonUnits(char initial);
	virtual ~CMelonUnits();

	double operator()(double val) const { return val / mUnit; }
	void AutoUnit(double dest);
	void SetUnit(LPCTSTR pstr);
	void SetUnit(char unit);
	double GetUnit() const { return mUnit; }

	char GetInitial() const { return mInitial; }
};
