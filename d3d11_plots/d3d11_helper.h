/**
 * d3d11_helper.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once

//------------------------------------------------------------------------
//	ID3D11デバイスラッパクラス
class d3d11dev_helper
{
public:
	// コンストラクタ
	d3d11dev_helper();

	// デストラクタ
	virtual ~d3d11dev_helper();

	// SwapChainでデバイス生成
	HRESULT Create(HWND hwnd, int width=-1, int height=-1);

	// リサイズ
	HRESULT Resize(HWND hwnd, int width=-1, int height=-1);

	// 開放
	void Release();

	// キャスト演算子
	operator ID3D11Device*()			{ return pDevice;    }
	operator ID3D11DeviceContext*()		{ return pContext;   }
	operator IDXGISwapChain*()			{ return pSwapChain; }

	// 画面情報
	UINT Width() const	{ return mWidth;  }
	UINT Height() const	{ return mHeight; }

private:
	ID3D11Device*			pDevice;
	ID3D11DeviceContext*	pContext;
	IDXGISwapChain*			pSwapChain;
	DXGI_SWAP_CHAIN_DESC	mSCdesc;
	UINT	mWidth;
	UINT	mHeight;
};

//------------------------------------------------------------------------
//	ID3D11RenderTargetViewラッパ
class d3d11render_view_helper
{
public:
	// コンストラクタ
	d3d11render_view_helper();

	// デストラクタ
	virtual ~d3d11render_view_helper();

	// RenderTargetViewを作成
	HRESULT Create(ID3D11Device* pdev, IDXGISwapChain* p_swap);
	HRESULT Create(d3d11dev_helper& helper);

	// RenderTargetを選択
	void Select(
		UINT num_view,
		ID3D11DeviceContext* context,
		ID3D11DepthStencilView* depth);

	// 開放
	void Release();

	// キャスト演算子
	operator ID3D11RenderTargetView*() { return pTarget; }

private:
	ID3D11RenderTargetView*	pTarget;
	HRESULT	m_hRes;
};

//------------------------------------------------------------------------
//	ID3D11DepthStencilViewラッパ
class d3d11depth_view_helper
{
public:
	// コンストラクタ
	d3d11depth_view_helper(
		DXGI_FORMAT format,
		D3D11_DSV_DIMENSION view_dim,
		UINT mip_slice);

	// デストラクタ
	virtual ~d3d11depth_view_helper();

	// DepthStencilViewの作成
	HRESULT Create(ID3D11Device* pdev, UINT width, UINT height);

	// キャスト演算子
	operator ID3D11DepthStencilView*() { return pDepthView; }

	// 開放
	void Release();

private:
	D3D11_DEPTH_STENCIL_VIEW_DESC	mDesc;
	ID3D11DepthStencilView*			pDepthView;
	UINT	mWidth;
	UINT	mHeight;
};

//------------------------------------------------------------------------
//	ID3D11Texture2Dラッパクラス
class d3d11tex2d_helper
{
public:
	// デフォルトコンストラクタ
	d3d11tex2d_helper();

	// コンストラクタ
	d3d11tex2d_helper(
		UINT width,
		UINT height,
		UINT mip_levels,
		UINT array_size,
		DXGI_FORMAT format,
		UINT samp_desc_count	= 1,
		UINT samp_desc_quality	= 0,
		D3D11_USAGE usage		= D3D11_USAGE_DEFAULT,
		UINT bind_flags			= 0,
		UINT cpu_access_flags	= 0,
		UINT misc_flags			= 0
		);

	// デストラクタ
	virtual ~d3d11tex2d_helper();

	// ディスクリプタの設定
	void SetDesc(
		UINT width,
		UINT height,
		UINT mip_levels,
		UINT array_size,
		DXGI_FORMAT format,
		UINT samp_desc_count	= 1,
		UINT samp_desc_quality	= 0,
		D3D11_USAGE usage		= D3D11_USAGE_DEFAULT,
		UINT bind_flags			= 0,
		UINT cpu_access_flags	= 0,
		UINT misc_flags			= 0
		);

	// 現在のパラメータでテクスチャを生成
	HRESULT Create(ID3D11Device* pdev, const D3D11_SUBRESOURCE_DATA* data=NULL);

	// 開放
	void Release();

	// キャスト演算子
	operator ID3D11Texture2D*()				{ return pTexture; }
	operator ID3D11ShaderResourceView*()	{ return pSRview;  }
	operator ID3D11ShaderResourceView**()	{ return &pSRview; }

	// アロー演算子
	ID3D11Texture2D* operator->()	{ return pTexture; }

private:
	ID3D11Texture2D*		pTexture;	// インターフェイス
	D3D11_TEXTURE2D_DESC	mDesc;		// ディスクリプタ

	ID3D11ShaderResourceView*		pSRview;
	D3D11_SHADER_RESOURCE_VIEW_DESC	mSRdesc;
};

//------------------------------------------------------------------------
//	ID3D11Bufferラッパクラス
class d3d11buffer_helper
{
public:
	// コンストラクタ
	d3d11buffer_helper(
		D3D11_USAGE usage, UINT bind_flags, UINT cpu_access=0);

	// デストラクタ
	virtual ~d3d11buffer_helper();

	// バッファを生成
	HRESULT Create(ID3D11Device* pdev,
		UINT byte_width,
		UINT misc_flags,
		UINT structure_stride,
		const void* p_data	= NULL,
		UINT mem_pitch		= 0,
		UINT slice_pitch	= 0);

	// 開放
	void Release();

	// マップ
	D3D11_MAPPED_SUBRESOURCE* Map(
		ID3D11DeviceContext* context,
		UINT sub_resource,
		D3D11_MAP map_type,
		UINT map_flags	=0);

	// アンマップ
	void Unmap(ID3D11DeviceContext* context, UINT sub_resource);

	// キャスト演算子
	operator ID3D11Buffer*()	{ return pBuffer;  }
	operator ID3D11Buffer**()	{ return &pBuffer; }

	// アロー演算子
	ID3D11Buffer* operator->()	{ return pBuffer; }

private:
	ID3D11Buffer*		pBuffer;		// インターフェイス
	D3D11_BUFFER_DESC	mDesc;
	D3D11_MAPPED_SUBRESOURCE	mResource;
};


//------------------------------------------------------------------------
//	シェーダー生成クラス
class d3d11shader_helper
{
public:
	typedef std::vector<BYTE> byte_vector_t;

	// コンストラクタ
	d3d11shader_helper();
	
	// デストラクタ
	virtual ~d3d11shader_helper();

	// 頂点シェーダー生成
	HRESULT CreateVS(
		ID3D11Device* pdev, const char* fname, ID3D11ClassLinkage *link);

	// ピクセルシェーダー生成
	HRESULT CreatePS(
		ID3D11Device* pdev, const char* fname, ID3D11ClassLinkage *link);

	// リソースをすべて開放
	void Release();

	// 頂点シェーダーを開放
	void ReleaseVS();

	// ピクセルシェーダーを開放
	void ReleasePS();

	// キャスト演算子
	operator ID3D11VertexShader*()	{ return pVS; }
	operator ID3D11PixelShader*()	{ return pPS; }

	// コード取得
	const BYTE* VCode() const	{ return &mVCode[0]; }
	size_t VSize() const		{ return mVCode.size(); }
	const BYTE* PCode() const	{ return &mPCode[0]; }
	size_t PSize() const		{ return mPCode.size(); }

private:
	ID3D11VertexShader*		pVS;		// 頂点シェーダー
	ID3D11PixelShader*		pPS;		// ピクセルシェーダー
	byte_vector_t		mVCode;			// 頂点シェーダーバイナリ
	byte_vector_t		mPCode;			// ピクセルシェーダーバイナリ
};

//------------------------------------------------------------------------
//	ID3D11InputLayoutラッパクラス
class d3d11input_helper
{
public:
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> input_list_t;

	// コンストラクタ
	d3d11input_helper();

	// デストラクタ
	virtual ~d3d11input_helper();

	// 入力リストを追加
	d3d11input_helper& Add(
		LPCSTR semantic_name,
		UINT semantic_index,
		DXGI_FORMAT format,
		UINT input_slot,
		UINT aligned_byte_offset,
		D3D11_INPUT_CLASSIFICATION input_slot_class,
		UINT instance_data_step_rate);

	// 入力リストをクリア
	void Clear();

	// レイアウトインターフェイスを生成
	HRESULT Create(
		ID3D11Device* pdev, const void* vcode, SIZE_T vsize);

	// 開放
	void Release();

	// キャスト演算子
	operator ID3D11InputLayout*()	{ return pInputLayout; }

private:
	input_list_t		mInputList;
	ID3D11InputLayout*	pInputLayout;
};

//------------------------------------------------------------------------
//	ID3D11SamplerStateラッパクラス
class d3d11sampler_state
{
public:
	// コンストラクタ
	d3d11sampler_state(
		D3D11_FILTER filter,
		D3D11_TEXTURE_ADDRESS_MODE addr_u,
		D3D11_TEXTURE_ADDRESS_MODE addr_v,
		D3D11_TEXTURE_ADDRESS_MODE addr_w,
		UINT max_anisotrophy,
		D3D11_COMPARISON_FUNC comparison_func);

	// デストラクタ
	virtual ~d3d11sampler_state();

	// 開放
	void Release();

	// SamplerStateを生成
	HRESULT Create(ID3D11Device* pdev);

	// ミップマップレベルの設定
	void SetMipLOD(FLOAT lod_bias, FLOAT min_lod, FLOAT max_lod);

	// キャスト演算子
	operator ID3D11SamplerState*()	{ return pSampler;  }
	operator ID3D11SamplerState**()	{ return &pSampler; }

private:
	ID3D11SamplerState*	pSampler;
	D3D11_SAMPLER_DESC	mSSdesc;
};


//------------------------------------------------------------------------
//	ID3D11BlendStateラッパクラス
class d3d11blend_state
{
public:
	// コンストラクタ
	d3d11blend_state(
		BOOL alpha_to_coverage, BOOL independent_blend);

	// デストラクタ
	virtual ~d3d11blend_state();

	// 開放
	void Release();

	// BlendStateを生成
	HRESULT Create(ID3D11Device* pdev);

	// ブレンドステートを設定
	void SetState(size_t index,
		BOOL blend_enable             = false,
		D3D11_BLEND src_blend         = D3D11_BLEND_ONE,
		D3D11_BLEND dest_blend        = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP blend_op       = D3D11_BLEND_OP_ADD,
		D3D11_BLEND src_blend_alpha   = D3D11_BLEND_ONE,
		D3D11_BLEND dest_blend_alpha  = D3D11_BLEND_ZERO,
		D3D11_BLEND_OP blend_op_alpha = D3D11_BLEND_OP_ADD,
		UINT8 write_mask = D3D11_COLOR_WRITE_ENABLE_ALL);

	operator ID3D11BlendState*()  { return pBlend; }

private:
	ID3D11BlendState*	pBlend;
	D3D11_BLEND_DESC	mBSdesc;
};


//------------------------------------------------------------------------
//	ID3D11DepthStencilStateラッパクラス
class d3d11depth_stencil_state
{
public:
	// デフォルトコンストラクタ
	d3d11depth_stencil_state(
		BOOL depth_enable   =FALSE,
		BOOL stencil_enable =FALSE);

	// デストラクタ
	virtual ~d3d11depth_stencil_state();

	// 開放
	void Release();

	// DepthStancilStateを生成
	HRESULT Create(ID3D11Device* pdev);

	// 深度ステートを設定
	void SetDepthState(
		BOOL depth_enable,
		D3D11_DEPTH_WRITE_MASK depth_write_mask,
		D3D11_COMPARISON_FUNC depth_func);

	// ステンシルステートを設定
	void SetStencilState(
		BOOL stencil_enable,
		UINT8 stencil_read_mask,
		UINT8 stencil_write_mask);

	// オペレーションを設定(Front==Back)
	void SetOperation(
		D3D11_STENCIL_OP stencil_fail_op,
		D3D11_STENCIL_OP stencil_depth_fail_op,
		D3D11_STENCIL_OP stencil_pass_op,
		D3D11_COMPARISON_FUNC stencil_func);

	// オペレーションを設定(Front!=Back)
	void SetOperation(
		D3D11_STENCIL_OP fr_stencil_fail_op,
		D3D11_STENCIL_OP fr_stencil_depth_fail_op,
		D3D11_STENCIL_OP fr_stencil_pass_op,
		D3D11_COMPARISON_FUNC fr_stencil_func,
		D3D11_STENCIL_OP bk_stencil_fail_op,
		D3D11_STENCIL_OP bk_stencil_depth_fail_op,
		D3D11_STENCIL_OP bk_stencil_pass_op,
		D3D11_COMPARISON_FUNC bk_stencil_func);

	// キャスト演算子
	operator ID3D11DepthStencilState*()	{ return mDepthStencil; }

private:
	ID3D11DepthStencilState*	mDepthStencil;
	D3D11_DEPTH_STENCIL_DESC	mDSdesc;
};

typedef std::vector<BYTE> byte_vector_t;

// バイナリをバッファに読み込む
DWORD ReadBinary(byte_vector_t& buf, const char* fname);

// ログ出力
extern int g_loglevel;
extern FILE* g_logfd;

#define LOG_ERROR	0x8000
#define LOG_INFO	0x1000
#define LOG_PRINTF(level,fmt,...) \
	if ((level)&g_loglevel) fprintf(g_logfd,fmt, ## __VA_ARGS__); fflush(g_logfd)
