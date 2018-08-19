/**
 * stencil_helper.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once
#include "d3d11_helper.h"

//------------------------------------------------------------------------
//	��`�N���b�s���O
class rect_stencil
{
public:
	// �f�t�H���g�R���X�g���N�^
	rect_stencil();

	// �����t���R���X�g���N�^
	rect_stencil(float ox, float oy, float width, float height);

	// �f�X�g���N�^
	virtual ~rect_stencil();

	// ���\�[�X�J��
	void Release();

	// �}�X�N���쐬���܂�
	HRESULT Create(ID3D11Device* pdev);

	// �O���t�B�b�N�`��
	void Render(ID3D11DeviceContext* context);

	// �N���b�s���O�G���A�̕ύX
	void SetArea(float ox, float oy, float width, float height);

private:
	struct vertex_t {
		float x;
		float y;
	};

	d3d11buffer_helper			mVertices;		// ���_�o�b�t�@
	d3d11buffer_helper			mIndices;		// �C���f�b�N�X�o�b�t�@
	d3d11shader_helper			mShader;		// �V�F�[�_�[
	d3d11input_helper			mInputLayout;	// ���̓��C�A�E�g
	d3d11depth_stencil_state	mDepthStencil;	// �X�e���V���X�e�[�g
	CD3D11_RECT					mRect;			// �N���b�s���O�G���A
	bool						m_modified;		// �ύX�t���O
};

