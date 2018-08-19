/**
 * ListViewPlus.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once
#include "ListItem.h"

//ユーザー定義メッセージ
#define	WM_USER_ENDITEMEDIT		(WM_USER+530)

//---------------------------------------------------------------------------
//
//	編集用コントロールインターフェイス
//
//---------------------------------------------------------------------------
class CItemEditor
{
public:
	virtual ~CItemEditor(){}

	virtual void BeginItemEdit(
		const CRect& rect, IListItem& item, int sub_item)=0;
	virtual void EndItemEdit(IListItem& item, int sub_item) {}

	virtual HWND InitItemEdit(HWND hWndParent)=0;
	virtual operator HWND() const=0;
};

//---------------------------------------------------------------------------
//
//	編集用エディット
//
//---------------------------------------------------------------------------
class CLabelEdit : public CWindowImpl<CLabelEdit, CEdit>, public CItemEditor
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CEdit::GetWndClassName())

	//メッセージマップ
	BEGIN_MSG_MAP(CLabelEdit)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KILLFOCUS(OnKillFocus)
	END_MSG_MAP()

	// ウインドウハンドルを返します
	operator HWND() const
	{
		return m_hWnd;
	}
	// キーダウンイベント
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if ( nChar==VK_ESCAPE || nChar==VK_RETURN )
			GetParent().SetFocus();
		else
			SetMsgHandled(FALSE);
	}

	// フォーカスイベント
	void OnKillFocus(CWindow wndFocus)
	{
		::SendMessage(GetParent(), WM_USER_ENDITEMEDIT, NULL, NULL);
		SetMsgHandled(FALSE);
	}
	// 編集を開始します
	void BeginItemEdit(
		const CRect& rect, IListItem& item, int sub_item)
	{
		//ウインドウ位置とサイズを設定
		MoveWindow(&rect);
		ShowWindow(SW_SHOW);
		SetFocus();

		SetWindowText(item.GetText(sub_item));
		SetSelAll();
	}
	// 編集を終了します
	void EndItemEdit(IListItem& item, int sub_item)
	{
		ATL::CString temp;

		GetWindowText(temp);
		item.SetText(sub_item, (LPCTSTR)temp);

		ShowWindow(SW_HIDE);
	}
	// ウインドウを作成します
	HWND InitItemEdit(HWND hWndParent)
	{
		Create(hWndParent, NULL, _T("LabelEdit"),
			ES_MULTILINE|ES_WANTRETURN|ES_AUTOHSCROLL|ES_AUTOVSCROLL|WS_CHILD|WS_BORDER);
		SetFont( GetParent().GetFont() );

		return GetParent();
	}
};

//---------------------------------------------------------------------------
//
//	編集用コンボボックス
//
//---------------------------------------------------------------------------
class CComboEdit : public CWindowImpl<CComboEdit, CComboBoxEx>, public CItemEditor
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CComboBoxEx::GetWndClassName())

	// メッセージマップ
	BEGIN_MSG_MAP(CComboEdit)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_SELENDCANCEL, OnSelEndCancel)
	END_MSG_MAP()

	// ウインドウハンドルを返します
	operator HWND() const
	{
		return m_hWnd;
	}
	// 編集を開始します
	void BeginItemEdit(const CRect& rect, IListItem& item, int sub_item)
	{
		//ウインドウ位置とサイズを設定
		MoveWindow(&rect);
		ShowWindow(SW_SHOW);
		SetFocus();
		
		// 文字列を検索して、選択します
		if ( GetCount()>0 ) {
			
			int i = FindStringExact(0, item.GetText(sub_item));
			
			if ( i!=CB_ERR )
				SetCurSel(i);
			else
				SetCurSel(0);
		}
	}
	// 編集を終了します
	void EndItemEdit(IListItem& item, int sub_item)
	{
		int i = GetCurSel();
		if ( i==CB_ERR ) return;
		
		// 文字列取得
		ATL::CString strText;
		GetLBText(i, strText);
		item.SetText(sub_item, (LPCTSTR)strText);

		// イメージ番号の取得
		COMBOBOXEXITEM cb_item = { 0 };
		cb_item.iItem = i;
		cb_item.mask = CBEIF_IMAGE;
		GetItem(&cb_item);
		item.SetImage(cb_item.iImage);

		ShowWindow(SW_HIDE);
	}
	// 選択キャンセルイベント
	void OnSelEndCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		if ( IsWindowVisible() ) {
			::SendMessage(GetParent(), WM_USER_ENDITEMEDIT, NULL, NULL);
		}
	}
	// ウインドウを作成します
	HWND InitItemEdit(HWND hWndParent)
	{
		Create(hWndParent, CRect(0, 0, 100, 100), _T("ComboEdit"), CBS_DROPDOWNLIST|WS_CHILD|WS_BORDER|WS_VSCROLL);
		SetFont( GetParent().GetFont() );

		return GetParent();
	}
};

//---------------------------------------------------------------------------
//
//	サブアイテム編集リストビュー
//
//---------------------------------------------------------------------------
template<class TListItem>
class CListViewPlus : public CWindowImpl<CListViewPlus<TListItem>, CListViewCtrl>
{
public:
	typedef	typename std::vector<TListItem>::iterator iterator;
	typedef	typename std::vector<TListItem>::const_iterator const_iterator;

private:
	int		mItem;
	int		mSubItem;
	int		mEditType;
	int		mDragItem;
	int		mDropItem;
	bool	mIsDrag;
	bool	mIsDblClick;
	CPoint	mDragAdj;
	
	CItemEditor*	mCurrentEditor;

	//編集用エディット
	CImageList	mDragImage;
	std::vector<TListItem>		mItems;
	std::map<int,CItemEditor*>	mEditor;

	// エディタのハンドルを取得
	HWND GetEditorHandle()
	{
		return (mCurrentEditor!=NULL) ?
			(HWND)(*mCurrentEditor) : reinterpret_cast<HWND>(INVALID_HANDLE_VALUE);
	}

public:
//	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	//メッセージマップ
	BEGIN_MSG_MAP(CListViewPlus)
		MSG_WM_MOUSEMOVE(OnMouseMove)
		MSG_WM_LBUTTONUP(OnLButtonUp)
		MSG_WM_LBUTTONDBLCLK(OnLButtonDblClick)
		MSG_WM_VSCROLL(OnScroll)
		MSG_WM_HSCROLL(OnScroll)
		MESSAGE_HANDLER_EX(WM_USER_ENDITEMEDIT, OnEndItemEdit)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_GETDISPINFO, OnGetDispInfo)
		REFLECTED_NOTIFY_CODE_HANDLER_EX(LVN_BEGINDRAG, OnBeginDrag)
		REFLECT_NOTIFICATIONS_HWND_FILTERED( GetEditorHandle() )
		DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()

	// スクロール時に編集をキャンセル
	void OnScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar)
	{
		::SendMessage(m_hWnd, WM_USER_ENDITEMEDIT, 0, 0);
		SetMsgHandled(FALSE);
	}
	//マウス移動イベント
	void OnMouseMove(UINT nFlags, CPoint point)
	{
		//ドラッグ中
		if ( mIsDrag ) {

			//カーソル位置からアイテム位置を取得します
			LVHITTESTINFO lvhit;
			lvhit.pt = point;
			SubItemHitTest(&lvhit);

			// 挿入位置のアイテムを強調表示します
			mDragImage.DragShowNolock(FALSE);
			if ( lvhit.iItem!=mDragItem )
				SetItemState(lvhit.iItem, LVIS_DROPHILITED, LVIS_DROPHILITED);
			if ( lvhit.iItem!=mDropItem )
				SetItemState(mDropItem, 0, LVIS_DROPHILITED);
			UpdateWindow();
			mDropItem = lvhit.iItem;
			mDragImage.DragShowNolock(TRUE);

			point.Offset(mDragAdj);
			mDragImage.DragMove(point);
		}
		SetMsgHandled(FALSE);
	}
	//ドラッグ終了を検出
	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		LVHITTESTINFO lvhit;
		ATL::CString str;

		if ( mIsDrag ) {

			//ドラッグ終了処理
			ReleaseCapture();
			mDragImage.DragLeave(m_hWnd);
			mDragImage.EndDrag();
			mDragImage.Destroy();
			
			//カーソル位置からアイテム位置を取得します
			lvhit.pt = point;
			SubItemHitTest(&lvhit);

			// 強調表示を解除します
			SetItemState(mDropItem, 0, LVIS_DROPHILITED);
			mDropItem = -1;

			// アイテムドロップの通知
			EndItemDrop(lvhit.iItem, mDragItem);
			UpdateWindow();
			mIsDrag = false;
		}

		SetMsgHandled(FALSE);
	}
	//ドラッグの開始
	LRESULT OnBeginDrag(LPNMHDR pnmh)
	{
		NMLISTVIEW* pnmlv = reinterpret_cast<NMLISTVIEW*>(pnmh);
		CPoint pt;
		CRect rect;
		
		//有効なアイテムインデックスの場合
		mIsDrag = false;
		if ( pnmlv->iItem>=0 ) {

			//インデックスを取得して、イメージを作成します
			mDragItem = pnmlv->iItem;
			mDragImage = CreateDragImage(pnmlv->iItem, &pnmlv->ptAction);
			
			//イメージのオフセットを計算します
			GetItemRect(mDragItem, &rect, LVIR_BOUNDS);
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			mDragAdj.SetPoint(rect.left - pt.x, rect.top - pt.y);

			//ドラッグ処理を開始します
			if ( !mDragImage.IsNull() ) {

				mDragImage.BeginDrag(0, 0, 0);
				mDragImage.DragEnter(m_hWnd, pnmlv->ptAction);
				SetCapture();
				
				mIsDrag = true;
			}
		}

		return FALSE;
	}
	//表示内容のコールバック
	LRESULT OnGetDispInfo(LPNMHDR pnmh)
	{
		LV_DISPINFO* plvdi = reinterpret_cast<LV_DISPINFO*>(pnmh);
		LVITEM& item(plvdi->item);

		// インデックスのチェック
		if ( (int)mItems.size()>item.iItem ) {

			if ( item.mask & LVIF_TEXT )
				lstrcpyn(item.pszText, mItems[ item.iItem ].GetText(item.iSubItem), item.cchTextMax);
//				item.pszText = mItems[ item.iItem ].GetText(item.iSubItem);

			if ( item.mask & LVIF_IMAGE )
				item.iImage = mItems[ item.iItem ].GetImage();
		}

		return TRUE;
	}
	//ダブルクリックで項目編集開始
	void OnLButtonDblClick(UINT nFlags, CPoint point)
	{
		LVHITTESTINFO lvhit;
		CRect rect, icon;

		//カーソル位置からアイテム位置を取得します
		lvhit.pt = point;
		mItem = SubItemHitTest(&lvhit);
		mSubItem = lvhit.iSubItem;
		if ( mItem<0 || mSubItem<0 ) return;
		
		//エディットの表示位置を取得します
		if ( mSubItem==0 ) {
			GetItemRect(mItem, &rect, LVIR_LABEL);
			
			// アイコンが設定されている場合は、表示領域を拡張
			if ( GetImageList(LVSIL_SMALL)!=NULL ) {
				GetItemRect(mItem, &icon, LVIR_ICON);
				rect |= icon;
			}
		}
		else
			GetSubItemRect(mItem, mSubItem, LVIR_LABEL, &rect);
		
		rect.left += 1;
		rect.bottom -= 1;

		//アイテムのテキストを取得します
		mEditType = mItems[ mItem ].GetEditType( mSubItem );
		std::map<int,CItemEditor*>::iterator it = mEditor.find(mEditType);
		
		if ( it==mEditor.end() )
			mCurrentEditor = NULL;

		else {
			mCurrentEditor = it->second;
			mCurrentEditor->BeginItemEdit(&rect, mItems[ mItem ], mSubItem);
		}

		SetMsgHandled(FALSE);
	}
	//アイテム編集の完了
	LRESULT OnEndItemEdit(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if ( mCurrentEditor!=NULL ) {
			
			// 内容を更新します
			mCurrentEditor->EndItemEdit(mItems[ mItem ], mSubItem);

			// 編集完了を通知します
			EndItemEdit(mItem, mSubItem);
			mEditType = -1;
			mCurrentEditor = NULL;

			// アイテムを再描画します
			RedrawItems(mItem, mItem);
			UpdateWindow();
		}
		return FALSE;
	}
//
//
	//コンストラクタ
	CListViewPlus()
	{
		mIsDrag = false;
		mIsDblClick = false;
		mCurrentEditor = NULL;
		mEditType = -1;
		mDropItem = -1;
	}
	//デストラクタ
	virtual ~CListViewPlus()
	{
		for ( unsigned int i=0; i<mEditor.size(); i++ )
			delete mEditor[i];
	}
//
//
	//アイテムの追加
	TListItem* AddListItem(const TListItem& item)
	{
		mItems.push_back(item);
		CListViewCtrl::SetItemCountEx(mItems.size(), LVSICF_NOINVALIDATEALL);
		return &mItems.back();
	}
	//アイテムの挿入
	TListItem* InsertListItem(const TListItem& item, unsigned int index)
	{
		std::vector<TListItem>::iterator it;
		it = mItems.insert(mItems.begin()+index, item);
		CListViewCtrl::SetItemCountEx(mItems.size(), LVSICF_NOINVALIDATEALL);
		return *it;
	}
	//アイテムを削除します
	void DeleteListItem(unsigned int index)
	{
		mItems.erase(mItems.begin() + index);
		CListViewCtrl::SetItemCountEx(mItems.size(), LVSICF_NOINVALIDATEALL);
	}
	//アイテムを全削除します
	void ClearListItem()
	{
		mItems.clear();
		CListViewCtrl::SetItemCountEx(0, LVSICF_NOINVALIDATEALL);
	}
	//アイテムデータアクセス
	template<class index_type>
	TListItem& operator[](index_type index)
	{
		return mItems[index];
	}
	template<class index_type>
	const TListItem& operator[](index_type index) const
	{
		return mItems[index];
	}
	//アイテムイテレータ
	iterator begin()
	{
		return mItems.begin();
	}
	iterator end()
	{
		return mItems.end();
	}
	const_iterator begin() const
	{
		return mItems.begin();
	}
	const_iterator end() const
	{
		return mItems.end();
	}
	//アイテム数の変更
	BOOL SetItemCount(int nItems)
	{
		mItems.resize(nItems);
		return CListViewCtrl::SetItemCountEx(mItems.size(), LVSICF_NOINVALIDATEALL);
	}
	BOOL SetItemCountEx(int nItems, DWORD dwFlags)
	{
		mItems.resize(nItems);
		return CListViewCtrl::SetItemCountEx(mItems.size(), dwFlags);
	}
//
//
	//エディタの追加
	template<class _Editor>
	_Editor* AddEditor(int edit_type)
	{
		_Editor* edit = new _Editor();
		edit->InitItemEdit(*this);
		DeleteEditor(edit_type);
		mEditor[ edit_type ] = edit;
		return edit;
	}
	//エディタの取得
	template<class _Editor>
	_Editor* GetEditor(int edit_type)
	{
		std::map<int,CItemEditor*> it = mEditor.find(edit_type);
		return ( it!=mEditor.end() ) ? dynamic_cast<_Editor*>(*it) : NULL;
	}
	//エディタの削除
	void DeleteEditor(int edit_type)
	{
		std::map<int,CItemEditor*>::iterator it = mEditor.find(edit_type);
		if ( it!=mEditor.end() ) {
			delete it->second;
			mEditor.erase(it);
		}
	}
	//エディタを全削除します
	void ClearEditor()
	{
		std::map<int,CItemEditor*>::iterator it;
		for ( it=mEditor.begin(); it!=mEditor.end(); it++ )
			delete (*it);
		mEditor.clear();
	}
//
//
	// アイテム編集の完了通知
	virtual void EndItemEdit(int item, int sub_item) {}
	// アイテムドロップの通知
	virtual void EndItemDrop(int dest, int item) {}
};
