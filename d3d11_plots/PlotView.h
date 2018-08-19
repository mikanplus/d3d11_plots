/**
 * PlotView.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once

//ユーザー定義メッセージ
#define	WM_USER_RESIZEPLOT		(WM_USER+531)

//------------------------------------------------------------------------
//	グラフエリア
class CPlotView :
	public CWindowImpl<CPlotView>,
	public CMessageFilter
{
public:
	DECLARE_WND_CLASS_EX(NULL, CS_HREDRAW|CS_VREDRAW, NULL)

	// メッセージマップ
	BEGIN_MSG_MAP(CPlotView)
		MSG_WM_SIZE(OnSize)
		MSG_WM_CREATE(OnCreate)
        MSG_WM_DESTROY(OnDestroy)
	END_MSG_MAP()
	
	// 各種イベントハンドラ
	BOOL PreTranslateMessage(MSG *pMsg);
	void OnSize(UINT nType, CSize size);
	int OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnDestroy();
};
