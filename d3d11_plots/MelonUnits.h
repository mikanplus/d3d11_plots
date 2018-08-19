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
//	単位相互変換クラス
//
//---------------------------------------------------------------------------
class CMelonUnits
{
private:

	double	mUnit;
	char	mInitial;

public:
	// コンストラクタとデストラクタ
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
