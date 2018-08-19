/**
 * d3d11_main.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

// d3d11_main.cpp
#include "stdafx.h"
#include "MainFrame.h"

#pragma comment( lib, "d3d11.lib" )

//ATL/WTL�C���X�^���X
CAppModule _Module;

//�G���g���[�|�C���g
int APIENTRY _tWinMain(HINSTANCE hInstance, 
                       HINSTANCE hPrevInstance, 
                       LPTSTR    lpCmdLine, 
                       int       nCmdShow)
{
	std::locale::global( std::locale("") );
	g_loglevel = LOG_ERROR|LOG_INFO;
	g_logfd = fopen("d3d11_plots.log", "a");

	// �R���g���[���̏�����
	HRESULT hRes = ::CoInitialize(NULL);
	ATLASSERT(SUCCEEDED(hRes));
	::DefWindowProc(NULL, 0, 0, 0L);
	AtlInitCommonControls(ICC_WIN95_CLASSES|ICC_USEREX_CLASSES);

	// ���b�Z�[�W���[�v���쐬
	CMessageLoop theLoop;
	_Module.Init(NULL, hInstance);
	_Module.AddMessageLoop(&theLoop);

	// �R�}���h���C���擾
	int argc = 0;
	LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	// �A�v���P�[�V�����Ǘ��N���X�̐ݒ�
    CMainFrame wnd;

	// �E�C���h�E���쐬���āA�v���O�����J�n
	wnd.CreateEx(NULL, CRect(100, 100, 800, 540));
	wnd.ParseCommandLine(argv, argc);
	wnd.LoadConfig();
	wnd.InitD3d();
	wnd.ShowWindow(nCmdShow);
	int nRet = theLoop.Run();
	LocalFree(argv);

	// �I������
	_Module.RemoveMessageLoop();
	_Module.Term();
	::CoUninitialize();

	fclose(g_logfd);
    return nRet;
}

