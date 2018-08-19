/**
 * MyListView.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once
#include "stdafx.h"
#include "ListViewPlus.h"

#define EDIT_TEXTBOX	0
#define EDIT_COLORDLG	1
#define EDIT_FILEDLG	2

//---------------------------------------------------------------------------
//
//	�v���p�e�B���X�g�r���[
//
//---------------------------------------------------------------------------
class IParamInterface;
class CPropListView :
	public CListViewPlus<CBasicListItem3>,
	public CCustomDraw<CPropListView>
{
private:
	IParamInterface*	mItem;

public:
	DECLARE_WND_SUPERCLASS(NULL, CListViewPlus::GetWndClassName())
	
	BEGIN_MSG_MAP(CPropListView)
		CHAIN_MSG_MAP_ALT(CCustomDraw<CPropListView>, 1)
		CHAIN_MSG_MAP(CListViewPlus<CBasicListItem3>)
	END_MSG_MAP()

	// �R���X�g���N�^�ƃf�X�g���N�^
	CPropListView(){}
	~CPropListView(){}

	// �e��p�����[�^�ƃ����N
	void SetProperties(IParamInterface* item);

	// �A�C�e���ҏW����
	void EndItemEdit(int item, int sub_item);

	// �J�X�^���h���[
	DWORD OnPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd);
	DWORD OnItemPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd);
	DWORD OnSubItemPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd);
};

//---------------------------------------------------------------------------
//
//	�F�I���_�C�A���O
//
//---------------------------------------------------------------------------
class CColorEdit : public CItemEditor
{
private:
	HWND	mEditorParent;
	DWORD	mColor;

public:
	void BeginItemEdit(const CRect& rect, IListItem& item, int sub_item);
	HWND InitItemEdit(HWND hWndParent);
	operator HWND() const { return reinterpret_cast<HWND>(INVALID_HANDLE_VALUE); }
};

//---------------------------------------------------------------------------
//
//	�t�@�C���I���_�C�A���O
//
//---------------------------------------------------------------------------
class CFileEdit : public CItemEditor
{
private:
	HWND	mEditorParent;
	ATL::CString	mFilePath;

public:
	void BeginItemEdit(const CRect& rect, IListItem& item, int sub_item);
	HWND InitItemEdit(HWND hWndParent);
	operator HWND() const { return reinterpret_cast<HWND>(INVALID_HANDLE_VALUE); }
};
