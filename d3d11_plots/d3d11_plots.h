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
//	矩形グリッドクラス
class rect_grid
{
public:
	// コンストラクタ
	rect_grid(BOOL enable_stencil, size_t max_vertices=1024);

	// デストラクタ
	virtual ~rect_grid();

	// リソース開放
	void Release();

	// リソース作成
	HRESULT Create(ID3D11Device* pdev);

	// グラフィック描画
	void Render(ID3D11DeviceContext* context);

	// 描画色設定
	void SetColor(DWORD color);

	// プロットデータをバッファに同期
	void Sync(ID3D11DeviceContext* context);

	// グラフエリアの設定
	void SetArea(float ox, float oy, float width, float height);

	// 目盛り設定
	void SetHTicks(double min, double max, double tick);
	void SetVTicks(double min, double max, double tick);

	// アクセスメソッド
	double Xmin() const		{ return m_xmin;  }
	double Xmax() const		{ return m_xmax;  }
	double Xtick() const	{ return m_xtick; }
	double Ymin() const		{ return m_ymin;  }
	double Ymax() const		{ return m_ymax;  }
	double Ytick() const	{ return m_ytick; }

private:
	// 頂点バッファ定義
	struct vertex_t {
		float	x;
		float	y;
		float	color[4];
	};
	typedef std::vector<vertex_t> vertex_array_t;

	double	m_xmin;			// X軸最小値
	double	m_xmax;			// X軸最大値
	double	m_xtick;		// X軸目盛

	double	m_ymin;			// Y軸最小値
	double	m_ymax;			// Y軸最大値
	double	m_ytick;		// Y軸目盛

	float	m_xoffset;		// プロットエリア位置X
	float	m_yoffset;		// プロットエリア位置Y
	float	m_width;		// プロットエリア幅
	float	m_height;		// プロットエリア高さ
	float	m_color[4];		// 描画色

	int		m_num_vertices;	// 描画対象の頂点数
	bool	m_modified;		// プロット設定変更フラグ

	vertex_array_t				mSource;		// 頂点データ
	d3d11buffer_helper			mVertices;		// 頂点バッファ
	d3d11input_helper			mInputLayout;	// 入力レイアウト
	d3d11shader_helper			mShader;		// シェーダー
	d3d11blend_state			mBlend;			// ブレンドステート
	d3d11depth_stencil_state	mDepthStencil;	// ステンシルステート
};

//------------------------------------------------------------------------
//	データ描画クラス
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

	// コンストラクタ
	rect_trace(int plot_type=PLOT_LINES, size_t max_length=65536);

	// デストラクタ
	virtual ~rect_trace();

	// リソース開放
	void Release();

	// リソース作成
	HRESULT Create(ID3D11Device* pdev);

	// グラフィック描画
	void Render(ID3D11DeviceContext* context);

	// 描画色設定
	void SetColor(DWORD color);

	// プロットデータをバッファに同期
	void Sync(ID3D11DeviceContext* context);

	// グラフエリアの設定
	void SetArea(
		float ox, float oy, float width, float height);

	// レンジ設定
	void SetHRange(double min, double max, bool log_scale=false);
	void SetVRange(double min, double max, bool log_scale=false);

	// データコピー(XYベクトル配列入力)
	void UpdateData(
		ID3D11DeviceContext* context,
		const c_type* xy_data, size_t length);

	// データコピー(XY独立配列入力)
	void UpdateData(
		ID3D11DeviceContext* context,
		const float* x_data, const float* y_data, size_t length);

	// データコピー(Xスカラ配列、Y複素配列入力)
	void UpdateData(
		ID3D11DeviceContext* context,
		const float* x_data, const c_type* y_data, size_t length);

	// データコピー(Xスカラ配列、Y複素配列入力、係数変更)
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

	d3d11buffer_helper			mVertices;			// ポイント表示用ポリゴン
	d3d11buffer_helper			mIndices;			// ポイント表示用インデックスバッファ
	d3d11buffer_helper			mStream0;			// 頂点バッファ0
	d3d11buffer_helper			mStream1;			// 頂点バッファ1
	d3d11buffer_helper			mConstants;			// 定数バッファ
	d3d11input_helper			mInputLayout[16];	// 入力レイアウト
	d3d11shader_helper			mShader[16];		// シェーダー
	d3d11blend_state			mBlend;				// ブレンドステート
	d3d11depth_stencil_state	mDepthStencil;		// ステンシルステート
	plot_constants_t			mPlotConst;

	bool		m_modified;				// プロット設定変更フラグ
	int			mPlotType;				// グラフ形式
	int			mStreamType;			// 入力ストリーム構成
	size_t		mLength;				// 入力データ数
	size_t		mMaxLength;				// 最大データ数
	DWORD		mColor;					// 描画色
	UINT		mStrides0;
	UINT		mStrides1;

	// シェーダーをロードして失敗時に例外送出
	HRESULT create_shader(
		ID3D11Device* pdev, int id, const char* vs_name, const char* ps_name);

	// 入力レイアウトを作成して失敗時に例外送出
	HRESULT create_layout(ID3D11Device* pdev, int id);
};
typedef std::shared_ptr<rect_trace> rect_trace_ptr;
typedef std::shared_ptr<const rect_trace> const_rect_trace_ptr;


//------------------------------------------------------------------------
//	矩形グラフクラス
class rect_plots
{
public:
	// コンストラクタ
	rect_plots();

	// デストラクタ
	virtual ~rect_plots();

	// リソース開放
	void Release();

	// リソース作成
	HRESULT Create(ID3D11Device* pdev, LPCTSTR font, int size);

	// グラフィック描画
	void Render(ID3D11DeviceContext* context);

	// トレース追加
	rect_trace_ptr AddTrace(
		ID3D11Device* pdev, int trace_type, size_t max_length=65536);

	// トレース取得
	rect_trace_ptr operator[](size_t index);

	// 描画色設定
	void SetColor(int id, DWORD color);

	// プロットデータをバッファに同期
	void Sync(ID3D11DeviceContext* context);

	// グラフエリアの設定
	void SetArea(float ox, float oy, float width, float height);

	// 目盛り設定
	void SetHTicks(double min, double max, double tick, bool all_traces=false);
	void SetVTicks(double min, double max, double tick, bool all_traces=false);

private:
	typedef std::vector<rect_trace_ptr> trace_list_t;

	rect_grid		mGrid;		// グリッド
	trace_list_t	mTraces;	// グラフ
	rect_stencil	mStencil;	// クリッピングエリア
	ascii_string	mText;		// テキスト

	float	m_xoffset;		// プロットエリア位置X
	float	m_yoffset;		// プロットエリア位置Y
	float	m_width;		// プロットエリア幅
	float	m_height;		// プロットエリア高さ

	bool	m_modified;		// 描画設定変更フラグ
};

