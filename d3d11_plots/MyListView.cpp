/**
 * MyListView.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "MyListView.h"
#include "ParamInterface.h"

//---------------------------------------------------------------------------
//	�v���p�e�B���X�g�r���[

// �v���p�e�B�ύX
void CPropListView::EndItemEdit(int item, int sub_item)
{
	// �L���ȃ^�[�Q�b�g�̏ꍇ
	if ( mItem!=NULL ) {

		// �v���p�e�B���X�V���܂�
		mItem->SetProperty(item, (*this)[ (unsigned int)item ]);
	}
}
//---------------------------------------------------------------------------

// �e��p�����[�^�ƃ����N
void CPropListView::SetProperties(IParamInterface* item)
{
	int i(0);

	// ���X�g���N���A���āA�v���p�e�B��ݒ肵�܂�
	ClearListItem();
	mItem = item;
	if ( item!=NULL ) {

		// �v���p�e�B�����ԂɎ擾���Ă����܂�
		CBasicListItem3 temp;
		for ( i=0; i<item->NumProperties(); i++ ) {
			item->GetProperty(i, temp);
			AddListItem(temp);
		}
	}

	// �A�C�e�����ĕ`�悵�܂�
	Invalidate();
}
//---------------------------------------------------------------------------

// �J�X�^���h���[�J�n
DWORD CPropListView::OnPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd)
{
	return CDRF_NOTIFYSUBITEMDRAW;
}
//---------------------------------------------------------------------------

// �J�X�^���h���[�A�C�e���`��
DWORD CPropListView::OnItemPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd)
{
	return CDRF_NOTIFYSUBITEMDRAW;
}
//---------------------------------------------------------------------------

// �J�X�^���h���[�T�u�A�C�e���`��
DWORD CPropListView::OnSubItemPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd)
{
	DWORD cl;
	unsigned int item, sub_item;
	LPNMLVCUSTOMDRAW lpnmlv = reinterpret_cast<LPNMLVCUSTOMDRAW>(lpnmcd);

	item = lpnmlv->nmcd.dwItemSpec;
	sub_item = lpnmlv->iSubItem;

	if ( item<GetItemCount() ) {

		// �F�I���̃Z���̏ꍇ�́A�w�i�F��ݒ肵�܂�
		if ( (*this)[ item ].GetEditType(sub_item)==EDIT_COLORDLG ) {
			tstringstream tstrm;
			tstrm.str( (*this)[ item ].GetText(sub_item) );
			tstrm >> std::hex >> cl;
			lpnmlv->clrTextBk = cl;
		}

		// �ʏ�̃Z���́A���w�i
		else {
			lpnmlv->clrTextBk = RGB(255, 255, 255);
		}
	}

	return CDRF_DODEFAULT;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//	�F�I���_�C�A���O

// �A�C�e���ҏW���J�n���܂�
void CColorEdit::BeginItemEdit(const CRect& rect, IListItem& item, int sub_item)
{
	CColorDialog dlg(item.GetColor());

	if ( dlg.DoModal()==IDOK ) {
		item.SetColor(dlg.GetColor());
	}

	::SendMessage(mEditorParent, WM_USER_ENDITEMEDIT, NULL, NULL);
}
//---------------------------------------------------------------------------

// �_�C�A���O�����������܂�
HWND CColorEdit::InitItemEdit(HWND hWndParent)
{
	mEditorParent = hWndParent;
	return hWndParent;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//	�t�@�C���I���_�C�A���O

// �A�C�e���ҏW���J�n�A�t�@�C���I���_�C�A���O���J���܂�
void CFileEdit::BeginItemEdit(const CRect& rect, IListItem& item, int sub_item)
{
	// �_�C�A���O�𐶐�
	CFileDialog dlg(TRUE, _T("s*p"), NULL, OFN_HIDEREADONLY | OFN_CREATEPROMPT,
		_T("Touch stone file (*.snp)\0*.s*p\0���ׂẴt�@�C�� (*.*)\0*.*\0\0"));

	if ( dlg.DoModal()==IDOK ) {
		item.SetText(sub_item, dlg.m_szFileName);
	}

	::SendMessage(mEditorParent, WM_USER_ENDITEMEDIT, NULL, NULL);
}

// �_�C�A���O�̏�����
HWND CFileEdit::InitItemEdit(HWND hWndParent)
{
	mEditorParent = hWndParent;
	return hWndParent;
}
