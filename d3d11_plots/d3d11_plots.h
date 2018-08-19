/**
 * d3d11_plots.h
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#pragma once
#include "d3d11_helper.h"
#include "ascii_string.h"
#include "stencil_helper.h"

//------------------------------------------------------------------------
//	��`�O���b�h�N���X
class rect_grid
{
public:
	// �R���X�g���N�^
	rect_grid(BOOL enable_stencil, size_t max_vertices=1024);

	// �f�X�g���N�^
	virtual ~rect_grid();

	// ���\�[�X�J��
	void Release();

	// ���\�[�X�쐬
	HRESULT Create(ID3D11Device* pdev);

	// �O���t�B�b�N�`��
	void Render(ID3D11DeviceContext* context);

	// �`��F�ݒ�
	void SetColor(DWORD color);

	// �v���b�g�f�[�^���o�b�t�@�ɓ���
	void Sync(ID3D11DeviceContext* context);

	// �O���t�G���A�̐ݒ�
	void SetArea(float ox, float oy, float width, float height);

	// �ڐ���ݒ�
	void SetHTicks(double min, double max, double tick);
	void SetVTicks(double min, double max, double tick);

	// �A�N�Z�X���\�b�h
	double Xmin() const		{ return m_xmin;  }
	double Xmax() const		{ return m_xmax;  }
	double Xtick() const	{ return m_xtick; }
	double Ymin() const		{ return m_ymin;  }
	double Ymax() const		{ return m_ymax;  }
	double Ytick() const	{ return m_ytick; }

private:
	// ���_�o�b�t�@��`
	struct vertex_t {
		float	x;
		float	y;
		float	color[4];
	};
	typedef std::vector<vertex_t> vertex_array_t;

	double	m_xmin;			// X���ŏ��l
	double	m_xmax;			// X���ő�l
	double	m_xtick;		// X���ڐ�

	double	m_ymin;			// Y���ŏ��l
	double	m_ymax;			// Y���ő�l
	double	m_ytick;		// Y���ڐ�

	float	m_xoffset;		// �v���b�g�G���A�ʒuX
	float	m_yoffset;		// �v���b�g�G���A�ʒuY
	float	m_width;		// �v���b�g�G���A��
	float	m_height;		// �v���b�g�G���A����
	float	m_color[4];		// �`��F

	int		m_num_vertices;	// �`��Ώۂ̒��_��
	bool	m_modified;		// �v���b�g�ݒ�ύX�t���O

	vertex_array_t				mSource;		// ���_�f�[�^
	d3d11buffer_helper			mVertices;		// ���_�o�b�t�@
	d3d11input_helper			mInputLayout;	// ���̓��C�A�E�g
	d3d11shader_helper			mShader;		// �V�F�[�_�[
	d3d11blend_state			mBlend;			// �u�����h�X�e�[�g
	d3d11depth_stencil_state	mDepthStencil;	// �X�e���V���X�e�[�g
};

//------------------------------------------------------------------------
//	�f�[�^�`��N���X
class rect_trace
{
public:
	typedef std::complex<float> c_type;
	typedef std::complex<double> z_type;
	enum {
		PLOT_LINES			= 0x04,
		PLOT_DOTS			= 0x08,
		INPUT_CTYPE0		= 0x00,
		INPUT_FLOAT0_FLOAT1	= 0x01,
		INPUT_FLOAT0_CTYPE1	= 0x02
	};

	// �R���X�g���N�^
	rect_trace(int plot_type=PLOT_LINES, size_t max_length=65536);

	// �f�X�g���N�^
	virtual ~rect_trace();

	// ���\�[�X�J��
	void Release();

	// ���\�[�X�쐬
	HRESULT Create(ID3D11Device* pdev);

	// �O���t�B�b�N�`��
	void Render(ID3D11DeviceContext* context);

	// �`��F�ݒ�
	void SetColor(DWORD color);

	// �v���b�g�f�[�^���o�b�t�@�ɓ���
	void Sync(ID3D11DeviceContext* context);

	// �O���t�G���A�̐ݒ�
	void SetArea(
		float ox, float oy, float width, float height);

	// �����W�ݒ�
	void SetHRange(double min, double max, bool log_scale=false);
	void SetVRange(double min, double max, bool log_scale=false);

	// �f�[�^�R�s�[(XY�x�N�g���z�����)
	void UpdateData(
		ID3D11DeviceContext* context,
		const c_type* xy_data, size_t length);

	// �f�[�^�R�s�[(XY�Ɨ��z�����)
	void UpdateData(
		ID3D11DeviceContext* context,
		const float* x_data, const float* y_data, size_t length);

	// �f�[�^�R�s�[(X�X�J���z��AY���f�z�����)
	void UpdateData(
		ID3D11DeviceContext* context,
		const float* x_data, const c_type* y_data, size_t length);

	// �f�[�^�R�s�[(X�X�J���z��AY���f�z����́A�W���ύX)
	void UpdateData(
		ID3D11DeviceContext* context,
		const float* x_data, const c_type* y_data, size_t length,
		const c_type& factor);

private:
	struct vertex_t {
		float	x;
		float	y;
		float	u;
		float	v;
	};
	struct plot_constants_t {
		float	left;
		float	top;
		float	width;
		float	height;
		float	h_min;
		float	h_max;
		float	v_min;
		float	v_max;
		float	color[4];
		float	h_log;
		float	h_linear;
		float	v_log;
		float	v_linear;
		c_type	factor;
		c_type	dummy;
	};

	d3d11buffer_helper			mVertices;			// �|�C���g�\���p�|���S��
	d3d11buffer_helper			mIndices;			// �|�C���g�\���p�C���f�b�N�X�o�b�t�@
	d3d11buffer_helper			mStream0;			// ���_�o�b�t�@0
	d3d11buffer_helper			mStream1;			// ���_�o�b�t�@1
	d3d11buffer_helper			mConstants;			// �萔�o�b�t�@
	d3d11input_helper			mInputLayout[16];	// ���̓��C�A�E�g
	d3d11shader_helper			mShader[16];		// �V�F�[�_�[
	d3d11blend_state			mBlend;				// �u�����h�X�e�[�g
	d3d11depth_stencil_state	mDepthStencil;		// �X�e���V���X�e�[�g
	plot_constants_t			mPlotConst;

	bool		m_modified;				// �v���b�g�ݒ�ύX�t���O
	int			mPlotType;				// �O���t�`��
	int			mStreamType;			// ���̓X�g���[���\��
	size_t		mLength;				// ���̓f�[�^��
	size_t		mMaxLength;				// �ő�f�[�^��
	DWORD		mColor;					// �`��F
	UINT		mStrides0;
	UINT		mStrides1;

	// �V�F�[�_�[�����[�h���Ď��s���ɗ�O���o
	HRESULT create_shader(
		ID3D11Device* pdev, int id, const char* vs_name, const char* ps_name);

	// ���̓��C�A�E�g���쐬���Ď��s���ɗ�O���o
	HRESULT create_layout(ID3D11Device* pdev, int id);
};
typedef std::shared_ptr<rect_trace> rect_trace_ptr;
typedef std::shared_ptr<const rect_trace> const_rect_trace_ptr;


//------------------------------------------------------------------------
//	��`�O���t�N���X
class rect_plots
{
public:
	// �R���X�g���N�^
	rect_plots();

	// �f�X�g���N�^
	virtual ~rect_plots();

	// ���\�[�X�J��
	void Release();

	// ���\�[�X�쐬
	HRESULT Create(ID3D11Device* pdev, LPCTSTR font, int size);

	// �O���t�B�b�N�`��
	void Render(ID3D11DeviceContext* context);

	// �g���[�X�ǉ�
	rect_trace_ptr AddTrace(
		ID3D11Device* pdev, int trace_type, size_t max_length=65536);

	// �g���[�X�擾
	rect_trace_ptr operator[](size_t index);

	// �`��F�ݒ�
	void SetColor(int id, DWORD color);

	// �v���b�g�f�[�^���o�b�t�@�ɓ���
	void Sync(ID3D11DeviceContext* context);

	// �O���t�G���A�̐ݒ�
	void SetArea(float ox, float oy, float width, float height);

	// �ڐ���ݒ�
	void SetHTicks(double min, double max, double tick, bool all_traces=false);
	void SetVTicks(double min, double max, double tick, bool all_traces=false);

private:
	typedef std::vector<rect_trace_ptr> trace_list_t;

	rect_grid		mGrid;		// �O���b�h
	trace_list_t	mTraces;	// �O���t
	rect_stencil	mStencil;	// �N���b�s���O�G���A
	ascii_string	mText;		// �e�L�X�g

	float	m_xoffset;		// �v���b�g�G���A�ʒuX
	float	m_yoffset;		// �v���b�g�G���A�ʒuY
	float	m_width;		// �v���b�g�G���A��
	float	m_height;		// �v���b�g�G���A����

	bool	m_modified;		// �`��ݒ�ύX�t���O
};

