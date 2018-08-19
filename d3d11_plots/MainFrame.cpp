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
//	�O�����C���t���[��

// �R���X�g���N�^
CMainFrame::CMainFrame()
	: mConstBuffer(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER),
	mDepthView(DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_DSV_DIMENSION_TEXTURE2D, 0)
{
}

// �C�ӂ̃t�@�C�����[�h
void CMainFrame::ParseCommandLine(LPWSTR* argv, int argc)
{
	if ( argv!=NULL && argc>0 ) {
		tstring::size_type pos;
		tstring doc_path;

		// �v���W�F�N�g�f�B���N�g���擾
		mExeName = argv[0];
		mExePath = argv[0];
		pos = mExePath.rfind(_T('\\'));
		mExePath.erase(pos, mExePath.size()-pos);

		// Ini�t�@�C���p�X�擾
		mIniName = mExePath + _T("\\d3d11_plots.ini");
	}
	else {
		mExeName = _T("d3d11_plots.exe");
		mIniName = _T("d3d11_plots.ini");
	}
}

// ���b�Z�[�W�t�B���^
BOOL CMainFrame::PreTranslateMessage(MSG *pMsg)
{
	if ( CFrameWindowImpl<CMainFrame>::PreTranslateMessage(pMsg) )
		return TRUE;

	return mPlotArea.PreTranslateMessage(pMsg);
}

// �^�C�}�[�C�x���g
void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float y(0.0f);
	std::vector<c_type> buf(50000);
	std::normal_distribution<float> dist(0.0f, 0.1f);
	
	CRect rect;
	mPlotArea.GetClientRect(rect);

	ID3D11DeviceContext* context = mD3d11Dev;
	IDXGISwapChain* swap_chain = mD3d11Dev;

	// �\���f�[�^�̍X�V
	for ( int i=0; i<buf.size(); i++ )
		buf[i] = c_type(dist(m_seed), dist(m_seed));
	mPlots[0]->UpdateData(context, &buf[0], buf.size());
	mPlots[1]->UpdateData(context, (float*)NULL, &buf[0], buf.size());

	// �����_�����O
	CD3D11_VIEWPORT vp(0.0f, 0.0f, rect.Width(), rect.Height());
	context->ClearRenderTargetView(mRenderTarget, color);
	context->ClearDepthStencilView(
		mDepthView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 0.0f, 0x00);
	context->VSSetConstantBuffers(0, 1, mConstBuffer);
	context->RSSetViewports(1, &vp);
	mPlots.Render(context);

	// ���s
	swap_chain->Present(0, 0);
}

// �T�C�Y�ύX�C�x���g
LRESULT CMainFrame::OnResizePlot(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	SetMsgHandled(FALSE);

	if ( wParam!=SIZE_RESTORED && wParam!=SIZE_MINIMIZED ) return 0;
	if ( NULL==(ID3D11Device*)mD3d11Dev ) return 0;

	CRect rect;
	mPlotArea.GetClientRect(&rect);
	float w(rect.Width()), h(rect.Height());

	// �o�b�t�@�̃��T�C�Y
	mRenderTarget.Release();
	mD3d11Dev.Resize(mPlotArea);
	mDepthView.Create(mD3d11Dev, w, h);
	mRenderTarget.Create(mD3d11Dev);
	mRenderTarget.Select(1, mD3d11Dev, mDepthView);

	// �萔�o�b�t�@�̍X�V
	ID3D11DeviceContext *context = mD3d11Dev;
	const_buffer_t cbuf = { w, h };
	context->UpdateSubresource(
		mConstBuffer, 0, NULL, &cbuf, sizeof(const_buffer_t), 0);

	// �O���t�G���A�̃��T�C�Y
	mPlots.SetArea(0.1f*w, 0.1f*h, 0.8f*w, 0.8f*h);

	return 0;
}

// �E�C���h�E����
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// �傫���A�C�R���ݒ�
	HICON hIcon = AtlLoadIconImage(IDR_MAINFRAME, LR_DEFAULTCOLOR,
		::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON));
	SetIcon(hIcon, TRUE);

	// �X�v���b�g�E�C���h�E�̍쐬
	mVSplitter.Create(m_hWnd, rcDefault, NULL,
		WS_CHILD| WS_VISIBLE| WS_CLIPCHILDREN| WS_CLIPSIBLINGS);
	mVSplitter.SetSplitterExtendedStyle(SPLIT_RIGHTALIGNED);

	// �r���[���쐬
	mPlotArea.Create(mVSplitter, rcDefault, NULL,
		WS_CHILD |WS_VISIBLE |WS_CLIPCHILDREN |WS_CLIPSIBLINGS, WS_EX_CLIENTEDGE);
	mPropList.Create(mVSplitter, rcDefault, NULL,
		WS_CHILD |WS_VISIBLE |WS_CLIPCHILDREN |WS_CLIPSIBLINGS |WS_BORDER |LVS_OWNERDATA |LVS_REPORT |LVS_SINGLESEL);

	// �y�C���̐ݒ�
	m_hWndClient = mVSplitter;
	mVSplitter.SetSplitterPane(SPLIT_PANE_LEFT, mPlotArea);
	mVSplitter.SetSplitterPane(SPLIT_PANE_RIGHT, mPropList);

	// ���C�A�E�g�X�V
	UpdateLayout();
	mVSplitter.SetSplitterPos(420);

	// ���b�Z�[�W�t�B���^�ǉ�
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->AddMessageFilter(this);

	return 0;
}

// �E�C���h�E�j��
void CMainFrame::OnDestroy()
{
	KillTimer(1);

	// DirectX�̏I��
	ID3D11DeviceContext* context = mD3d11Dev;
	context->ClearState();
	mRenderTarget.Release();
	mConstBuffer.Release();
	mPlots.Release();
	mD3d11Dev.Release();

	SaveConfig();

	// ���b�Z�[�W���[�v�폜
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	pLoop->RemoveMessageFilter(this);
	SetMsgHandled(false);
}

// �ݒ�̓ǂݍ���
void CMainFrame::LoadConfig()
{
	int top, left, width, height;
	boost::optional<int> value;

	// �ݒ�t�@�C���̃��[�h
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini((const char*)CT2CA(mIniName.c_str()), pt);

	// �E�C���h�E�T�C�Y�̓ǂݍ���
	value = pt.get_optional<int>("MainFrame.Width");
	width = ( value ) ? *value : 800;

	value = pt.get_optional<int>("MainFrame.Height");
	height = ( value ) ? *value : 480;

	value = pt.get_optional<int>("MainFrame.Top");
	top = ( value ) ? *value : 100;

	value = pt.get_optional<int>("MainFrame.Left");
	left = ( value ) ? *value : 100;

	// �E�C���h�E�T�C�Y��ݒ�
	WINDOWPLACEMENT place = { sizeof(WINDOWPLACEMENT) };
	place.rcNormalPosition.left = left;
	place.rcNormalPosition.right = left + width;
	place.rcNormalPosition.top = top;
	place.rcNormalPosition.bottom = top + height;
	place.showCmd = SW_SHOWNORMAL;
	SetWindowPlacement(&place);

	// �X�v���b�g�E�C���h�E�̐ݒ�ǂݍ���
	value = pt.get_optional<int>("MainFrame.VSplitPos");
	left = ( value ) ? *value : 500;
	mVSplitter.SetSplitterPos(left);
}

// �ݒ�̕ۑ�
void CMainFrame::SaveConfig()
{
	int vpos, hpos;
	WINDOWPLACEMENT place = { sizeof(WINDOWPLACEMENT) };
	GetWindowPlacement(&place);
	CRect rect(place.rcNormalPosition);
	boost::property_tree::ptree pt;

	// �E�C���h�E�T�C�Y�̏����o��
	pt.put("MainFrame.Width",	rect.Width());
	pt.put("MainFrame.Height",	rect.Height());
	pt.put("MainFrame.Top",		rect.top);
	pt.put("MainFrame.Left",	rect.left);

	// �ŏ�������Ă���ꍇ�́A���̂܂܃X�v���b�^�̈ʒu���擾
	if ( place.showCmd==SW_SHOWMINIMIZED ) {
		vpos = mVSplitter.GetSplitterPos();
	}

	// ���̂ق��̏ꍇ�́A�Œ肳��Ă���y�C�����ƍ�����ێ�����悤SplitPos���v�Z
	else {
		CRect wnd_rect(0, 0, 0, 0);
		GetWindowRect(&wnd_rect);
		vpos = rect.Width() - (wnd_rect.Width() - mVSplitter.GetSplitterPos());
	}

	// �X�v���b�g�E�C���h�E�̐ݒ菑���o��
	pt.put("MainFrame.VSplitPos", vpos);

	// �ݒ�t�@�C�������o��
	boost::property_tree::write_ini((const char*)CT2CA(mIniName.c_str()), pt);
}

// Direct3d�̏���������
void CMainFrame::InitD3d()
{
	// DirectX�̏�����
	HRESULT hr = mD3d11Dev.Create(mPlotArea);
	if ( FAILED(hr) ) return;

	// �[�x�o�b�t�@�̐���
	mDepthView.Create(mD3d11Dev, mD3d11Dev.Width(), mD3d11Dev.Height());

	// �����_�����O�^�[�Q�b�g�̐���
	mRenderTarget.Create(mD3d11Dev);
	mRenderTarget.Select(1, mD3d11Dev, mDepthView);

	// �萔�o�b�t�@�̐���
	CRect rect;
	mPlotArea.GetClientRect(&rect);
	const_buffer_t cbuf = { rect.Width(), rect.Height() };
	mConstBuffer.Create(
		mD3d11Dev, sizeof(const_buffer_t), 0, 0, &cbuf, sizeof(cbuf));

	// �O���b�h�̍쐬
	rect_trace_ptr trace;
	mPlots.Create(mD3d11Dev, _T("Consolas"), 18);
	mPlots.SetArea(50.0f, 50.0f, 480, 320);
	mPlots.SetHTicks(0.0, 50000.0, 10000.0);
	mPlots.SetVTicks(-5, 15, 5);
	mPlots.SetColor(-1, 0xff808080);

	// �g���[�X�̒ǉ�
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

	// �^�C�}�[�쐬
	SetTimer(1, 16, NULL);
}
