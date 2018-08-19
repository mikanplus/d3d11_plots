/**
 * MainFrame.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once
#include "resource.h"
#include "d3d11_helper.h"
#include "ascii_string.h"
#include "stencil_helper.h"
#include "d3d11_plots.h"
#include "MyListView.h"
#include "PlotView.h"

//------------------------------------------------------------------------
//	�O�����C���t���[��
class CMainFrame :
	public CFrameWindowImpl<CMainFrame>,
	public CMessageFilter
{
public:
	// �R���X�g���N�^�ƃf�X�g���N�^
	CMainFrame();
	virtual ~CMainFrame() {}

    // �E�B���h�E�N���X���A���ʃ��\�[�XID�A�X�^�C���A�w�i�F��o�^
	DECLARE_FRAME_WND_CLASS_EX(_T("d3d11_plots"), IDR_MAINFRAME,
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_WINDOW);

	// ���b�Z�[�W�}�b�v
    BEGIN_MSG_MAP(CMainFrame)
		MSG_WM_TIMER(OnTimer)
		MESSAGE_HANDLER_EX(WM_USER_RESIZEPLOT, OnResizePlot)
		MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
		REFLECT_NOTIFICATIONS_HWND_FILTERED(mPropList)
    END_MSG_MAP()

	// �e��C�x���g�n���h��
	BOOL PreTranslateMessage(MSG *pMsg);
	void OnTimer(UINT_PTR nIDEvent);
	LRESULT OnResizePlot(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void OnSize(UINT nType, CSize size);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();

	// �����ݒ�̃Z�[�u�ƃ��[�h
	void LoadConfig();
	void SaveConfig();

	// ����������
	void InitD3d();

	// �R�}���h���C�����
	void ParseCommandLine(LPWSTR* argv, int argc);

private:
	struct vertex_t {
		float	pos[3];
		float	col[4];
	};
	struct const_buffer_t {
		float	screenW;
		float	screenH;
		float	dummy1;
		float	dummy2;
	};
	typedef std::complex<float> c_type;

	// �e��R���g���[��
	CSplitterWindow		mVSplitter;
	CPlotView			mPlotArea;
	CPropListView		mPropList;

	// ���̑������o
	tstring				mExePath;		// EXE�p�X
	tstring				mExeName;		// EXE�t�@�C����
	tstring				mIniName;		// Ini�t�@�C����
	std::mt19937		m_seed;

	d3d11dev_helper			mD3d11Dev;
	d3d11render_view_helper	mRenderTarget;
	d3d11depth_view_helper	mDepthView;
	d3d11buffer_helper		mConstBuffer;
	rect_plots				mPlots;
};
