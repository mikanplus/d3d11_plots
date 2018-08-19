/**
 * ascii_string.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "ascii_string.h"

//------------------------------------------------------------------------
//	SUB_RESOURCE�z��N���X

// �f�t�H���g�R���X�g���N�^
d3d11sub_resource_array::d3d11sub_resource_array()
{
}

// �R���X�g���N�^
d3d11sub_resource_array::d3d11sub_resource_array(
	size_t num_elements, size_t data_size)
{
	resize(num_elements, data_size);
}

// �f�X�g���N�^
d3d11sub_resource_array::~d3d11sub_resource_array()
{
	clear();
}

// ���T�C�Y
void d3d11sub_resource_array::resize(
	size_t num_elements, size_t data_size)
{
	mResources.resize(num_elements);
	mSize.resize(num_elements, data_size);

	for ( size_t i=0; i<num_elements; i++ ) {
		mResources[i].pSysMem = new BYTE[ data_size ];
		mResources[i].SysMemPitch = 0;
		mResources[i].SysMemSlicePitch = 0;
	}
}

// ���\�[�X�N���A
void d3d11sub_resource_array::clear()
{
	for ( size_t i=0; i<mResources.size(); i++ ) {
		delete[] (BYTE*)(mResources[i].pSysMem);
	}
	mResources.clear();
	mSize.clear();
}


//------------------------------------------------------------------------
//	ASCII������N���X

// �R���X�g���N�^
ascii_string::ascii_string(BOOL enable_stencil, size_t max_len)
	: mVertices(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER),
	mIndices(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER),
	mInstances(
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_VERTEX_BUFFER),
	mSampler(
		D3D11_FILTER_MIN_MAG_MIP_LINEAR,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		D3D11_TEXTURE_ADDRESS_CLAMP,
		0, D3D11_COMPARISON_ALWAYS),
	mBlend(FALSE, FALSE),
	mDepthStencil(FALSE, enable_stencil),
	m_size(14),
	m_font(_T("Consolas")),
	m_width(14),
	m_height(14),
	m_num_instances(0),
	m_modified(false)
{
	mSource.resize(max_len);
	memset(&mSource[0], 0, sizeof(font_instance_t)*max_len);

	mInputLayout.Add(
		"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	mInputLayout.Add(
		"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0);
	mInputLayout.Add(
		"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	mInputLayout.Add(
		"TEXCOORD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_PER_INSTANCE_DATA, 1);
}

// �f�X�g���N�^
ascii_string::~ascii_string()
{
	Release();
}

// ���\�[�X�J��
void ascii_string::Release()
{
	mDepthStencil.Release();
	mBlend.Release();
	mSampler.Release();
	mInputLayout.Release();
	mShader.Release();
	mInstances.Release();
	mIndices.Release();
	mVertices.Release();
	mTexture.Release();
}

// ���\�[�X�쐬
HRESULT ascii_string::Create(
	ID3D11Device* pdev, LPCTSTR font, int size)
{
	m_font = font;
	m_size = size;

	//�f�o�C�X�R���e�L�X�g�̎擾
	HDC hdc = GetDC(NULL);

	//�t�H���g���
	HFONT hFont = CreateFont(
		m_size, 0, 0, 0, 0, FALSE, FALSE, FALSE,
		ANSI_CHARSET,
		OUT_TT_ONLY_PRECIS,
		CLIP_DEFAULT_PRECIS,
		PROOF_QUALITY,
		FIXED_PITCH | FF_MODERN,
		m_font.c_str()
	);

	//�t�H���g��I��
	HFONT oldFont = (HFONT)SelectObject(hdc, hFont);

	//�e�N�X�`���[�𐶐����܂�
	d3d11sub_resource_array data;
	std::tie(m_width, m_height) = create_fonts(data, hdc);

	//�I�u�W�F�N�g�̊J��
	SelectObject(hdc, oldFont);
	DeleteObject(hFont);
	ReleaseDC(NULL, hdc);

	HRESULT hr;

	// �t�H���g�e�N�X�`���z�񐶐�
	mTexture.SetDesc(
		m_width, m_height, 1,
		data.num_elements(),
		DXGI_FORMAT_R8G8B8A8_UNORM, 1, 0,
		D3D11_USAGE_DEFAULT, D3D11_BIND_SHADER_RESOURCE);

	hr = mTexture.Create(pdev, data);
	
	if ( FAILED(hr) ) {
		LOG_PRINTF(LOG_ERROR, "ascii_string:\tfailed to create textures (%08x)\n", hr);
		goto ASCII_STRING_CREATE_ERROR;
	}

	// ���_�o�b�t�@�̍쐬
	vertex_t vertices[] = {
		{ 0.0f,    0.0f, 0.0f, 0.0f },
		{ m_width, 0.0f, 1.0f, 0.0f },
		{ 0.0f,    m_height, 0.0f, 1.0f },
		{ m_width, m_height, 1.0f, 1.0f }
	};
	hr = mVertices.Create(pdev, sizeof(vertices), 0, 0, vertices);
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;

	// �C���f�b�N�X�o�b�t�@�̍쐬
	WORD indices[] = { 0, 1, 2, 3, 2, 1 };
	hr = mIndices.Create(pdev, sizeof(indices), 0, 0, indices);
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;

	// �C���X�^���X�f�[�^�̍쐬
	hr = mInstances.Create(
		pdev, sizeof(font_instance_t)*mSource.size(), 0,0);
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;

	// �V�F�[�_�[�̍쐬
	hr = mShader.CreateVS(pdev, "vs_ascii_string.cso", NULL);
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;
	hr = mShader.CreatePS(pdev, "ps_ascii_string.cso", NULL);
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;

	// ���̓��C�A�E�g�̍쐬
	hr = mInputLayout.Create(pdev, mShader.VCode(), mShader.VSize());
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;

	// �T���v���̍쐬
	hr = mSampler.Create(pdev);
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;

	// �u�����h�X�e�[�g�̍쐬
	mBlend.SetState(
		0, TRUE,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);
	hr = mBlend.Create(pdev);
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;

	// �X�e���V���X�e�[�g�̍쐬
	hr = mDepthStencil.Create(pdev);
	if ( FAILED(hr) ) goto ASCII_STRING_CREATE_ERROR;

	return hr;

ASCII_STRING_CREATE_ERROR:
	Release();
	return hr;
}

// �O���t�B�b�N�`��
void ascii_string::Render(ID3D11DeviceContext* context)
{
	if ( m_num_instances<=0 ) return;

	UINT strides, offset(0);
	if ( m_modified ) Sync(context);

	context->VSSetShader(mShader, NULL, 0);
	context->PSSetShader(mShader, NULL, 0);
	context->IASetInputLayout(mInputLayout);
	strides = sizeof(vertex_t);
	context->IASetVertexBuffers(0, 1, mVertices, &strides, &offset);
	strides = sizeof(font_instance_t);
	context->IASetVertexBuffers(1, 1, mInstances, &strides, &offset);
	context->IASetIndexBuffer(mIndices, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->PSSetShaderResources(0, 1, mTexture);
	context->PSSetSamplers(0, 1, mSampler);
	context->OMSetBlendState(mBlend, NULL, 0xffffffff);
	context->OMSetDepthStencilState(mDepthStencil, 0xff);
	context->DrawIndexedInstanced(6, m_num_instances, 0, 0, 0);
}

// �`��F�ݒ�
void ascii_string::SetColor(DWORD color)
{
	m_color[0] = (float)(color & 0xff)/255.0f;
	m_color[1] = (float)(color>>8 & 0xff)/255.0f;
	m_color[2] = (float)(color>>16 & 0xff)/255.0f;
	m_color[3] = (float)(color>>24 & 0xff)/255.0f;

	instance_array_t::iterator it;
	for ( it=mSource.begin(); it!=mSource.end(); ++it ) {
		it->col[0] = m_color[0];
		it->col[1] = m_color[1];
		it->col[2] = m_color[2];
		it->col[3] = m_color[3];
	}

	m_modified = true;
}

// �e�L�X�g�f�[�^���o�b�t�@�ɓ���
void ascii_string::Sync(ID3D11DeviceContext* context)
{
	UINT row_pitch(mSource.size()*sizeof(font_instance_t));
	context->UpdateSubresource(
		mInstances, 0, NULL, &mSource[0], row_pitch, 0);

	m_modified = false;
}

void ascii_string::rand_walk(bool init)
{
	size_t i;

	if ( init ) {
		for ( i=0; i<m_num_instances; i++ ) {
			mSource[i].x = 200.0f;
			mSource[i].y = 200.0f;
		}
		m_modified = true;
		return;
	}

	std::normal_distribution<float> dist(0.0f, 5.0f);
	for ( i=0; i<m_num_instances; i++ ) {
		mSource[i].x += dist(m_gen);
		mSource[i].y += dist(m_gen);
	}
	m_modified = true;
}

// ������`��
void ascii_string::Print(float px, float py, const char* text)
{
	float x(px), y(py);
	char ch;
	size_t i, k;

	for ( i=0, k=m_num_instances; text[i]!=0 && k<mSource.size(); i++ ) {

		ch = text[i];
		if ( ch=='\n' ) {
			x = px;
			y += 1.1f * m_size;
		}
		else if ( ch>=32 && ch<=126 ) {
			mSource[k].x = x;
			mSource[k].y = y;
			mSource[k].scale = 1.0f;
			mSource[k].tex_w = ch - 32;
			x += m_width;
			k++;
		}
	}

	m_num_instances = k;
	m_modified = true;
}

// ���������
void ascii_string::Clear()
{
	m_num_instances = 0;
	m_modified = true;
}

// �t�H���g�e�N�X�`������
std::pair<int, int>
ascii_string::create_fonts(
	d3d11sub_resource_array& out, HDC hdc) const
{
	int tex_w, tex_h, gly_w, gly_h, size, i, x, y;
	UINT ch;
	TEXTMETRIC tm;
	GLYPHMETRICS gm;
	MAT2 mat = {{0,1},{0,0},{0,0},{0,1}};

	// �e�N�X�`���̍ő�T�C�Y���擾
	GetTextMetrics(hdc, &tm);
	tex_w = 0;
	tex_h = tm.tmHeight;
	for ( ch=32; ch<=126; ch++ ) {
		GetGlyphOutline(hdc, ch, GGO_GRAY4_BITMAP, &gm, 0, NULL, &mat);
		if ( tex_w<gm.gmCellIncX ) tex_w = gm.gmCellIncX;
	}

	// �e�N�X�`���o�b�t�@�m��
	out.resize(127-32, sizeof(DWORD)*tex_w*tex_h);
	DWORD *ptr, *px;
	for ( i=0, ch=32; ch<=126; i++, ch++ ) {
		ptr = (DWORD*)out(i)->pSysMem;
		out(i)->SysMemPitch = sizeof(DWORD)*tex_w;
		out(i)->SysMemSlicePitch = sizeof(DWORD)*tex_w*tex_h;
		std::fill(ptr, ptr+tex_w*tex_h, 0x00ffffff);

		// �t�H���g���̎擾
		size = GetGlyphOutline(hdc, ch, GGO_GRAY4_BITMAP, &gm, 0, NULL, &mat);
		if ( size<=0 ) continue;
		gly_h = gm.gmBlackBoxY;
		gly_w = size / gly_h;
		ptr += (tm.tmAscent - gm.gmptGlyphOrigin.y -1)*tex_w;
		
		// �O���t�擾
		BYTE *face = new BYTE[size];
		GetGlyphOutline(hdc, ch, GGO_GRAY4_BITMAP, &gm, size, face, &mat);

		// �A���t�@�l��255/65�{���ď�������
		for ( y=0; y<gly_h; y++ ) {
			px = ptr + y*tex_w;
			for ( x=0; x<gly_w; x++ ) {
				px[x + gm.gmptGlyphOrigin.x]
					= ((DWORD)face[y*gly_w + x]*255)<<20 | 0x00ffffff;
			}
		}

		delete[] face;
	}

	return std::make_pair(tex_w, tex_h);
}
