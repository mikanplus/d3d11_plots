/**
 * MainFrame.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "MainFrame.h"

//---------------------------------------------------------------------------
//	外装メインフレーム

// コンストラクタ
CMainFrame::CMainFrame()
	: mConstBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER),
	mDepthView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_DSV_DIMENSION_TEXTURE2D, 0)
{
}

// 任意のファイルロード
void CMainFrame::ParseCommandLine(LPWSTR* argv, int argc)
{
	if ( argv!=NULL && argc>0 ) {
		tstring::size_type pos;
		tstring doc_path;

		// プロジェクトディレクトリ取得
		mExeName = argv[0];
		mExePath = argv[0];
		pos = mExePath.rfind(_T('\\'));
		mExePath.erase(pos, mExePath.size()-pos);

		// Iniファイルパス取得
		mIniName = mExePath + _T("\\d3d11_plots.ini");
	}
	else {
		mExeName = _T("d3d11_plots.exe");
		mIniName = _T("d3d11_plots.ini");
	}
}

// メッセージフィルタ
BOOL CMainFrame::PreTranslateMessage(MSG *pMsg)
{
	if ( CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg) )
		return TRUE;

	return FALSE;
}

// タイマーイベント
void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	// 表示データ作成
	float y(0.0f);
	std::vector<c_type> buf(50000);
	std::normal_distribution<float> dist(0.0f, 0.1f);
//	std::exponential_distribution<float> dist(0.1f);
//	std::uniform_real_distribution<float> dist(-0.2, 0.2);
	
	HRESULT hr;
	CRect rect;
	GetClientRect(rect);

	ID3D11DeviceContext* context = mD3d11Dev;
	IDXGISwapChain* swap_chain = mD3d11Dev;
	CD3D11_VIEWPORT vp(0.0f, 0.0f, rect.Width(), rect.Height());
	
	const_buffer_t cbuf = { rect.Width(), rect.Height() };
	context->UpdateSubresource(
		mConstBuffer, 0, NULL, &cbuf, sizeof(const_buffer_t), 0);

	context->ClearRenderTargetView(mRenderTarget, color);
	context->ClearDepthStencilView(
		mDepthView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 0.0f, 0x00);
	context->VSSetConstantBuffers(0, 1, mConstBuffer);
	context->RSSetViewports(1, &vp);

	for ( int i=0; i<buf.size(); i++ )
		buf[i] = c_type(dist(m_seed), dist(m_seed));
	mPlots[0]->UpdateData(context, &buf[0], buf.size());

	mPlots[1]->UpdateData(
		context, (float*)NULL, &buf[0], buf.size());

	mPlots.Render(context);

	swap_chain->Present(0, 0);
}

// サイズ変更イベント
void CMainFrame::OnSize(UINT nType, CSize size)
{
	if ( nType!=SIZE_RESTORED && nType!=SIZE_MINIMIZED ) return;
	if ( NULL==(ID3D11Device*)mD3d11Dev ) return;

	mRenderTarget.Release();
	mD3d11Dev.Resize(*this);
	mDepthView.Create(mD3d11Dev, mD3d11Dev.Width(), mD3d11Dev.Height());
	mRenderTarget.Create(mD3d11Dev);
	mRenderTarget.Select(1, mD3d11Dev, mDepthView);
}

// ウインドウ生成
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// 大きいアイコン設定
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR,
		::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);

	// メッセージフィルタ追加
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return 0;
}

// ウインドウ破棄
void CMainFrame::OnDestroy()
{
	KillTimer(1);

	// DirectXの終了
	ID3D11DeviceContext* context = mD3d11Dev;
	context->ClearState();
	mRenderTarget.Release();
	mConstBuffer.Release();
	mPlots.Release();
	mD3d11Dev.Release();

	SaveConfig();

	// メッセージループ削除
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);
	SetMsgHandled(false);
}

// 設定の読み込み
void CMainFrame::LoadConfig()
{
	int top, left, width, height;
	boost::optional<int> value;

	// 設定ファイルのロード
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini((const char*)CT2CA(mIniName.c_str()), pt);

	// ウインドウサイズの読み込み
	value = pt.get_optional<int>("MainFrame.Width");
	width = ( value ) ? *value : 800;

	value = pt.get_optional<int>("MainFrame.Height");
	height = ( value ) ? *value : 480;

	value = pt.get_optional<int>("MainFrame.Top");
	top = ( value ) ? *value : 100;

	value = pt.get_optional<int>("MainFrame.Left");
	left = ( value ) ? *value : 100;

	// ウインドウサイズを設定
	WINDOWPLACEMENT place = { sizeof(WINDOWPLACEMENT) };
	place.rcNormalPosition.left = left;
	place.rcNormalPosition.right = left + width;
	place.rcNormalPosition.top = top;
	place.rcNormalPosition.bottom = top + height;
	place.showCmd = SW_SHOWNORMAL;
	SetWindowPlacement(&place);
}

// 設定の保存
void CMainFrame::SaveConfig()
{
	int vpos, hpos;
	WINDOWPLACEMENT place = { sizeof(WINDOWPLACEMENT) };
	GetWindowPlacement(&place);
	CRect rect(place.rcNormalPosition);

	boost::property_tree::ptree pt;

	// ウインドウサイズの書き出し
	pt.put("MainFrame.Width",	rect.Width());
	pt.put("MainFrame.Height",	rect.Height());
	pt.put("MainFrame.Top",		rect.top);
	pt.put("MainFrame.Left",	rect.left);

	// 設定ファイル書き出し
	boost::property_tree::write_ini((const char*)CT2CA(mIniName.c_str()), pt);
}

// Direct3dの初期化処理
void CMainFrame::InitD3d()
{
	// DirectXの初期化
	HRESULT hr = mD3d11Dev.Create(*this);
	if ( FAILED(hr) ) return;

	// 深度バッファの生成
	mDepthView.Create(mD3d11Dev, mD3d11Dev.Width(), mD3d11Dev.Height());

	// レンダリングターゲットの生成
	mRenderTarget.Create(mD3d11Dev);
	mRenderTarget.Select(1, mD3d11Dev, mDepthView);

	// 定数バッファの生成
	mConstBuffer.Create(mD3d11Dev, sizeof(const_buffer_t), 0, 0);

	// グリッドの作成
	rect_trace_ptr trace;
	mPlots.Create(mD3d11Dev, _T("Consolas"), 18);
	mPlots.SetArea(50.0f, 50.0f, 480, 320);
	mPlots.SetHTicks(0.0, 50000.0, 10000.0);
	mPlots.SetVTicks(-5, 15, 5);
	mPlots.SetColor(-1, 0xff808080);

	// トレースの追加
	std::vector<float> buf(50000);
	for ( int i=0; i<buf.size(); i++ ) buf[i] = i;

	trace = mPlots.AddTrace(mD3d11Dev, rect_trace::PLOT_DOTS);
	trace->SetColor(0x1000f0f0);
	trace->SetHRange(-1.25, 1.25);
	trace->SetVRange(-1.0, 1.0);

	trace = mPlots.AddTrace(mD3d11Dev, rect_trace::PLOT_LINES);
	trace->SetColor(0x4000f000);
	trace->SetVRange(-5, 15, true);
	trace->UpdateData(mD3d11Dev, &buf[0], (c_type*)NULL, buf.size());

	// タイマー作成
	SetTimer(1, 16, NULL);
}
