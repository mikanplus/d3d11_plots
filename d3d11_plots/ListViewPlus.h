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

//���[�U�[��`���b�Z�[�W
#define	WM_USER_ENDITEMEDIT		(WM_USER+530)

//---------------------------------------------------------------------------
//
//	�ҏW�p�R���g���[���C���^�[�t�F�C�X
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
//	�ҏW�p�G�f�B�b�g
//
//---------------------------------------------------------------------------
class CLabelEdit : public CWindowImpl<CLabelEdit, CEdit>, public CItemEditor
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CEdit::GetWndClassName())

	//���b�Z�[�W�}�b�v
	BEGIN_MSG_MAP(CLabelEdit)
		MSG_WM_KEYDOWN(OnKeyDown)
		MSG_WM_KILLFOCUS(OnKillFocus)
	END_MSG_MAP()

	// �E�C���h�E�n���h����Ԃ��܂�
	operator HWND() const
	{
		return m_hWnd;
	}
	// �L�[�_�E���C�x���g
	void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
	{
		if ( nChar==VK_ESCAPE || nChar==VK_RETURN )
			GetParent().SetFocus();
		else
			SetMsgHandled(FALSE);
	}

	// �t�H�[�J�X�C�x���g
	void OnKillFocus(CWindow wndFocus)
	{
		::SendMessage(GetParent(), WM_USER_ENDITEMEDIT, NULL, NULL);
		SetMsgHandled(FALSE);
	}
	// �ҏW���J�n���܂�
	void BeginItemEdit(
		const CRect& rect, IListItem& item, int sub_item)
	{
		//�E�C���h�E�ʒu�ƃT�C�Y��ݒ�
		MoveWindow(&rect);
		ShowWindow(SW_SHOW);
		SetFocus();

		SetWindowText(item.GetText(sub_item));
		SetSelAll();
	}
	// �ҏW���I�����܂�
	void EndItemEdit(IListItem& item, int sub_item)
	{
		ATL::CString temp;

		GetWindowText(temp);
		item.SetText(sub_item, (LPCTSTR)temp);

		ShowWindow(SW_HIDE);
	}
	// �E�C���h�E���쐬���܂�
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
//	�ҏW�p�R���{�{�b�N�X
//
//---------------------------------------------------------------------------
class CComboEdit : public CWindowImpl<CComboEdit, CComboBoxEx>, public CItemEditor
{
public:
	DECLARE_WND_SUPERCLASS(NULL, CComboBoxEx::GetWndClassName())

	// ���b�Z�[�W�}�b�v
	BEGIN_MSG_MAP(CComboEdit)
		REFLECTED_COMMAND_CODE_HANDLER_EX(CBN_SELENDCANCEL, OnSelEndCancel)
	END_MSG_MAP()

	// �E�C���h�E�n���h����Ԃ��܂�
	operator HWND() const
	{
		return m_hWnd;
	}
	// �ҏW���J�n���܂�
	void BeginItemEdit(const CRect& rect, IListItem& item, int sub_item)
	{
		//�E�C���h�E�ʒu�ƃT�C�Y��ݒ�
		MoveWindow(&rect);
		ShowWindow(SW_SHOW);
		SetFocus();
		
		// ��������������āA�I�����܂�
		if ( GetCount()>0 ) {
			
			int i = FindStringExact(0, item.GetText(sub_item));
			
			if ( i!=CB_ERR )
				SetCurSel(i);
			else
				SetCurSel(0);
		}
	}
	// �ҏW���I�����܂�
	void EndItemEdit(IListItem& item, int sub_item)
	{
		int i = GetCurSel();
		if ( i==CB_ERR ) return;
		
		// ������擾
		ATL::CString strText;
		GetLBText(i, strText);
		item.SetText(sub_item, (LPCTSTR)strText);

		// �C���[�W�ԍ��̎擾
		COMBOBOXEXITEM cb_item = { 0 };
		cb_item.iItem = i;
		cb_item.mask = CBEIF_IMAGE;
		GetItem(&cb_item);
		item.SetImage(cb_item.iImage);

		ShowWindow(SW_HIDE);
	}
	// �I���L�����Z���C�x���g
	void OnSelEndCancel(UINT uNotifyCode, int nID, CWindow wndCtl)
	{
		if ( IsWindowVisible() ) {
			::SendMessage(GetParent(), WM_USER_ENDITEMEDIT, NULL, NULL);
		}
	}
	// �E�C���h�E���쐬���܂�
	HWND InitItemEdit(HWND hWndParent)
	{
		Create(hWndParent, CRect(0, 0, 100, 100), _T("ComboEdit"), CBS_DROPDOWNLIST|WS_CHILD|WS_BORDER|WS_VSCROLL);
		SetFont( GetParent().GetFont() );

		return GetParent();
	}
};

//---------------------------------------------------------------------------
//
//	�T�u�A�C�e���ҏW���X�g�r���[
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

	//�ҏW�p�G�f�B�b�g
	CImageList	mDragImage;
	std::vector<TListItem>		mItems;
	std::map<int,CItemEditor*>	mEditor;

	// �G�f�B�^�̃n���h�����擾
	HWND GetEditorHandle()
	{
		return (mCurrentEditor!=NULL) ?
			(HWND)(*mCurrentEditor) : reinterpret_cast<HWND>(INVALID_HANDLE_VALUE);
	}

public:
//	DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

	//���b�Z�[�W�}�b�v
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

	// �X�N���[�����ɕҏW���L�����Z��
	void OnScroll(UINT nSBCode, UINT nPos, CScrollBar pScrollBar)
	{
		::SendMessage(m_hWnd, WM_USER_ENDITEMEDIT, 0, 0);
		SetMsgHandled(FALSE);
	}
	//�}�E�X�ړ��C�x���g
	void OnMouseMove(UINT nFlags, CPoint point)
	{
		//�h���b�O��
		if ( mIsDrag ) {

			//�J�[�\���ʒu����A�C�e���ʒu���擾���܂�
			LVHITTESTINFO lvhit;
			lvhit.pt = point;
			SubItemHitTest(&lvhit);

			// �}���ʒu�̃A�C�e���������\�����܂�
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
	//�h���b�O�I�������o
	void OnLButtonUp(UINT nFlags, CPoint point)
	{
		LVHITTESTINFO lvhit;
		ATL::CString str;

		if ( mIsDrag ) {

			//�h���b�O�I������
			ReleaseCapture();
			mDragImage.DragLeave(m_hWnd);
			mDragImage.EndDrag();
			mDragImage.Destroy();
			
			//�J�[�\���ʒu����A�C�e���ʒu���擾���܂�
			lvhit.pt = point;
			SubItemHitTest(&lvhit);

			// �����\�����������܂�
			SetItemState(mDropItem, 0, LVIS_DROPHILITED);
			mDropItem = -1;

			// �A�C�e���h���b�v�̒ʒm
			EndItemDrop(lvhit.iItem, mDragItem);
			UpdateWindow();
			mIsDrag = false;
		}

		SetMsgHandled(FALSE);
	}
	//�h���b�O�̊J�n
	LRESULT OnBeginDrag(LPNMHDR pnmh)
	{
		NMLISTVIEW* pnmlv = reinterpret_cast<NMLISTVIEW*>(pnmh);
		CPoint pt;
		CRect rect;
		
		//�L���ȃA�C�e���C���f�b�N�X�̏ꍇ
		mIsDrag = false;
		if ( pnmlv->iItem>=0 ) {

			//�C���f�b�N�X���擾���āA�C���[�W���쐬���܂�
			mDragItem = pnmlv->iItem;
			mDragImage = CreateDragImage(pnmlv->iItem, &pnmlv->ptAction);
			
			//�C���[�W�̃I�t�Z�b�g���v�Z���܂�
			GetItemRect(mDragItem, &rect, LVIR_BOUNDS);
			GetCursorPos(&pt);
			ScreenToClient(&pt);
			mDragAdj.SetPoint(rect.left - pt.x, rect.top - pt.y);

			//�h���b�O�������J�n���܂�
			if ( !mDragImage.IsNull() ) {

				mDragImage.BeginDrag(0, 0, 0);
				mDragImage.DragEnter(m_hWnd, pnmlv->ptAction);
				SetCapture();
				
				mIsDrag = true;
			}
		}

		return FALSE;
	}
	//�\�����e�̃R�[���o�b�N
	LRESULT OnGetDispInfo(LPNMHDR pnmh)
	{
		LV_DISPINFO* plvdi = reinterpret_cast<LV_DISPINFO*>(pnmh);
		LVITEM& item(plvdi->item);

		// �C���f�b�N�X�̃`�F�b�N
		if ( (int)mItems.size()>item.iItem ) {

			if ( item.mask & LVIF_TEXT )
				lstrcpyn(item.pszText, mItems[ item.iItem ].GetText(item.iSubItem), item.cchTextMax);
//				item.pszText = mItems[ item.iItem ].GetText(item.iSubItem);

			if ( item.mask & LVIF_IMAGE )
				item.iImage = mItems[ item.iItem ].GetImage();
		}

		return TRUE;
	}
	//�_�u���N���b�N�ō��ڕҏW�J�n
	void OnLButtonDblClick(UINT nFlags, CPoint point)
	{
		LVHITTESTINFO lvhit;
		CRect rect, icon;

		//�J�[�\���ʒu����A�C�e���ʒu���擾���܂�
		lvhit.pt = point;
		mItem = SubItemHitTest(&lvhit);
		mSubItem = lvhit.iSubItem;
		if ( mItem<0 || mSubItem<0 ) return;
		
		//�G�f�B�b�g�̕\���ʒu���擾���܂�
		if ( mSubItem==0 ) {
			GetItemRect(mItem, &rect, LVIR_LABEL);
			
			// �A�C�R�����ݒ肳��Ă���ꍇ�́A�\���̈���g��
			if ( GetImageList(LVSIL_SMALL)!=NULL ) {
				GetItemRect(mItem, &icon, LVIR_ICON);
				rect |= icon;
			}
		}
		else
			GetSubItemRect(mItem, mSubItem, LVIR_LABEL, &rect);
		
		rect.left += 1;
		rect.bottom -= 1;

		//�A�C�e���̃e�L�X�g���擾���܂�
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
	//�A�C�e���ҏW�̊���
	LRESULT OnEndItemEdit(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if ( mCurrentEditor!=NULL ) {
			
			// ���e���X�V���܂�
			mCurrentEditor->EndItemEdit(mItems[ mItem ], mSubItem);

			// �ҏW������ʒm���܂�
			EndItemEdit(mItem, mSubItem);
			mEditType = -1;
			mCurrentEditor = NULL;

			// �A�C�e�����ĕ`�悵�܂�
			RedrawItems(mItem, mItem);
			UpdateWindow();
		}
		return FALSE;
	}
//
//
	//�R���X�g���N�^
	CListViewPlus()
	{
		mIsDrag = false;
		mIsDblClick = false;
		mCurrentEditor = NULL;
		mEditType = -1;
		mDropItem = -1;
	}
	//�f�X�g���N�^
	virtual ~CListViewPlus()
	{
		for ( unsigned int i=0; i<mEditor.size(); i++ )
			delete mEditor[i];
	}
//
//
	//�A�C�e���̒ǉ�
	TListItem* AddListItem(const TListItem& item)
	{
		mItems.push_back(item);
		CListViewCtrl::SetItemCountEx(mItems.size(), LVSICF_NOINVALIDATEALL);
		return &mItems.back();
	}
	//�A�C�e���̑}��
	TListItem* InsertListItem(const TListItem& item, unsigned int index)
	{
		std::vector<TListItem>::iterator it;
		it = mItems.insert(mItems.begin()+index, item);
		CListViewCtrl::SetItemCountEx(mItems.size(), LVSICF_NOINVALIDATEALL);
		return *it;
	}
	//�A�C�e�����폜���܂�
	void DeleteListItem(unsigned int index)
	{
		mItems.erase(mItems.begin() + index);
		CListViewCtrl::SetItemCountEx(mItems.size(), LVSICF_NOINVALIDATEALL);
	}
	//�A�C�e����S�폜���܂�
	void ClearListItem()
	{
		mItems.clear();
		CListViewCtrl::SetItemCountEx(0, LVSICF_NOINVALIDATEALL);
	}
	//�A�C�e���f�[�^�A�N�Z�X
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
	//�A�C�e���C�e���[�^
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
	//�A�C�e�����̕ύX
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
	//�G�f�B�^�̒ǉ�
	template<class _Editor>
	_Editor* AddEditor(int edit_type)
	{
		_Editor* edit = new _Editor();
		edit->InitItemEdit(*this);
		DeleteEditor(edit_type);
		mEditor[ edit_type ] = edit;
		return edit;
	}
	//�G�f�B�^�̎擾
	template<class _Editor>
	_Editor* GetEditor(int edit_type)
	{
		std::map<int,CItemEditor*> it = mEditor.find(edit_type);
		return ( it!=mEditor.end() ) ? dynamic_cast<_Editor*>(*it) : NULL;
	}
	//�G�f�B�^�̍폜
	void DeleteEditor(int edit_type)
	{
		std::map<int,CItemEditor*>::iterator it = mEditor.find(edit_type);
		if ( it!=mEditor.end() ) {
			delete it->second;
			mEditor.erase(it);
		}
	}
	//�G�f�B�^��S�폜���܂�
	void ClearEditor()
	{
		std::map<int,CItemEditor*>::iterator it;
		for ( it=mEditor.begin(); it!=mEditor.end(); it++ )
			delete (*it);
		mEditor.clear();
	}
//
//
	// �A�C�e���ҏW�̊����ʒm
	virtual void EndItemEdit(int item, int sub_item) {}
	// �A�C�e���h���b�v�̒ʒm
	virtual void EndItemDrop(int dest, int item) {}
};
