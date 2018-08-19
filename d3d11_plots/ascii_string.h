/**
 * ascii_string.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once
#include "d3d11_helper.h"

//------------------------------------------------------------------------
//	SUB_RESOURCE�z��N���X
class d3d11sub_resource_array
{
public:
	typedef std::vector<D3D11_SUBRESOURCE_DATA> resource_array_t;
	typedef std::vector<size_t> size_array_t;

	// �f�t�H���g�R���X�g���N�^
	d3d11sub_resource_array();

	// �R���X�g���N�^
	d3d11sub_resource_array(size_t num_elements, size_t data_size);

	// �f�X�g���N�^
	virtual ~d3d11sub_resource_array();

	// ���T�C�Y
	void resize(size_t num_elements, size_t data_size);

	// ���\�[�X�N���A
	void clear();

	// �e�T�u���\�[�X�T�C�Y�̎擾
	size_t size(size_t index) const	{ return mSize[index]; }

	// �v�f���̎擾
	size_t num_elements() const		{ return mResources.size(); }

	// �v�f�A�N�Z�X
	D3D11_SUBRESOURCE_DATA* operator()(size_t index) {
		return &mResources[index];
	}
	const D3D11_SUBRESOURCE_DATA* operator()(size_t index) const {
		return &mResources[index];
	}

	// �L���X�g���Z�q
	operator D3D11_SUBRESOURCE_DATA*()		{ return &mResources[0]; }
	operator const D3D11_SUBRESOURCE_DATA*(){ return &mResources[0]; }

private:
	resource_array_t	mResources;
	size_array_t		mSize;

};


//------------------------------------------------------------------------
//	ASCII������N���X
class ascii_string
{
public:
	// �R���X�g���N�^
	ascii_string(BOOL enable_stencil, size_t max_len=8192);

	// �f�X�g���N�^
	virtual ~ascii_string();

	// ���\�[�X�J��
	void Release();

	// ���\�[�X�쐬
	HRESULT Create(ID3D11Device* pdev, LPCTSTR font, int size);

	// �O���t�B�b�N�`��
	void Render(ID3D11DeviceContext* context);

	// �t�H���g�č\�z
	void SetColor(DWORD color);

	// �e�L�X�g�f�[�^���o�b�t�@�ɓ���
	void Sync(ID3D11DeviceContext* context);

	// ������`��
	void Print(float px, float py, const char* text);

	// ���������
	void Clear();

	// �A�N�Z�X���\�b�h
	int Size() const		{ return m_size;   }
	int FontWidth() const	{ return m_width;  }
	int FontHeight() const	{ return m_height; }

	void rand_walk(bool init=false);

private:
	struct vertex_t {
		float	x;
		float	y;
		float	u;
		float	v;
	};
	struct font_instance_t {
		float	col[4];
		float	x;
		float	y;
		float	scale;
		float	tex_w;
	};
	typedef std::vector<font_instance_t> instance_array_t;

	d3d11tex2d_helper			mTexture;		// �e�N�X�`���z��
	d3d11buffer_helper			mVertices;		// ���_�o�b�t�@
	d3d11buffer_helper			mIndices;		// �C���f�b�N�X�o�b�t�@
	d3d11buffer_helper			mInstances;		// �C���X�^���X���
	d3d11shader_helper			mShader;		// �V�F�[�_�[
	d3d11input_helper			mInputLayout;	// ���̓��C�A�E�g
	instance_array_t			mSource;		// �C���X�^���X���̌��f�[�^
	d3d11sampler_state			mSampler;		// �T���v��
	d3d11blend_state			mBlend;			// �u�����h�X�e�[�g
	d3d11depth_stencil_state	mDepthStencil;	// �X�e���V���X�e�[�g

	int			m_size;		// �t�H���g�T�C�Y
	tstring		m_font;		// �t�H���g����
	std::string	m_text;		// ������
	int			m_width;
	int			m_height;
	int			m_num_instances;
	float		m_color[4];
	bool		m_modified;

	std::mt19937	m_gen;

	std::pair<int, int>
		create_fonts(d3d11sub_resource_array& out, HDC hdc) const;
};
