/**
 * PlotView.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "PlotView.h"

// メッセージフィルタ
BOOL CPlotView::PreTranslateMessage(MSG *pMsg)
{
	return FALSE;
}

// リサイズイベント
void CPlotView::OnSize(UINT nType, CSize size)
{
	::PostMessage(
		GetParent().GetParent(), WM_USER_RESIZEPLOT, nType, 0);
}

//ウインドウ開始イベント
int CPlotView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CMessageLoop* loop = _Module.GetMessageLoop();
	loop->AddMessageFilter(this);

	return 0;
}

//ウインドウ破棄イベント
void CPlotView::OnDestroy()
{
	CMessageLoop* loop = _Module.GetMessageLoop();
	loop->RemoveMessageFilter(this);
}
