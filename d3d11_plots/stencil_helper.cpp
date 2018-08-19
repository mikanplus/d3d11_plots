/**
 * stencil_helper.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "stencil_helper.h"

//------------------------------------------------------------------------
//	��`�N���b�s���O

// �f�t�H���g�R���X�g���N�^
rect_stencil::rect_stencil()
	: mVertices(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER),
	mIndices(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER),
	mDepthStencil(TRUE, TRUE),
	mRect(0.0f, 0.0f, 1.0f, 1.0f),
	m_modified(true)
{
	mInputLayout.Add(
		"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
}

// �����t���R���X�g���N�^
rect_stencil::rect_stencil(
	float ox, float oy, float width, float height)
	: mVertices(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER),
	mIndices(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER),
	mDepthStencil(TRUE, TRUE),
	mRect(0.0f, 0.0f, 1.0f, 1.0f),
	m_modified(true)
{
	SetArea(ox, oy, width, height);
	mInputLayout.Add(
		"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
}

// �f�X�g���N�^
rect_stencil::~rect_stencil()
{
	Release();
}

// ���\�[�X�J��
void rect_stencil::Release()
{
	mDepthStencil.Release();
	mInputLayout.Release();
	mShader.Release();
	mIndices.Release();
	mVertices.Release();
}

// �}�X�N���쐬���܂�
HRESULT rect_stencil::Create(ID3D11Device* pdev)
{
	float left, right, top, bottom;
	HRESULT hr;

	left   = mRect.left;
	right  = mRect.right;
	top    = mRect.top;
	bottom = mRect.bottom;

	// ���_�o�b�t�@�̍쐬
	vertex_t vertices[] = {
		{ left, top }, { right, top }, { left, bottom }, { right, bottom }
	};
	hr = mVertices.Create(pdev, sizeof(vertices), 0, 0, vertices);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	// �C���f�b�N�X�o�b�t�@�̍쐬
	WORD indices[] = { 0, 1, 2, 3, 2, 1 };
	hr = mIndices.Create(pdev, sizeof(indices), 0, 0, indices);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	// �V�F�[�_�[�̍쐬
	hr = mShader.CreateVS(pdev, "vs_def_stencil.cso", NULL);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;
	hr = mShader.CreatePS(pdev, "ps_def_stencil.cso", NULL);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	// ���̓��C�A�E�g�̍쐬
	hr = mInputLayout.Create(pdev, mShader.VCode(), mShader.VSize());
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	// �X�e���V���X�e�[�g�̍쐬(Z�e�X�gFAIL�A�X�e���V����������)
	mDepthStencil.SetDepthState(
		TRUE, D3D11_DEPTH_WRITE_MASK_ZERO, D3D11_COMPARISON_NEVER);
	mDepthStencil.SetOperation(
		D3D11_STENCIL_OP_REPLACE,
		D3D11_STENCIL_OP_REPLACE,
		D3D11_STENCIL_OP_REPLACE,
		D3D11_COMPARISON_ALWAYS);
	hr = mDepthStencil.Create(pdev);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	m_modified = false;
	return hr;

RECT_STENCIL_CREATE_ERROR:
	Release();
	return hr;
}

// �O���t�B�b�N�`��
void rect_stencil::Render(ID3D11DeviceContext* context)
{
	UINT strides, offset(0);

	// �N���b�s���O�G���A�ύX���Ƀo�b�t�@�X�V
	if ( m_modified ) {
		vertex_t vertices[] = {
			{ mRect.left,  mRect.top },
			{ mRect.right, mRect.top },
			{ mRect.left,  mRect.bottom },
			{ mRect.right, mRect.bottom }
		};
		CD3D11_BOX box(0, 0, 0, sizeof(vertices), 1, 1);
		context->UpdateSubresource(
			mVertices, 0, &box, vertices, sizeof(vertices), 0);
		m_modified = false;
	}

	context->VSSetShader(mShader, NULL, 0);
	context->PSSetShader(mShader, NULL, 0);
	context->IASetInputLayout(mInputLayout);
	strides = sizeof(vertex_t);
	context->IASetVertexBuffers(0, 1, mVertices, &strides, &offset);
	context->IASetIndexBuffer(mIndices, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->OMSetBlendState(NULL, NULL, 0xffffffff);
	context->OMSetDepthStencilState(mDepthStencil, 0xff);
	context->DrawIndexed(6, 0, 0);
}

// �N���b�s���O�G���A�̕ύX
void rect_stencil::SetArea(
	float ox, float oy, float width, float height)
{
	mRect.left = ox;
	mRect.top = oy;
	mRect.right = ox + width;
	mRect.bottom = oy + height;
	m_modified = true;
}
