/**
 * d3d11_plots.cpp
 *
 * Copyright (c) 2018 mikanplus(http://www.usamimi.info/~mikanplus/)
 *
 * This software is released under the MIT License.
 * http://opensource.org/licenses/mit-license.php
 */

#include "stdafx.h"
#include "d3d11_plots.h"


//------------------------------------------------------------------------
//	矩形グリッドクラス

// コンストラクタ
rect_grid::rect_grid(BOOL enable_stencil, size_t max_vertices)
	: m_xmin(0.0),
	m_xmax(1.0),
	m_xtick(0.2),
	m_ymin(0.0),
	m_ymax(1.0),
	m_ytick(0.2),
	m_xoffset(0.0f),
	m_yoffset(0.0f),
	m_width(320.0f),
	m_height(240.0f),
	m_modified(true),
	mVertices(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER),
	mBlend(FALSE, FALSE),
	mDepthStencil(FALSE, enable_stencil)
{
	mSource.resize(max_vertices);
	memset(&mSource[0], 0, sizeof(vertex_t)*max_vertices);

	mInputLayout.Add(
		"POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	mInputLayout.Add(
		"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D11_INPUT_PER_VERTEX_DATA, 0);
}

// デストラクタ
rect_grid::~rect_grid()
{
	Release();
}

// リソース開放
void rect_grid::Release()
{
	mDepthStencil.Release();
	mBlend.Release();
	mShader.Release();
	mInputLayout.Release();
	mVertices.Release();
}

// リソース作成
HRESULT rect_grid::Create(ID3D11Device* pdev)
{
	HRESULT hr;

	// 頂点バッファの作成
	hr = mVertices.Create(
		pdev, sizeof(vertex_t)*mSource.size(), 0, 0);
	if ( FAILED(hr) ) goto RECT_GRID_CREATE_ERROR;

	// シェーダーの作成
	hr = mShader.CreateVS(pdev, "vs_rect_grid.cso", NULL);
	if ( FAILED(hr) ) goto RECT_GRID_CREATE_ERROR;
	hr = mShader.CreatePS(pdev, "ps_rect_grid.cso", NULL);
	if ( FAILED(hr) ) goto RECT_GRID_CREATE_ERROR;

	// 入力レイアウトの作成
	hr = mInputLayout.Create(pdev, mShader.VCode(), mShader.VSize());
	if ( FAILED(hr) ) goto RECT_GRID_CREATE_ERROR;

	// ブレンドステートの作成
	mBlend.SetState(
		0, TRUE,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);
	hr = mBlend.Create(pdev);
	if ( FAILED(hr) ) goto RECT_GRID_CREATE_ERROR;

	// ステンシルステートの作成
	hr = mDepthStencil.Create(pdev);
	if ( FAILED(hr) ) goto RECT_GRID_CREATE_ERROR;

	return hr;

RECT_GRID_CREATE_ERROR:
	Release();
	return hr;
}

// グラフィック描画
void rect_grid::Render(ID3D11DeviceContext* context)
{
	UINT strides, offset(0);
	if ( m_modified ) Sync(context);
	if ( m_num_vertices<2 ) return;

	context->VSSetShader(mShader, NULL, 0);
	context->PSSetShader(mShader, NULL, 0);
	context->IASetInputLayout(mInputLayout);
	strides = sizeof(vertex_t);
	context->IASetVertexBuffers(0, 1, mVertices, &strides, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	context->PSSetShaderResources(0, 0, NULL);
	context->PSSetSamplers(0, 0, NULL);
	context->OMSetBlendState(mBlend, NULL, 0xffffffff);
	context->OMSetDepthStencilState(mDepthStencil, 0xff);
	context->Draw(m_num_vertices, 0);
}

// フォント再構築
void rect_grid::SetColor(DWORD color)
{
	m_color[0] = (float)(color & 0xff)/255.0f;
	m_color[1] = (float)(color>>8 & 0xff)/255.0f;
	m_color[2] = (float)(color>>16 & 0xff)/255.0f;
	m_color[3] = (float)(color>>24 & 0xff)/255.0f;
	m_modified = true;
}

// プロットデータをバッファに同期
void rect_grid::Sync(ID3D11DeviceContext* context)
{
	double t, dx, dy;
	int i, k, num_xtick, num_ytick;

	// X軸目盛の本数を算出→Xスケール算出
	t = std::abs((m_xmax - m_xmin)/m_xtick);
	dx = m_width / t;
	num_xtick = std::floor(t);

	// Y軸目盛の本数を算出→Yスケール算出
	t = std::abs((m_ymax - m_ymin)/m_ytick);
	dy = m_height / t;
	num_ytick = std::floor(t);

	vertex_t* ptr = &mSource[0];
	k = 0;

	// X軸目盛の頂点設定
	t = m_xoffset;
	for ( i=0; i<num_xtick && k<mSource.size(); i++ ) {
		ptr[k].x = t;
		ptr[k++].y = m_yoffset;
		ptr[k].x = t;
		ptr[k++].y = m_yoffset + m_height;
		t += dx;
	}

	// Y軸目盛の頂点設定
	t = m_yoffset + m_height;
	for ( i=0; i<num_ytick && k<mSource.size(); i++ ) {
		ptr[k].x = m_xoffset;
		ptr[k++].y = t;
		ptr[k].x = m_xoffset + m_width;
		ptr[k++].y = t;
		t -= dy;
	}

	// 外枠右側
	if ( k<mSource.size() ) {
		ptr[k].x = m_xoffset + m_width;
		ptr[k++].y = m_yoffset;
		ptr[k].x = m_xoffset + m_width;
		ptr[k++].y = m_yoffset + m_height;
	}

	// 外枠上側
	if ( k<mSource.size() ) {
		ptr[k].x = m_xoffset;
		ptr[k++].y = m_yoffset;
		ptr[k].x = m_xoffset + m_width;
		ptr[k++].y = m_yoffset;
	}

	// 描画色設定
	for ( i=0; i<k; i++ ) {
		ptr[i].color[0] = m_color[0];
		ptr[i].color[1] = m_color[1];
		ptr[i].color[2] = m_color[2];
		ptr[i].color[3] = m_color[3];
	}

	// バッファ更新
	CD3D11_BOX box(0, 0, 0, sizeof(vertex_t)*k, 1, 1);
	context->UpdateSubresource(
		mVertices, 0, &box, &mSource[0], sizeof(vertex_t)*k, 0);

	m_num_vertices = k;
	m_modified = false;
}

// グラフエリアの設定
void rect_grid::SetArea(float ox, float oy, float width, float height)
{
	m_xoffset = ox;
	m_yoffset = oy;
	m_width = width;
	m_height = height;
	m_modified = true;
}

// 目盛り設定
void rect_grid::SetHTicks(double min, double max, double tick)
{
	m_xmin = min;
	m_xmax = max;
	m_xtick = tick;
	m_modified = true;
}
void rect_grid::SetVTicks(double min, double max, double tick)
{
	m_ymin = min;
	m_ymax = max;
	m_ytick = tick;
	m_modified = true;
}


//------------------------------------------------------------------------
//	データ描画クラス

// コンストラクタ
rect_trace::rect_trace(int plot_type, size_t max_length)
	: mVertices(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER),
	mIndices(D3D11_USAGE_DEFAULT, D3D11_BIND_INDEX_BUFFER),
	mStream0(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER),
	mStream1(D3D11_USAGE_DEFAULT, D3D11_BIND_VERTEX_BUFFER),
	mConstants(D3D11_USAGE_DEFAULT, D3D11_BIND_CONSTANT_BUFFER),
	mBlend(FALSE, FALSE),
	mDepthStencil(FALSE, TRUE),
	mPlotType(plot_type),
	mStreamType(INPUT_CTYPE0),
	mLength(0),
	mMaxLength(max_length),
	mColor(0xff00ffff),
	m_modified(false)
{
	mPlotConst.left = 0.0f;
	mPlotConst.top = 0.0f;
	mPlotConst.width = 320.0f;
	mPlotConst.height = 240.0f;
	mPlotConst.h_min = -1.0f;
	mPlotConst.h_max = 1.0f;
	mPlotConst.v_min = -1.0f;
	mPlotConst.v_max = 1.0f;
	mPlotConst.h_log = 0.0f;
	mPlotConst.h_linear = 1.0f;
	mPlotConst.v_log = 0.0f;
	mPlotConst.v_linear = 1.0f;
	mPlotConst.factor = c_type(1.0, 0.0);
	SetColor(mColor);
}

// デストラクタ
rect_trace::~rect_trace()
{
	Release();
}

// リソース開放
void rect_trace::Release()
{
	mDepthStencil.Release();
	mBlend.Release();
	for ( int i=0; i<16; i++ ) {
		mShader[i].Release();
		mInputLayout[i].Release();
	}
	mConstants.Release();
	mStream1.Release();
	mStream0.Release();
	mIndices.Release();
	mVertices.Release();
}

// リソース作成
HRESULT rect_trace::Create(ID3D11Device* pdev)
{
	HRESULT hr;

	// 頂点バッファの作成
	vertex_t vertices[] = {
		{ -0.5f, -0.5f, 0.0f, 0.0f },
		{  0.5f, -0.5f, 1.0f, 0.0f },
		{ -0.5f,  0.5f, 0.0f, 1.0f },
		{  0.5f,  0.5f, 1.0f, 1.0f }
	};
	hr = mVertices.Create(pdev, sizeof(vertices), 0, 0, vertices);
	if ( FAILED(hr) ) goto RECT_TRACE_CREATE_ERROR;

	// インデックスバッファの作成
	WORD indices[] = { 0, 1, 2, 3, 2, 1 };
	hr = mIndices.Create(pdev, sizeof(indices), 0, 0, indices);
	if ( FAILED(hr) ) goto RECT_TRACE_CREATE_ERROR;

	// 頂点バッファ0
	hr = mStream0.Create(pdev, sizeof(c_type)*mMaxLength, 0, 0);
	if ( FAILED(hr) ) goto RECT_TRACE_CREATE_ERROR;

	// 頂点バッファ1
	hr = mStream1.Create(pdev, sizeof(c_type)*mMaxLength, 0, 0);
	if ( FAILED(hr) ) goto RECT_TRACE_CREATE_ERROR;

	// 定数バッファの作成
	hr = mConstants.Create(pdev, sizeof(mPlotConst), 0, 0);
	if ( FAILED(hr) ) goto RECT_TRACE_CREATE_ERROR;


	try {
		// シェーダー(線グラフ、XYベクトル配列入力)
		create_shader(pdev, PLOT_LINES|INPUT_CTYPE0,
			"vs_plot_using_ctype0.cso", "ps_plot.cso");
		
		// シェーダー(線グラフ、XY独立配列入力)
		create_shader(pdev, PLOT_LINES|INPUT_FLOAT0_FLOAT1,
			"vs_plot_using_float01.cso", "ps_plot.cso");
		
		// シェーダー(線グラフ、Xスカラ配列、Y複素配列入力)
		create_shader(pdev, PLOT_LINES|INPUT_FLOAT0_CTYPE1,
			"vs_plot_using_float0_ctype1.cso", "ps_plot.cso");
		
		// シェーダー(散布図、XYベクトル配列入力)
		create_shader(pdev, PLOT_DOTS|INPUT_CTYPE0,
			"vs_plot_using_ctype0.cso", "ps_plot.cso");
		
		// シェーダー(散布図、XY独立配列入力)
		create_shader(pdev, PLOT_DOTS|INPUT_FLOAT0_FLOAT1,
			"vs_plot_using_float01.cso", "ps_plot.cso");
	}
	catch ( HRESULT hr ) {
		goto RECT_TRACE_CREATE_ERROR;
	}


	// 線グラフ、XYベクトル配列入力
	mInputLayout[PLOT_LINES|INPUT_CTYPE0]
		.Add("POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	// 線グラフ、XY独立配列入力
	mInputLayout[PLOT_LINES|INPUT_FLOAT0_FLOAT1]
		.Add("POSITION", 0, DXGI_FORMAT_R32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0)
		.Add("POSITION", 1, DXGI_FORMAT_R32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	// 線グラフ、Xスカラ配列、Y複素配列入力
	mInputLayout[PLOT_LINES|INPUT_FLOAT0_CTYPE1]
		.Add("POSITION", 0, DXGI_FORMAT_R32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0)
		.Add("POSITION", 1, DXGI_FORMAT_R32G32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	// 散布図、XYベクトル配列入力
	mInputLayout[PLOT_DOTS|INPUT_CTYPE0]
		.Add("POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	// 散布図、XY独立配列入力
	mInputLayout[PLOT_DOTS|INPUT_FLOAT0_FLOAT1]
		.Add("POSITION", 0, DXGI_FORMAT_R32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0)
		.Add("POSITION", 1, DXGI_FORMAT_R32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);

	// 入力レイアウトの一括作成
	try {
		create_layout(pdev, PLOT_LINES|INPUT_CTYPE0);
		create_layout(pdev, PLOT_LINES|INPUT_FLOAT0_FLOAT1);
		create_layout(pdev, PLOT_LINES|INPUT_FLOAT0_CTYPE1);
		create_layout(pdev, PLOT_DOTS|INPUT_CTYPE0);
		create_layout(pdev, PLOT_DOTS|INPUT_FLOAT0_FLOAT1);
	}
	catch ( HRESULT hr ) {
		goto RECT_TRACE_CREATE_ERROR;
	}

	// ブレンドステートの作成
	mBlend.SetState(
		0, TRUE,
		D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_ONE, D3D11_BLEND_OP_ADD);
	hr = mBlend.Create(pdev);
	if ( FAILED(hr) ) goto RECT_TRACE_CREATE_ERROR;

	// ステンシルステートの作成
	hr = mDepthStencil.Create(pdev);
	if ( FAILED(hr) ) goto RECT_TRACE_CREATE_ERROR;

	return hr;

RECT_TRACE_CREATE_ERROR:
	Release();
	return hr;
}

// グラフィック描画
void rect_trace::Render(ID3D11DeviceContext* context)
{
	// データ不足
	if ( mLength<=2 ) return;

	// プロット設定に変更がある場合は定数バッファ更新
	if ( m_modified ) Sync(context);

	UINT offset(0);
	int id = mPlotType | mStreamType;
	D3D_PRIMITIVE_TOPOLOGY topology;

	// 入力データのトポロジ設定と組み合わせフィルタ
	switch ( id ) {
	case PLOT_LINES|INPUT_CTYPE0:
	case PLOT_LINES|INPUT_FLOAT0_FLOAT1:
	case PLOT_LINES|INPUT_FLOAT0_CTYPE1:
		topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
		break;

	case PLOT_DOTS|INPUT_CTYPE0:
	case PLOT_DOTS|INPUT_FLOAT0_FLOAT1:
		topology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
		break;

	default:
		return;
	}

	// レンダリング
	context->VSSetShader(mShader[id], NULL, 0);
	context->PSSetShader(mShader[id], NULL, 0);
	context->VSSetConstantBuffers(1, 1, mConstants);
	context->IASetInputLayout(mInputLayout[id]);
	context->IASetVertexBuffers(0, 1, mStream0, &mStrides0, &offset);
	context->IASetVertexBuffers(1, 1, mStream1, &mStrides1, &offset);
//	context->IASetIndexBuffer(mIndices, DXGI_FORMAT_R16_UINT, 0);
	context->IASetPrimitiveTopology(topology);
	context->OMSetBlendState(mBlend, NULL, 0xffffffff);
	context->OMSetDepthStencilState(mDepthStencil, 0xff);
	context->Draw(mLength, 0);
}

// 描画色設定
void rect_trace::SetColor(DWORD color)
{
	mColor = color;
	mPlotConst.color[0] = (float)(color & 0xff)/255.0f;
	mPlotConst.color[1] = (float)(color>>8 & 0xff)/255.0f;
	mPlotConst.color[2] = (float)(color>>16 & 0xff)/255.0f;
	mPlotConst.color[3] = (float)(color>>24 & 0xff)/255.0f;
	m_modified = true;
}

// プロットデータをバッファに同期
void rect_trace::Sync(ID3D11DeviceContext* context)
{
	context->UpdateSubresource(
		mConstants, 0, NULL, &mPlotConst, sizeof(mPlotConst), 0);
	m_modified = false;
}

// グラフエリアの設定
void rect_trace::SetArea(
	float ox, float oy, float width, float height)
{
	mPlotConst.left = ox;
	mPlotConst.top = oy;
	mPlotConst.width = width;
	mPlotConst.height = height;
	m_modified = true;
}

// レンジ設定
void rect_trace::SetHRange(double min, double max, bool log_scale)
{
	mPlotConst.h_min = min;
	mPlotConst.h_max = max;

	if ( log_scale ) {
		mPlotConst.h_log = 1.0f;
		mPlotConst.h_linear = 0.0f;
	}
	else {
		mPlotConst.h_log = 0.0f;
		mPlotConst.h_linear = 1.0f;
	}
	
	m_modified = true;
}
void rect_trace::SetVRange(double min, double max, bool log_scale)
{
	mPlotConst.v_min = min;
	mPlotConst.v_max = max;

	if ( log_scale ) {
		mPlotConst.v_log = 1.0f;
		mPlotConst.v_linear = 0.0f;
	}
	else {
		mPlotConst.v_log = 0.0f;
		mPlotConst.v_linear = 1.0f;
	}
	
	m_modified = true;
}

// データコピー(XYベクトル配列入力)
void rect_trace::UpdateData(
	ID3D11DeviceContext* context,
	const c_type* xy_data, size_t length)
{
	size_t n;

	// 描画データ数を制限
	n = (length<mMaxLength) ? length : mMaxLength;

	// 頂点バッファ(stream0)にコピー
	CD3D11_BOX box(0, 0, 0, sizeof(c_type)*n, 1, 1);
	if ( xy_data!=NULL ) {
		context->UpdateSubresource(
			mStream0, 0, &box, xy_data, sizeof(c_type)*n, 0);
	}

	// 入力データ構造を記録
	mStreamType = INPUT_CTYPE0;
	mStrides0 = sizeof(c_type);
	mStrides1 = sizeof(float);
	mLength = n;
}

// データコピー(XY独立配列入力)
void rect_trace::UpdateData(
	ID3D11DeviceContext* context,
	const float* x_data, const float* y_data, size_t length)
{
	size_t n;

	// 描画データ数を制限
	n = (length<mMaxLength) ? length : mMaxLength;

	// 頂点バッファ(stream0)にコピー
	CD3D11_BOX box(0, 0, 0, sizeof(float)*n, 1, 1);
	if ( x_data!=NULL ) {
		context->UpdateSubresource(
			mStream0, 0, &box, x_data, sizeof(float)*n, 0);
	}

	// 頂点バッファ(stream1)にコピー
	if ( y_data!=NULL ) {
		context->UpdateSubresource(
			mStream1, 0, &box, y_data, sizeof(float)*n, 0);
	}

	// 入力データ構造を記録
	mStreamType = INPUT_FLOAT0_FLOAT1;
	mStrides0 = sizeof(float);
	mStrides1 = sizeof(float);
	mLength = n;
}

// データコピー(Xスカラ配列、Y複素配列入力)
void rect_trace::UpdateData(
	ID3D11DeviceContext* context,
	const float* x_data, const c_type* y_data, size_t length)
{
	UpdateData(context, x_data, y_data, length, mPlotConst.factor);
}

// データコピー(Xスカラ配列、Y複素配列入力、係数変更)
void rect_trace::UpdateData(
	ID3D11DeviceContext* context,
	const float* x_data, const c_type* y_data, size_t length,
	const c_type& factor)
{
	size_t n;

	// 描画データ数を制限
	n = (length<mMaxLength) ? length : mMaxLength;

	// 係数設定
	if ( mPlotConst.factor!=factor ) {
		mPlotConst.factor = factor;
		m_modified = true;
	}

	// 頂点バッファ(stream0)にコピー
	CD3D11_BOX box(0, 0, 0, sizeof(float)*n, 1, 1);
	if ( x_data!=NULL ) {
		context->UpdateSubresource(
			mStream0, 0, &box, x_data, sizeof(float)*n, 0);
	}

	// 頂点バッファ(stream1)にコピー
	if ( y_data!=NULL ) {
		box.right = sizeof(c_type)*n;
		context->UpdateSubresource(
			mStream1, 0, &box, y_data, sizeof(c_type)*n, 0);
	}

	// 入力データ構造を記録
	mStreamType = INPUT_FLOAT0_CTYPE1;
	mStrides0 = sizeof(float);
	mStrides1 = sizeof(c_type);
	mLength = n;
}

// シェーダーをロードして失敗時に例外送出
HRESULT rect_trace::create_shader(
	ID3D11Device* pdev, int id, const char* vs_name, const char* ps_name)
{
	HRESULT hr;

	hr = mShader[id].CreateVS(pdev, vs_name, NULL);
	if ( FAILED(hr) ) throw hr;
	hr = mShader[id].CreatePS(pdev, ps_name, NULL);
	if ( FAILED(hr) ) throw hr;

	return hr;
}

// 入力レイアウトを作成して失敗時に例外送出
HRESULT rect_trace::create_layout(ID3D11Device* pdev, int id)
{
	HRESULT hr = mInputLayout[id].Create(
		pdev, mShader[id].VCode(), mShader[id].VSize());
	if ( FAILED(hr) ) throw hr;
	return hr;
}


//------------------------------------------------------------------------
//	矩形グラフクラス

// コンストラクタ
rect_plots::rect_plots()
	: mGrid(FALSE),
	mText(FALSE),
	m_xoffset(0.0f),
	m_yoffset(0.0f),
	m_width(480.0f),
	m_height(320.0f)
{
}

// デストラクタ
rect_plots::~rect_plots()
{
	Release();
}

// リソース開放
void rect_plots::Release()
{
	mText.Release();
	mGrid.Release();
	mStencil.Release();
	mTraces.clear();
}

// リソース作成
HRESULT rect_plots::Create(ID3D11Device* pdev, LPCTSTR font, int size)
{
	HRESULT hr;

	hr = mGrid.Create(pdev);
	if ( FAILED(hr) ) goto RECT_PLOTS_CREATE_ERROR;
	hr = mText.Create(pdev, font, size);
	if ( FAILED(hr) ) goto RECT_PLOTS_CREATE_ERROR;
	hr = mStencil.Create(pdev);
	if ( FAILED(hr) ) goto RECT_PLOTS_CREATE_ERROR;

	return hr;

RECT_PLOTS_CREATE_ERROR:
	Release();
	return hr;
}

// グラフィック描画
void rect_plots::Render(ID3D11DeviceContext* context)
{
	if ( m_modified ) Sync(context);

	mStencil.Render(context);
	mGrid.Render(context);
	mText.Render(context);

	trace_list_t::iterator it;
	for ( it=mTraces.begin(); it!=mTraces.end(); ++it ) {
		(*it)->Render(context);
	}
}

// トレース追加
rect_trace_ptr rect_plots::AddTrace(
	ID3D11Device* pdev, int trace_type, size_t max_length)
{
	mTraces.push_back(
		rect_trace_ptr(new rect_trace(trace_type, max_length)) );
	
	mTraces.back()->Create(pdev);
	mTraces.back()->SetArea(m_xoffset, m_yoffset, m_width, m_height);
	mTraces.back()->SetHRange(mGrid.Xmin(), mGrid.Xmax());
	mTraces.back()->SetVRange(mGrid.Ymin(), mGrid.Ymax());

	return mTraces.back();
}

// トレース取得
rect_trace_ptr rect_plots::operator[](size_t index)
{
	return (index<mTraces.size()) ? mTraces[index] : rect_trace_ptr(nullptr);
}

// 描画色設定
void rect_plots::SetColor(int id, DWORD color)
{
	switch ( id ) {
	case -1:
		mGrid.SetColor(color);
		mText.SetColor(color);
		break;

	default:
		if ( id>=0 && id<mTraces.size() )
			mTraces[id]->SetColor(color);
		break;
	}
}

// プロットデータをバッファに同期
void rect_plots::Sync(ID3D11DeviceContext* context)
{
	char buf[1024];
	double t, x, y, dx, dy;
	int num_xtick, num_ytick, i, k;

	// XY目盛り情報を算出
	t = std::abs((mGrid.Xmax() - mGrid.Xmin())/mGrid.Xtick());
	dx = (double)m_width / t;
	num_xtick = std::floor(t);
	t = std::abs((mGrid.Ymax() - mGrid.Ymin())/mGrid.Ytick());
	dy = (double)m_height / t;
	num_ytick = std::floor(t);

	// X軸目盛りのテキストを生成
	mText.Clear();
	t = mGrid.Xmin();
	x = m_xoffset;
	y = m_yoffset + m_height + 0.1f*mText.FontHeight();
	for ( i=0; i<num_xtick; i++ ) {
		sprintf_s(buf, "%g\n", t);
		mText.Print(x, y, buf);
		t += mGrid.Xtick();
		x += dx;
	}

	// Y軸目盛りのテキストを生成
	t = mGrid.Ymin();
	x = m_xoffset;
	y = m_yoffset + m_height - mText.FontHeight();
	for ( i=0; i<num_ytick; i++ ) {
		k = sprintf_s(buf, "%g\n", t);
		mText.Print(x-k*mText.FontWidth(), y, buf);
		t += mGrid.Ytick();
		y -= dy;
	}

	m_modified = false;
}

// グラフエリアの設定
void rect_plots::SetArea(float ox, float oy, float width, float height)
{
	mGrid.SetArea(ox, oy, width, height);
	mStencil.SetArea(ox, oy, width, height);

	trace_list_t::iterator it;
	for ( it=mTraces.begin(); it!=mTraces.end(); ++it ) {
		(*it)->SetArea(ox, oy, width, height);
	}

	m_xoffset = ox;
	m_yoffset = oy;
	m_width = width;
	m_height = height;
	m_modified = true;
}

// 目盛り設定
void rect_plots::SetHTicks(
	double min, double max, double tick, bool all_traces)
{
	mGrid.SetHTicks(min, max, tick);

	if ( all_traces ) {
		trace_list_t::iterator it;
		for ( it=mTraces.begin(); it!=mTraces.end(); ++it ) {
			(*it)->SetHRange(min, max);
		}
	}

	m_modified = true;
}
void rect_plots::SetVTicks(
	double min, double max, double tick, bool all_traces)
{
	mGrid.SetVTicks(min, max, tick);

	if ( all_traces ) {
		trace_list_t::iterator it;
		for ( it=mTraces.begin(); it!=mTraces.end(); ++it ) {
			(*it)->SetVRange(min, max);
		}
	}

	m_modified = true;
}
