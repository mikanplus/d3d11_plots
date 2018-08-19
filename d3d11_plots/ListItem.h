/**
 * ListItem.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once

//---------------------------------------------------------------------------
//
//	CListViewPlus�p�A�C�e���C���^�[�t�F�C�X
//
//---------------------------------------------------------------------------
const int EDIT_DISABLED	= -1;
class IListItem
{
public:
	//�f�X�g���N�^
	virtual ~IListItem(){}

	//�e��A�N�Z�X���\�b�h
	virtual LPCTSTR GetText(int col) const { return NULL; }
	virtual void SetText(int col, LPCTSTR tstr) {}

	virtual int GetImage() const { return -1; }
	virtual void SetImage(int img) {}

	virtual int GetEditType(int col) const { return EDIT_DISABLED; }
	virtual void SetEditType(int col, int type) {}

	virtual DWORD GetColor() const	{ return RGB(255,255,255); }
	virtual void SetColor(DWORD color) {}
};

//---------------------------------------------------------------------------
//
//	CListViewPlus�W���A�C�e��
//
//---------------------------------------------------------------------------
template<unsigned int _Column, int _DefEditType=EDIT_DISABLED>
class CBasicListItem : public IListItem
{
private:
	int		mImage;
	int		mEditType[_Column];
	DWORD	mColor;

	ATL::CString	mText[_Column];

public:
	// �R���X�g���N�^
	CBasicListItem()
	{
		mImage = -1;
		std::fill(mEditType, mEditType+_Column, _DefEditType);
	}

	// �e�L�X�g�̎擾
	LPCTSTR GetText(int col) const
	{
		return (col>=0 && col<_Column) ? mText[col] : (LPCTSTR)NULL;
	}
	// �e�L�X�g�̐ݒ�
	void SetText(int col, LPCTSTR tstr)
	{
		if (col>=0 && col<_Column)
			mText[col] = tstr;
	}

	// �C���[�W�̎擾
	int GetImage() const { return mImage; }
	// �C���[�W�̐ݒ�
	void SetImage(int img) { mImage = img; }
	// �ҏW���@�̎擾
	int GetEditType(int col) const
	{
		return (col>=0 && col<_Column) ? mEditType[col] : EDIT_DISABLED;
	}
	// �ҏW���@�̐ݒ�
	void SetEditType(int col, int type)
	{
		if ( col>=0 && col<_Column )
			mEditType[col] = type;
	}

	// �F�̐ݒ�
	DWORD GetColor() const		{ return mColor; }
	void SetColor(DWORD color)	{ mColor=color;  }
};

typedef CBasicListItem<2>	CBasicListItem2;
typedef CBasicListItem<3>	CBasicListItem3;
typedef CBasicListItem<4>	CBasicListItem4;
