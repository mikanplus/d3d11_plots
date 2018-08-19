/**
 * ParamInterface.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once

//---------------------------------------------------------------------------
//
//	パラメータインターフェイス
//
//---------------------------------------------------------------------------
class IListItem;
class CMelonUnits;
class IParamInterface
{
protected:
	// 各種コンバータ
	bool GetVariable(IListItem& item, bool& var);
	void SetItem(IListItem& item, bool var);

	bool GetVariable(IListItem& item, float& var);
	bool GetVariable(IListItem& item, double& var);
	void SetItem(IListItem& item, double var);

	bool GetVariable(IListItem& item, std::complex<double>& var);
	void SetItem(IListItem& item, const std::complex<double>& var);

//	bool GetVariable(IListItem& item, Gdiplus::Color& color);
//	void SetItem(IListItem& item, const Gdiplus::Color& color);

	bool GetVariable(IListItem& item, double& var, CMelonUnits& unit);
	void SetItem(IListItem& item, double var, const CMelonUnits& unit, LPCTSTR ext);

public:
	// コンストラクタとデストラクタ
	IParamInterface(){}
	virtual ~IParamInterface(){}
	
	// パラメータの入出力
	virtual void SetParam(IListItem& item) {}
	virtual void GetParam(IListItem& item) {}

	// プロパティの入出力
	virtual void SetProperty(int index, IListItem& item) {}
	virtual void GetProperty(int index, IListItem& item) {}

	// プロパティの数
	virtual int NumProperties() const { return 0; }
};
