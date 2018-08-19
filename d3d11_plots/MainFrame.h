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

//------------------------------------------------------------------------
//	外装メインフレーム
class CMainFrame :
	public CFrameWindowImpl<CMainFrame>,
	public CMessageFilter
{
public:
	// コンストラクタとデストラクタ
	CMainFrame();
	virtual ~CMainFrame() {}

    // ウィンドウクラス名、共通リソースID、スタイル、背景色を登録
	DECLARE_FRAME_WND_CLASS_EX(_T("ConstellationView"), IDR_MAINFRAME,
		CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS, COLOR_WINDOW);

	// メッセージマップ
    BEGIN_MSG_MAP(CMainFrame)
		MSG_WM_TIMER(OnTimer)
		MSG_WM_SIZE(OnSize)
		MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
		CHAIN_MSG_MAP(CFrameWindowImpl<CMainFrame>)
    END_MSG_MAP()

	// 各種イベントハンドラ
	BOOL PreTranslateMessage(MSG *pMsg);
	void OnTimer(UINT_PTR nIDEvent);
	void OnSize(UINT nType, CSize size);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();

	// 初期設定のセーブとロード
	void LoadConfig();
	void SaveConfig();

	// 初期化処理
	void InitD3d();

	// コマンドライン解析
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

	// その他メンバ
	tstring				mExePath;		// EXEパス
	tstring				mExeName;		// EXEファイル名
	tstring				mIniName;		// Iniファイル名
	std::mt19937		m_seed;

	d3d11dev_helper			mD3d11Dev;
	d3d11render_view_helper	mRenderTarget;
	d3d11depth_view_helper	mDepthView;
	d3d11buffer_helper		mConstBuffer;
	rect_plots				mPlots;
};
