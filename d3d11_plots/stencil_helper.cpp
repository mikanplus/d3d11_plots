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
//	矩形クリッピング

// デフォルトコンストラクタ
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

// 引数付きコンストラクタ
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

// デストラクタ
rect_stencil::~rect_stencil()
{
	Release();
}

// リソース開放
void rect_stencil::Release()
{
	mDepthStencil.Release();
	mInputLayout.Release();
	mShader.Release();
	mIndices.Release();
	mVertices.Release();
}

// マスクを作成します
HRESULT rect_stencil::Create(ID3D11Device* pdev)
{
	float left, right, top, bottom;
	HRESULT hr;

	left   = mRect.left;
	right  = mRect.right;
	top    = mRect.top;
	bottom = mRect.bottom;

	// 頂点バッファの作成
	vertex_t vertices[] = {
		{ left, top }, { right, top }, { left, bottom }, { right, bottom }
	};
	hr = mVertices.Create(pdev, sizeof(vertices), 0, 0, vertices);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	// インデックスバッファの作成
	WORD indices[] = { 0, 1, 2, 3, 2, 1 };
	hr = mIndices.Create(pdev, sizeof(indices), 0, 0, indices);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	// シェーダーの作成
	hr = mShader.CreateVS(pdev, "vs_def_stencil.cso", NULL);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;
	hr = mShader.CreatePS(pdev, "ps_def_stencil.cso", NULL);
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	// 入力レイアウトの作成
	hr = mInputLayout.Create(pdev, mShader.VCode(), mShader.VSize());
	if ( FAILED(hr) ) goto RECT_STENCIL_CREATE_ERROR;

	// ステンシルステートの作成(ZテストFAIL、ステンシル書き込み)
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

// グラフィック描画
void rect_stencil::Render(ID3D11DeviceContext* context)
{
	UINT strides, offset(0);

	// クリッピングエリア変更時にバッファ更新
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

// クリッピングエリアの変更
void rect_stencil::SetArea(
	float ox, float oy, float width, float height)
{
	mRect.left = ox;
	mRect.top = oy;
	mRect.right = ox + width;
	mRect.bottom = oy + height;
	m_modified = true;
}
