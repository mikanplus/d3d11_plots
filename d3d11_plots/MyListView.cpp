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
//	プロパティリストビュー

// プロパティ変更
void CPropListView::EndItemEdit(int item, int sub_item)
{
	// 有効なターゲットの場合
	if ( mItem!=NULL ) {

		// プロパティを更新します
		mItem->SetProperty(item, (*this)[ (unsigned int)item ]);
	}
}
//---------------------------------------------------------------------------

// 各種パラメータとリンク
void CPropListView::SetProperties(IParamInterface* item)
{
	int i(0);

	// リストをクリアして、プロパティを設定します
	ClearListItem();
	mItem = item;
	if ( item!=NULL ) {

		// プロパティを順番に取得していきます
		CBasicListItem3 temp;
		for ( i=0; i<item->NumProperties(); i++ ) {
			item->GetProperty(i, temp);
			AddListItem(temp);
		}
	}

	// アイテムを再描画します
	Invalidate();
}
//---------------------------------------------------------------------------

// カスタムドロー開始
DWORD CPropListView::OnPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd)
{
	return CDRF_NOTIFYSUBITEMDRAW;
}
//---------------------------------------------------------------------------

// カスタムドローアイテム描画
DWORD CPropListView::OnItemPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd)
{
	return CDRF_NOTIFYSUBITEMDRAW;
}
//---------------------------------------------------------------------------

// カスタムドローサブアイテム描画
DWORD CPropListView::OnSubItemPrePaint(int nID, LPNMCUSTOMDRAW lpnmcd)
{
	DWORD cl;
	unsigned int item, sub_item;
	LPNMLVCUSTOMDRAW lpnmlv = reinterpret_cast<LPNMLVCUSTOMDRAW>(lpnmcd);

	item = lpnmlv->nmcd.dwItemSpec;
	sub_item = lpnmlv->iSubItem;

	if ( item<GetItemCount() ) {

		// 色選択のセルの場合は、背景色を設定します
		if ( (*this)[ item ].GetEditType(sub_item)==EDIT_COLORDLG ) {
			tstringstream tstrm;
			tstrm.str( (*this)[ item ].GetText(sub_item) );
			tstrm >> std::hex >> cl;
			lpnmlv->clrTextBk = cl;
		}

		// 通常のセルは、白背景
		else {
			lpnmlv->clrTextBk = RGB(255, 255, 255);
		}
	}

	return CDRF_DODEFAULT;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//	色選択ダイアログ

// アイテム編集を開始します
void CColorEdit::BeginItemEdit(const CRect& rect, IListItem& item, int sub_item)
{
	CColorDialog dlg(item.GetColor());

	if ( dlg.DoModal()==IDOK ) {
		item.SetColor(dlg.GetColor());
	}

	::SendMessage(mEditorParent, WM_USER_ENDITEMEDIT, NULL, NULL);
}
//---------------------------------------------------------------------------

// ダイアログを初期化します
HWND CColorEdit::InitItemEdit(HWND hWndParent)
{
	mEditorParent = hWndParent;
	return hWndParent;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
//	ファイル選択ダイアログ

// アイテム編集を開始、ファイル選択ダイアログを開きます
void CFileEdit::BeginItemEdit(const CRect& rect, IListItem& item, int sub_item)
{
	// ダイアログを生成
	CFileDialog dlg(TRUE, _T("s*p"), NULL, OFN_HIDEREADONLY | OFN_CREATEPROMPT,
		_T("Touch stone file (*.snp)\0*.s*p\0すべてのファイル (*.*)\0*.*\0\0"));

	if ( dlg.DoModal()==IDOK ) {
		item.SetText(sub_item, dlg.m_szFileName);
	}

	::SendMessage(mEditorParent, WM_USER_ENDITEMEDIT, NULL, NULL);
}

// ダイアログの初期化
HWND CFileEdit::InitItemEdit(HWND hWndParent)
{
	mEditorParent = hWndParent;
	return hWndParent;
}
