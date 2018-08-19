/**
 * d3d11_helper.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "d3d11_helper.h"

int g_loglevel;
FILE* g_logfd;

//------------------------------------------------------------------------
//	ID3D11デバイスラッパクラス

// コンストラクタ
d3d11dev_helper::d3d11dev_helper()
	: pDevice(NULL),
	pContext(NULL),
	pSwapChain(NULL)
{
}

// デストラクタ
d3d11dev_helper::~d3d11dev_helper()
{
	Release();
}

// SwapChainでデバイス生成
HRESULT d3d11dev_helper::Create(HWND hwnd, int width, int height)
{
	Release();

	HRESULT hr;
	UINT flags(0);

	// 負の値が格納されている場合は自分でサイズを取得
	if ( width<0 || height<0 ) {
		CRect rect;
		::GetClientRect(hwnd, &rect);
		width = rect.Width();
		height = rect.Height();
	}

	mWidth = width;
	mHeight = height;

	D3D_FEATURE_LEVEL levels[] = { D3D_FEATURE_LEVEL_11_0 };
	D3D_FEATURE_LEVEL lv;

	// ディスクリプタの初期化
	memset(&mSCdesc, 0, sizeof(mSCdesc));
	mSCdesc.BufferCount = 1;
	mSCdesc.BufferDesc.Width = mWidth;
	mSCdesc.BufferDesc.Height = mHeight;
	mSCdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mSCdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	mSCdesc.OutputWindow = hwnd;
	mSCdesc.SampleDesc.Count = 1;
	mSCdesc.SampleDesc.Quality = 0;
	mSCdesc.Windowed = TRUE;
	mSCdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// SwapChainの作成
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		flags,
		levels,
		1,
		D3D11_SDK_VERSION,
		&mSCdesc,
		&pSwapChain,
		&pDevice,
		&lv,
		&pContext
		);

	if ( FAILED(hr) )
		LOG_PRINTF(LOG_ERROR, "d3d11dev_helper:\t%08x\n", hr);

	return hr;
}

// リサイズ
HRESULT d3d11dev_helper::Resize(HWND hwnd, int width, int height)
{
	HRESULT hr;
	if ( pSwapChain==NULL || pDevice==NULL ) return -1;

	// 負の値が格納されている場合は自分でサイズを取得
	if ( width<0 || height<0 ) {
		CRect rect;
		::GetClientRect(hwnd, &rect);
		width = rect.Width();
		height = rect.Height();
	}

	mWidth = width;
	mHeight = height;

	// ターゲットを開放してリサイズ
	ID3D11RenderTargetView*	target = NULL;
	pContext->OMSetRenderTargets(1, &target, NULL);
	hr = pSwapChain->ResizeBuffers(
		mSCdesc.BufferCount,
		mWidth, mHeight,
		mSCdesc.BufferDesc.Format,
		mSCdesc.Flags);

	if ( FAILED(hr) )
		LOG_PRINTF(LOG_ERROR, "d3d11dev_helper:\t%08x\n", hr);

	return hr;
}

// 開放
void d3d11dev_helper::Release()
{
	if ( pContext ) {
		pContext->ClearState();
	}

	if ( pSwapChain ) {
		pSwapChain->Release();
		pSwapChain = NULL;
	}

	if ( pContext ) {
		pContext->Release();
		pContext = NULL;
	}

	if ( pDevice ) {
		pDevice->Release();
		pDevice = NULL;
	}
}


//------------------------------------------------------------------------
//	ID3D11RenderTargetViewラッパ

// コンストラクタ(backbafferターゲット)
d3d11render_view_helper::d3d11render_view_helper()
	: pTarget(NULL)
{
}

// デストラクタ
d3d11render_view_helper::~d3d11render_view_helper()
{
	Release();
}

// RenderTargetViewを作成
HRESULT d3d11render_view_helper::Create(
	ID3D11Device* pdev, IDXGISwapChain* p_swap)
{
	Release();
	
	// バックバッファの取得
	ID3D11Texture2D *p_buff;
	m_hRes = p_swap->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&p_buff);
	if ( FAILED(m_hRes) ) return m_hRes;

	// レンダリングターゲット生成
	m_hRes = pdev->CreateRenderTargetView(p_buff, NULL, &pTarget);
	
	if ( p_buff ) {
		p_buff->Release();
	}

	if ( FAILED(m_hRes) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11render_view_helper:\t%08x\n", m_hRes);
	}
	return m_hRes;
}
HRESULT d3d11render_view_helper::Create(d3d11dev_helper& helper)
{
	return Create(helper, helper);
}

// RenderTargetを選択
void d3d11render_view_helper::Select(
	UINT num_view, ID3D11DeviceContext* context, ID3D11DepthStencilView* depth)
{
	if ( NULL==pTarget ) return;
	context->OMSetRenderTargets(num_view, &pTarget, depth);
}

// 開放
void d3d11render_view_helper::Release()
{
	if ( pTarget ) {
		pTarget->Release();
		pTarget = NULL;
	}
}


//------------------------------------------------------------------------
//	ID3D11DepthStencilViewラッパ

// コンストラクタ
d3d11depth_view_helper::d3d11depth_view_helper(
	DXGI_FORMAT format,
	D3D11_DSV_DIMENSION view_dim,
	UINT mip_slice)
	: mWidth(0),
	mHeight(0),
	pDepthView(NULL)
{
	memset(&mDesc, 0, sizeof(mDesc));

	mDesc.Format = format;
	mDesc.ViewDimension = view_dim;
	mDesc.Texture2D.MipSlice = mip_slice;
}

// デストラクタ
d3d11depth_view_helper::~d3d11depth_view_helper()
{
	Release();
}

// DepthStencilViewの作成
HRESULT d3d11depth_view_helper::Create(
	ID3D11Device* pdev, UINT width, UINT height)
{
	HRESULT hr;

	Release();
	mWidth = width;
	mHeight = height;

	// テクスチャ作成
	d3d11tex2d_helper tex2d(
		mWidth, mHeight, 1, 1, mDesc.Format, 1, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_DEPTH_STENCIL);
	hr = tex2d.Create(pdev);
	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR,
			"d3d11depth_view_helper:\tfailed to create texture %08x\n", hr);
		return hr;
	}

	// 深度バッファ作成
	hr = pdev->CreateDepthStencilView(tex2d, &mDesc, &pDepthView);
	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR,
			"d3d11depth_view_helper:\tfailed to create depth-stencil view %08x\n", hr);
		return hr;
	}

	return 0;
}

// 開放
void d3d11depth_view_helper::Release()
{
	if ( pDepthView ) {
		pDepthView->Release();
		pDepthView = NULL;
	}
}


//------------------------------------------------------------------------
//	ID3D11Texture2Dラッパクラス

// デフォルトコンストラクタ
d3d11tex2d_helper::d3d11tex2d_helper()
	: pTexture(NULL),
	pSRview(NULL)
{
	memset(&mDesc, 0, sizeof(mDesc));
}

// コンストラクタ
d3d11tex2d_helper::d3d11tex2d_helper(
	UINT width,
	UINT height,
	UINT mip_levels,
	UINT array_size,
	DXGI_FORMAT format,
	UINT samp_desc_count,
	UINT samp_desc_quality,
	D3D11_USAGE usage,
	UINT bind_flags,
	UINT cpu_access_flags,
	UINT misc_flags)
	: pTexture(NULL),
	pSRview(NULL)
{
	SetDesc(
		width,
		height,
		mip_levels,
		array_size,
		format,
		samp_desc_count,
		samp_desc_quality,
		usage,
		bind_flags,
		cpu_access_flags,
		misc_flags);
}

// デストラクタ
d3d11tex2d_helper::~d3d11tex2d_helper()
{
	Release();
}

// ディスクリプタの設定
void d3d11tex2d_helper::SetDesc(
	UINT width,
	UINT height,
	UINT mip_levels,
	UINT array_size,
	DXGI_FORMAT format,
	UINT samp_desc_count,
	UINT samp_desc_quality,
	D3D11_USAGE usage,
	UINT bind_flags,
	UINT cpu_access_flags,
	UINT misc_flags)
{
	mDesc.Width = width;
	mDesc.Height = height;
	mDesc.MipLevels = mip_levels;
	mDesc.ArraySize = array_size;
	mDesc.Format = format;
	mDesc.SampleDesc.Count = samp_desc_count;
	mDesc.SampleDesc.Quality = samp_desc_quality;
	mDesc.Usage = usage;
	mDesc.BindFlags = bind_flags;
	mDesc.CPUAccessFlags = cpu_access_flags;
	mDesc.MiscFlags = misc_flags;

	mSRdesc.Format = format;

	// テクスチャ配列の場合
	if ( array_size>=2 ) {

		// マルチサンプルテクスチャの場合
		if ( samp_desc_count>1 ) {
			mSRdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
			mSRdesc.Texture2DMSArray.FirstArraySlice = 0;
			mSRdesc.Texture2DMSArray.ArraySize = array_size;
		}
		else {
			mSRdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
			mSRdesc.Texture2DArray.MostDetailedMip = 0;
			mSRdesc.Texture2DArray.MipLevels = mip_levels;
			mSRdesc.Texture2DArray.FirstArraySlice = 0;
			mSRdesc.Texture2DArray.ArraySize = array_size;
		}
	}

	// 単一非マルチサンプルテクスチャ
	else if ( samp_desc_count==1 ) {

		// マルチサンプルテクスチャの場合
		if ( samp_desc_count>1 ) {
			mSRdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		else {
			mSRdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			mSRdesc.Texture2D.MostDetailedMip = 0;
			mSRdesc.Texture2D.MipLevels = mip_levels;
		}
	}
}

// 現在のパラメータでテクスチャを生成
HRESULT d3d11tex2d_helper::Create(
	ID3D11Device* pdev, const D3D11_SUBRESOURCE_DATA* data)
{
	Release();
	
	// テクスチャを作製
	HRESULT hr = pdev->CreateTexture2D(&mDesc, data, &pTexture);

	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11tex2d_helper:\t%08x\n", hr);
		return hr;
	}

	// リソースビューを作製
	if ( mDesc.BindFlags==D3D11_BIND_SHADER_RESOURCE ) {
		hr = pdev->CreateShaderResourceView(
			pTexture, &mSRdesc, &pSRview);
	}
	return hr;
}

// 開放
void d3d11tex2d_helper::Release()
{
	if ( pSRview!=NULL ) {
		pSRview->Release();
		pSRview = NULL;
	}
	if ( pTexture!=NULL ) {
		pTexture->Release();
		pTexture = NULL;
	}
}

//------------------------------------------------------------------------
//	ID3D11Bufferラッパクラス

// コンストラクタ
d3d11buffer_helper::d3d11buffer_helper(
	D3D11_USAGE usage, UINT bind_flags, UINT cpu_access)
	: pBuffer(NULL)
{
	memset(&mDesc, 0, sizeof(mDesc));
	mDesc.Usage = usage;
	mDesc.BindFlags = bind_flags;
	mDesc.CPUAccessFlags = cpu_access;
}

// デストラクタ
d3d11buffer_helper::~d3d11buffer_helper()
{
	Release();
}

// バッファを生成
HRESULT d3d11buffer_helper::Create(
	ID3D11Device* pdev,
	UINT byte_width,
	UINT misc_flags,
	UINT structure_stride,
	const void* p_data,
	UINT mem_pitch,
	UINT slice_pitch)
{
	HRESULT hr;
	Release();

	mDesc.ByteWidth = byte_width;
	mDesc.MiscFlags = misc_flags;
	mDesc.StructureByteStride = structure_stride;
	
	if ( p_data!=NULL ) {
		D3D11_SUBRESOURCE_DATA sub_res;
		sub_res.pSysMem = p_data;
		sub_res.SysMemPitch = mem_pitch;
		sub_res.SysMemSlicePitch = slice_pitch;
		hr = pdev->CreateBuffer(&mDesc, &sub_res, &pBuffer);
	}
	else {
		hr = pdev->CreateBuffer(&mDesc, NULL, &pBuffer);
	}
	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11buffer_helper:\t%08x\n", hr);
	}
	return hr;
}

// 開放
void d3d11buffer_helper::Release()
{
	if ( pBuffer ) {
		pBuffer->Release();
		pBuffer = NULL;
	}
}

// マップ
D3D11_MAPPED_SUBRESOURCE* d3d11buffer_helper::Map(
	ID3D11DeviceContext* context,
	UINT sub_resource, D3D11_MAP map_type, UINT map_flags)
{
	HRESULT hr;
	memset(&mResource, 0, sizeof(mResource));

	hr = context->Map(
		pBuffer, sub_resource, map_type, map_flags, &mResource);

	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11buffer_helper:\t%08x\n", hr);
		return NULL;
	}

	return &mResource;
}

// アンマップ
void d3d11buffer_helper::Unmap(
	ID3D11DeviceContext* context, UINT sub_resource)
{
	context->Unmap(pBuffer, sub_resource);
}


//------------------------------------------------------------------------
//	シェーダー生成クラス

// コンストラクタ
d3d11shader_helper::d3d11shader_helper()
	: pVS(NULL),
	pPS(NULL)
{
}
	
// デストラクタ
d3d11shader_helper::~d3d11shader_helper()
{
	Release();
}

// 頂点シェーダー生成
HRESULT d3d11shader_helper::CreateVS(
	ID3D11Device* pdev, const char* fname, ID3D11ClassLinkage *link)
{
	HRESULT hr;
	ReleaseVS();

	// バイナリ読み込み
	if ( 0==ReadBinary(mVCode, fname) ) return -1;

	// シェーダー生成
	hr = pdev->CreateVertexShader(
		&mVCode[0], mVCode.size(), link, &pVS);

	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11shader_helper:\t%08x\n", hr);
	}
	return hr;
}

// ピクセルシェーダー生成
HRESULT d3d11shader_helper::CreatePS(
	ID3D11Device* pdev, const char* fname, ID3D11ClassLinkage *link)
{
	HRESULT hr;
	ReleasePS();

	// バイナリ読み込み
	if ( 0==ReadBinary(mPCode, fname) ) return -1;

	// シェーダー生成
	hr = pdev->CreatePixelShader(
		&mPCode[0], mPCode.size(), link, &pPS);

	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11shader_helper:\t%08x\n", hr);
	}
	return hr;
}

// リソースをすべて開放
void d3d11shader_helper::Release()
{
	ReleaseVS();
	ReleasePS();
}

// 頂点シェーダーを開放
void d3d11shader_helper::ReleaseVS()
{
	if ( pVS ) {
		pVS->Release();
		pVS = NULL;
		mVCode.clear();
	}
}

// ピクセルシェーダーを開放
void d3d11shader_helper::ReleasePS()
{
	if ( pPS ) {
		pPS->Release();
		pPS = NULL;
		mPCode.clear();
	}
}

//------------------------------------------------------------------------
//	ID3D11InputLayoutラッパクラス

// コンストラクタ
d3d11input_helper::d3d11input_helper()
	: pInputLayout(NULL)
{
}

// デストラクタ
d3d11input_helper::~d3d11input_helper()
{
	Release();
}

// 入力リストを追加
d3d11input_helper& d3d11input_helper::Add(
	LPCSTR semantic_name,
	UINT semantic_index,
	DXGI_FORMAT format,
	UINT input_slot,
	UINT aligned_byte_offset,
	D3D11_INPUT_CLASSIFICATION input_slot_class,
	UINT instance_data_step_rate)
{
	D3D11_INPUT_ELEMENT_DESC desc;

	desc.SemanticName = semantic_name;
	desc.SemanticIndex = semantic_index;
	desc.Format = format;
	desc.InputSlot = input_slot;
	desc.AlignedByteOffset = aligned_byte_offset;
	desc.InputSlotClass = input_slot_class;
	desc.InstanceDataStepRate = instance_data_step_rate;
	mInputList.push_back(desc);

	return *this;
}

// 入力リストをクリア
void d3d11input_helper::Clear()
{
	mInputList.clear();
}

// レイアウトインターフェイスを生成
HRESULT d3d11input_helper::Create(
	ID3D11Device* pdev, const void* vcode, SIZE_T vsize)
{
	HRESULT hr;

	hr = pdev->CreateInputLayout(
		&mInputList[0], mInputList.size(), vcode, vsize, &pInputLayout);
	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11input_helper:\t%08x\n", hr);
	}
	return hr;
}

// 開放
void d3d11input_helper::Release()
{
	if ( pInputLayout ) {
		pInputLayout->Release();
		pInputLayout = NULL;
	}
}

//------------------------------------------------------------------------
//	ID3D11SamplerStateラッパクラス

// コンストラクタ
d3d11sampler_state::d3d11sampler_state(
	D3D11_FILTER filter,
	D3D11_TEXTURE_ADDRESS_MODE addr_u,
	D3D11_TEXTURE_ADDRESS_MODE addr_v,
	D3D11_TEXTURE_ADDRESS_MODE addr_w,
	UINT max_anisotrophy,
	D3D11_COMPARISON_FUNC comparison_func)
	: pSampler(NULL)
{
	memset(&mSSdesc, 0, sizeof(mSSdesc));
	mSSdesc.Filter = filter;
	mSSdesc.AddressU = addr_u;
	mSSdesc.AddressV = addr_v;
	mSSdesc.AddressW = addr_w;
	mSSdesc.MaxAnisotropy = max_anisotrophy;
	mSSdesc.ComparisonFunc = comparison_func;
	mSSdesc.MipLODBias = 0.0f;
	mSSdesc.MinLOD = 0.0f;
	mSSdesc.MaxLOD = (std::numeric_limits<FLOAT>::max)();
}

// デストラクタ
d3d11sampler_state::~d3d11sampler_state()
{
	Release();
}

// 開放
void d3d11sampler_state::Release()
{
	if ( pSampler!=NULL ) {
		pSampler->Release();
		pSampler = NULL;
	}
}

// SamplerStateを生成
HRESULT d3d11sampler_state::Create(ID3D11Device* pdev)
{
	HRESULT hr;

	Release();
	hr = pdev->CreateSamplerState(&mSSdesc, &pSampler);
	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11sampler_state:\t%08x\n", hr);
	}

	return hr;
}

// ミップマップレベルの設定
void d3d11sampler_state::SetMipLOD(
	FLOAT lod_bias, FLOAT min_lod, FLOAT max_lod)
{
	mSSdesc.MipLODBias = lod_bias;
	mSSdesc.MinLOD = min_lod;
	mSSdesc.MaxLOD = max_lod;
}


//------------------------------------------------------------------------
//	ID3D11BlendStateラッパクラス

// コンストラクタ
d3d11blend_state::d3d11blend_state(
	BOOL alpha_to_coverage, BOOL independent_blend)
	: pBlend(NULL)
{
	memset(&mBSdesc, 0, sizeof(mBSdesc));
	mBSdesc.AlphaToCoverageEnable = alpha_to_coverage;
	mBSdesc.IndependentBlendEnable = independent_blend;
}

// デストラクタ
d3d11blend_state::~d3d11blend_state()
{
	Release();
}

// 開放
void d3d11blend_state::Release()
{
	if ( pBlend!=NULL ) {
		pBlend->Release();
		pBlend = NULL;
	}
}

// SamplerStateを生成
HRESULT d3d11blend_state::Create(ID3D11Device* pdev)
{
	Release();

	HRESULT hr;
	hr = pdev->CreateBlendState(&mBSdesc, &pBlend);
	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11blend_state:\t%08x\n", hr);
	}

	return hr;
}

// ブレンドステートを設定
void d3d11blend_state::SetState(
	size_t index,
	BOOL blend_enable,
	D3D11_BLEND src_blend,
	D3D11_BLEND dest_blend,
	D3D11_BLEND_OP blend_op,
	D3D11_BLEND src_blend_alpha,
	D3D11_BLEND dest_blend_alpha,
	D3D11_BLEND_OP blend_op_alpha,
	UINT8 write_mask)
{
	if ( index>=8 ) return;

	mBSdesc.RenderTarget[index].BlendEnable = blend_enable;
	mBSdesc.RenderTarget[index].SrcBlend = src_blend;
	mBSdesc.RenderTarget[index].DestBlend = dest_blend;
	mBSdesc.RenderTarget[index].BlendOp = blend_op;
	mBSdesc.RenderTarget[index].SrcBlendAlpha = src_blend_alpha;
	mBSdesc.RenderTarget[index].DestBlendAlpha = dest_blend_alpha;
	mBSdesc.RenderTarget[index].BlendOpAlpha = blend_op_alpha;
	mBSdesc.RenderTarget[index].RenderTargetWriteMask = write_mask;
}


//------------------------------------------------------------------------
//	ID3D11DepthStencilStateラッパクラス

// デフォルトコンストラクタ
d3d11depth_stencil_state::d3d11depth_stencil_state(
	BOOL depth_enable, BOOL stencil_enable)
	: mDepthStencil(NULL)
{
	memset(&mDSdesc, 0, sizeof(mDSdesc));

	mDSdesc.DepthEnable = depth_enable;
	mDSdesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	mDSdesc.DepthFunc = D3D11_COMPARISON_LESS;

	mDSdesc.StencilEnable = stencil_enable;
	mDSdesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	mDSdesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	mDSdesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDSdesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mDSdesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDSdesc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;
	mDSdesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	mDSdesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	mDSdesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	mDSdesc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;
}

// デストラクタ
d3d11depth_stencil_state::~d3d11depth_stencil_state()
{
	Release();
}

// 開放
void d3d11depth_stencil_state::Release()
{
	if ( mDepthStencil ) {
		mDepthStencil->Release();
		mDepthStencil = NULL;
	}
}

// DepthStancilStateを生成
HRESULT d3d11depth_stencil_state::Create(ID3D11Device* pdev)
{
	HRESULT hr;

	Release();
	hr = pdev->CreateDepthStencilState(&mDSdesc, &mDepthStencil);
	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "d3d11depth_stencil_state:\t%08x\n", hr);
	}

	return hr;
}

// 深度ステートを設定
void d3d11depth_stencil_state::SetDepthState(
	BOOL depth_enable,
	D3D11_DEPTH_WRITE_MASK depth_write_mask,
	D3D11_COMPARISON_FUNC depth_func)
{
	mDSdesc.DepthEnable = depth_enable;
	mDSdesc.DepthWriteMask = depth_write_mask;
	mDSdesc.DepthFunc = depth_func;
}

// ステンシルステートを設定
void d3d11depth_stencil_state::SetStencilState(
	BOOL stencil_enable,
	UINT8 stencil_read_mask,
	UINT8 stencil_write_mask)
{
	mDSdesc.StencilEnable = stencil_enable;
	mDSdesc.StencilReadMask = stencil_read_mask;
	mDSdesc.StencilWriteMask = stencil_write_mask;
}

// オペレーションを設定(Front==Back)
void d3d11depth_stencil_state::SetOperation(
	D3D11_STENCIL_OP stencil_fail_op,
	D3D11_STENCIL_OP stencil_depth_fail_op,
	D3D11_STENCIL_OP stencil_pass_op,
	D3D11_COMPARISON_FUNC stencil_func)
{
	SetOperation(
		stencil_fail_op,
		stencil_depth_fail_op,
		stencil_pass_op,
		stencil_func,
		stencil_fail_op,
		stencil_depth_fail_op,
		stencil_pass_op,
		stencil_func);
}

// オペレーションを設定(Front!=Back)
void d3d11depth_stencil_state::SetOperation(
	D3D11_STENCIL_OP fr_stencil_fail_op,
	D3D11_STENCIL_OP fr_stencil_depth_fail_op,
	D3D11_STENCIL_OP fr_stencil_pass_op,
	D3D11_COMPARISON_FUNC fr_stencil_func,
	D3D11_STENCIL_OP bk_stencil_fail_op,
	D3D11_STENCIL_OP bk_stencil_depth_fail_op,
	D3D11_STENCIL_OP bk_stencil_pass_op,
	D3D11_COMPARISON_FUNC bk_stencil_func)
{
	mDSdesc.FrontFace.StencilFailOp = fr_stencil_fail_op;
	mDSdesc.FrontFace.StencilDepthFailOp = fr_stencil_depth_fail_op;
	mDSdesc.FrontFace.StencilPassOp = fr_stencil_pass_op;
	mDSdesc.FrontFace.StencilFunc = fr_stencil_func;
	mDSdesc.BackFace.StencilFailOp = fr_stencil_fail_op;
	mDSdesc.BackFace.StencilDepthFailOp = fr_stencil_depth_fail_op;
	mDSdesc.BackFace.StencilPassOp = fr_stencil_pass_op;
	mDSdesc.BackFace.StencilFunc = fr_stencil_func;
}


// バイナリをバッファに読み込む
DWORD ReadBinary(byte_vector_t& buf, const char* fname)
{
	HANDLE hfile = CreateFileA(
		fname, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hfile==INVALID_HANDLE_VALUE ) {
		return 0;
	}

	// ファイルサイズを取得してバッファに読み込み
	DWORD size, n;
	size = GetFileSize(hfile, NULL);
	buf.resize(size);
	if ( 0==ReadFile(hfile, &buf[0], size, &n, NULL) ) {
		n = 0;
	}

	CloseHandle(hfile);
	return n;
}

