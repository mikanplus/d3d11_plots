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
//	ID3D11�f�o�C�X���b�p�N���X
class d3d11dev_helper
{
public:
	// �R���X�g���N�^
	d3d11dev_helper();

	// �f�X�g���N�^
	virtual ~d3d11dev_helper();

	// SwapChain�Ńf�o�C�X����
	HRESULT Create(HWND hwnd, int width=-1, int height=-1);

	// ���T�C�Y
	HRESULT Resize(HWND hwnd, int width=-1, int height=-1);

	// �J��
	void Release();

	// �L���X�g���Z�q
	operator ID3D11Device*()			{ return pDevice;    }
	operator ID3D11DeviceContext*()		{ return pContext;   }
	operator IDXGISwapChain*()			{ return pSwapChain; }

	// ��ʏ��
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
//	ID3D11RenderTargetView���b�p
class d3d11render_view_helper
{
public:
	// �R���X�g���N�^
	d3d11render_view_helper();

	// �f�X�g���N�^
	virtual ~d3d11render_view_helper();

	// RenderTargetView���쐬
	HRESULT Create(ID3D11Device* pdev, IDXGISwapChain* p_swap);
	HRESULT Create(d3d11dev_helper& helper);

	// RenderTarget��I��
	void Select(
		UINT num_view,
		ID3D11DeviceContext* context,
		ID3D11DepthStencilView* depth);

	// �J��
	void Release();

	// �L���X�g���Z�q
	operator ID3D11RenderTargetView*() { return pTarget; }

private:
	ID3D11RenderTargetView*	pTarget;
	HRESULT	m_hRes;
};

//------------------------------------------------------------------------
//	ID3D11DepthStencilView���b�p
class d3d11depth_view_helper
{
public:
	// �R���X�g���N�^
	d3d11depth_view_helper(
		DXGI_FORMAT format,
		D3D11_DSV_DIMENSION view_dim,
		UINT mip_slice);

	// �f�X�g���N�^
	virtual ~d3d11depth_view_helper();

	// DepthStencilView�̍쐬
	HRESULT Create(ID3D11Device* pdev, UINT width, UINT height);

	// �L���X�g���Z�q
	operator ID3D11DepthStencilView*() { return pDepthView; }

	// �J��
	void Release();

private:
	D3D11_DEPTH_STENCIL_VIEW_DESC	mDesc;
	ID3D11DepthStencilView*			pDepthView;
	UINT	mWidth;
	UINT	mHeight;
};

//------------------------------------------------------------------------
//	ID3D11Texture2D���b�p�N���X
class d3d11tex2d_helper
{
public:
	// �f�t�H���g�R���X�g���N�^
	d3d11tex2d_helper();

	// �R���X�g���N�^
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

	// �f�X�g���N�^
	virtual ~d3d11tex2d_helper();

	// �f�B�X�N���v�^�̐ݒ�
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

	// ���݂̃p�����[�^�Ńe�N�X�`���𐶐�
	HRESULT Create(ID3D11Device* pdev, const D3D11_SUBRESOURCE_DATA* data=NULL);

	// �J��
	void Release();

	// �L���X�g���Z�q
	operator ID3D11Texture2D*()				{ return pTexture; }
	operator ID3D11ShaderResourceView*()	{ return pSRview;  }
	operator ID3D11ShaderResourceView**()	{ return &pSRview; }

	// �A���[���Z�q
	ID3D11Texture2D* operator->()	{ return pTexture; }

private:
	ID3D11Texture2D*		pTexture;	// �C���^�[�t�F�C�X
	D3D11_TEXTURE2D_DESC	mDesc;		// �f�B�X�N���v�^

	ID3D11ShaderResourceView*		pSRview;
	D3D11_SHADER_RESOURCE_VIEW_DESC	mSRdesc;
};

//------------------------------------------------------------------------
//	ID3D11Buffer���b�p�N���X
class d3d11buffer_helper
{
public:
	// �R���X�g���N�^
	d3d11buffer_helper(
		D3D11_USAGE usage, UINT bind_flags, UINT cpu_access=0);

	// �f�X�g���N�^
	virtual ~d3d11buffer_helper();

	// �o�b�t�@�𐶐�
	HRESULT Create(ID3D11Device* pdev,
		UINT byte_width,
		UINT misc_flags,
		UINT structure_stride,
		const void* p_data	= NULL,
		UINT mem_pitch		= 0,
		UINT slice_pitch	= 0);

	// �J��
	void Release();

	// �}�b�v
	D3D11_MAPPED_SUBRESOURCE* Map(
		ID3D11DeviceContext* context,
		UINT sub_resource,
		D3D11_MAP map_type,
		UINT map_flags	=0);

	// �A���}�b�v
	void Unmap(ID3D11DeviceContext* context, UINT sub_resource);

	// �L���X�g���Z�q
	operator ID3D11Buffer*()	{ return pBuffer;  }
	operator ID3D11Buffer**()	{ return &pBuffer; }

	// �A���[���Z�q
	ID3D11Buffer* operator->()	{ return pBuffer; }

private:
	ID3D11Buffer*		pBuffer;		// �C���^�[�t�F�C�X
	D3D11_BUFFER_DESC	mDesc;
	D3D11_MAPPED_SUBRESOURCE	mResource;
};


//------------------------------------------------------------------------
//	�V�F�[�_�[�����N���X
class d3d11shader_helper
{
public:
	typedef std::vector<BYTE> byte_vector_t;

	// �R���X�g���N�^
	d3d11shader_helper();
	
	// �f�X�g���N�^
	virtual ~d3d11shader_helper();

	// ���_�V�F�[�_�[����
	HRESULT CreateVS(
		ID3D11Device* pdev, const char* fname, ID3D11ClassLinkage *link);

	// �s�N�Z���V�F�[�_�[����
	HRESULT CreatePS(
		ID3D11Device* pdev, const char* fname, ID3D11ClassLinkage *link);

	// ���\�[�X�����ׂĊJ��
	void Release();

	// ���_�V�F�[�_�[���J��
	void ReleaseVS();

	// �s�N�Z���V�F�[�_�[���J��
	void ReleasePS();

	// �L���X�g���Z�q
	operator ID3D11VertexShader*()	{ return pVS; }
	operator ID3D11PixelShader*()	{ return pPS; }

	// �R�[�h�擾
	const BYTE* VCode() const	{ return &mVCode[0]; }
	size_t VSize() const		{ return mVCode.size(); }
	const BYTE* PCode() const	{ return &mPCode[0]; }
	size_t PSize() const		{ return mPCode.size(); }

private:
	ID3D11VertexShader*		pVS;		// ���_�V�F�[�_�[
	ID3D11PixelShader*		pPS;		// �s�N�Z���V�F�[�_�[
	byte_vector_t		mVCode;			// ���_�V�F�[�_�[�o�C�i��
	byte_vector_t		mPCode;			// �s�N�Z���V�F�[�_�[�o�C�i��
};

//------------------------------------------------------------------------
//	ID3D11InputLayout���b�p�N���X
class d3d11input_helper
{
public:
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> input_list_t;

	// �R���X�g���N�^
	d3d11input_helper();

	// �f�X�g���N�^
	virtual ~d3d11input_helper();

	// ���̓��X�g��ǉ�
	d3d11input_helper& Add(
		LPCSTR semantic_name,
		UINT semantic_index,
		DXGI_FORMAT format,
		UINT input_slot,
		UINT aligned_byte_offset,
		D3D11_INPUT_CLASSIFICATION input_slot_class,
		UINT instance_data_step_rate);

	// ���̓��X�g���N���A
	void Clear();

	// ���C�A�E�g�C���^�[�t�F�C�X�𐶐�
	HRESULT Create(
		ID3D11Device* pdev, const void* vcode, SIZE_T vsize);

	// �J��
	void Release();

	// �L���X�g���Z�q
	operator ID3D11InputLayout*()	{ return pInputLayout; }

private:
	input_list_t		mInputList;
	ID3D11InputLayout*	pInputLayout;
};

//------------------------------------------------------------------------
//	ID3D11SamplerState���b�p�N���X
class d3d11sampler_state
{
public:
	// �R���X�g���N�^
	d3d11sampler_state(
		D3D11_FILTER filter,
		D3D11_TEXTURE_ADDRESS_MODE addr_u,
		D3D11_TEXTURE_ADDRESS_MODE addr_v,
		D3D11_TEXTURE_ADDRESS_MODE addr_w,
		UINT max_anisotrophy,
		D3D11_COMPARISON_FUNC comparison_func);

	// �f�X�g���N�^
	virtual ~d3d11sampler_state();

	// �J��
	void Release();

	// SamplerState�𐶐�
	HRESULT Create(ID3D11Device* pdev);

	// �~�b�v�}�b�v���x���̐ݒ�
	void SetMipLOD(FLOAT lod_bias, FLOAT min_lod, FLOAT max_lod);

	// �L���X�g���Z�q
	operator ID3D11SamplerState*()	{ return pSampler;  }
	operator ID3D11SamplerState**()	{ return &pSampler; }

private:
	ID3D11SamplerState*	pSampler;
	D3D11_SAMPLER_DESC	mSSdesc;
};


//------------------------------------------------------------------------
//	ID3D11BlendState���b�p�N���X
class d3d11blend_state
{
public:
	// �R���X�g���N�^
	d3d11blend_state(
		BOOL alpha_to_coverage, BOOL independent_blend);

	// �f�X�g���N�^
	virtual ~d3d11blend_state();

	// �J��
	void Release();

	// BlendState�𐶐�
	HRESULT Create(ID3D11Device* pdev);

	// �u�����h�X�e�[�g��ݒ�
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
//	ID3D11DepthStencilState���b�p�N���X
class d3d11depth_stencil_state
{
public:
	// �f�t�H���g�R���X�g���N�^
	d3d11depth_stencil_state(
		BOOL depth_enable   =FALSE,
		BOOL stencil_enable =FALSE);

	// �f�X�g���N�^
	virtual ~d3d11depth_stencil_state();

	// �J��
	void Release();

	// DepthStancilState�𐶐�
	HRESULT Create(ID3D11Device* pdev);

	// �[�x�X�e�[�g��ݒ�
	void SetDepthState(
		BOOL depth_enable,
		D3D11_DEPTH_WRITE_MASK depth_write_mask,
		D3D11_COMPARISON_FUNC depth_func);

	// �X�e���V���X�e�[�g��ݒ�
	void SetStencilState(
		BOOL stencil_enable,
		UINT8 stencil_read_mask,
		UINT8 stencil_write_mask);

	// �I�y���[�V������ݒ�(Front==Back)
	void SetOperation(
		D3D11_STENCIL_OP stencil_fail_op,
		D3D11_STENCIL_OP stencil_depth_fail_op,
		D3D11_STENCIL_OP stencil_pass_op,
		D3D11_COMPARISON_FUNC stencil_func);

	// �I�y���[�V������ݒ�(Front!=Back)
	void SetOperation(
		D3D11_STENCIL_OP fr_stencil_fail_op,
		D3D11_STENCIL_OP fr_stencil_depth_fail_op,
		D3D11_STENCIL_OP fr_stencil_pass_op,
		D3D11_COMPARISON_FUNC fr_stencil_func,
		D3D11_STENCIL_OP bk_stencil_fail_op,
		D3D11_STENCIL_OP bk_stencil_depth_fail_op,
		D3D11_STENCIL_OP bk_stencil_pass_op,
		D3D11_COMPARISON_FUNC bk_stencil_func);

	// �L���X�g���Z�q
	operator ID3D11DepthStencilState*()	{ return mDepthStencil; }

private:
	ID3D11DepthStencilState*	mDepthStencil;
	D3D11_DEPTH_STENCIL_DESC	mDSdesc;
};

typedef std::vector<BYTE> byte_vector_t;

// �o�C�i�����o�b�t�@�ɓǂݍ���
DWORD ReadBinary(byte_vector_t& buf, const char* fname);

// ���O�o��
extern int g_loglevel;
extern FILE* g_logfd;

#define LOG_ERROR	0x8000
#define LOG_INFO	0x1000
#define LOG_PRINTF(level,fmt,...) \
	if ((level)&g_loglevel) fprintf(g_logfd,fmt, ## __VA_ARGS__); fflush(g_logfd)
