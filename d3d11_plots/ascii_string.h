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
//	SUB_RESOURCE配列クラス
class d3d11sub_resource_array
{
public:
	typedef std::vector<D3D11_SUBRESOURCE_DATA> resource_array_t;
	typedef std::vector<size_t> size_array_t;

	// デフォルトコンストラクタ
	d3d11sub_resource_array();

	// コンストラクタ
	d3d11sub_resource_array(size_t num_elements, size_t data_size);

	// デストラクタ
	virtual ~d3d11sub_resource_array();

	// リサイズ
	void resize(size_t num_elements, size_t data_size);

	// リソースクリア
	void clear();

	// 各サブリソースサイズの取得
	size_t size(size_t index) const	{ return mSize[index]; }

	// 要素数の取得
	size_t num_elements() const		{ return mResources.size(); }

	// 要素アクセス
	D3D11_SUBRESOURCE_DATA* operator()(size_t index) {
		return &mResources[index];
	}
	const D3D11_SUBRESOURCE_DATA* operator()(size_t index) const {
		return &mResources[index];
	}

	// キャスト演算子
	operator D3D11_SUBRESOURCE_DATA*()		{ return &mResources[0]; }
	operator const D3D11_SUBRESOURCE_DATA*(){ return &mResources[0]; }

private:
	resource_array_t	mResources;
	size_array_t		mSize;

};


//------------------------------------------------------------------------
//	ASCII文字列クラス
class ascii_string
{
public:
	// コンストラクタ
	ascii_string(BOOL enable_stencil, size_t max_len=8192);

	// デストラクタ
	virtual ~ascii_string();

	// リソース開放
	void Release();

	// リソース作成
	HRESULT Create(ID3D11Device* pdev, LPCTSTR font, int size);

	// グラフィック描画
	void Render(ID3D11DeviceContext* context);

	// フォント再構築
	void SetColor(DWORD color);

	// テキストデータをバッファに同期
	void Sync(ID3D11DeviceContext* context);

	// 文字列描画
	void Print(float px, float py, const char* text);

	// 文字列消去
	void Clear();

	// アクセスメソッド
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

	d3d11tex2d_helper			mTexture;		// テクスチャ配列
	d3d11buffer_helper			mVertices;		// 頂点バッファ
	d3d11buffer_helper			mIndices;		// インデックスバッファ
	d3d11buffer_helper			mInstances;		// インスタンス情報
	d3d11shader_helper			mShader;		// シェーダー
	d3d11input_helper			mInputLayout;	// 入力レイアウト
	instance_array_t			mSource;		// インスタンス情報の元データ
	d3d11sampler_state			mSampler;		// サンプラ
	d3d11blend_state			mBlend;			// ブレンドステート
	d3d11depth_stencil_state	mDepthStencil;	// ステンシルステート

	int			m_size;		// フォントサイズ
	tstring		m_font;		// フォント名称
	std::string	m_text;		// 文字列
	int			m_width;
	int			m_height;
	int			m_num_instances;
	float		m_color[4];
	bool		m_modified;

	std::mt19937	m_gen;

	std::pair<int, int>
		create_fonts(d3d11sub_resource_array& out, HDC hdc) const;
};
