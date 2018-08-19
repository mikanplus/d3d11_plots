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
//	矩形クリッピング
class rect_stencil
{
public:
	// デフォルトコンストラクタ
	rect_stencil();

	// 引数付きコンストラクタ
	rect_stencil(float ox, float oy, float width, float height);

	// デストラクタ
	virtual ~rect_stencil();

	// リソース開放
	void Release();

	// マスクを作成します
	HRESULT Create(ID3D11Device* pdev);

	// グラフィック描画
	void Render(ID3D11DeviceContext* context);

	// クリッピングエリアの変更
	void SetArea(float ox, float oy, float width, float height);

private:
	struct vertex_t {
		float x;
		float y;
	};

	d3d11buffer_helper			mVertices;		// 頂点バッファ
	d3d11buffer_helper			mIndices;		// インデックスバッファ
	d3d11shader_helper			mShader;		// シェーダー
	d3d11input_helper			mInputLayout;	// 入力レイアウト
	d3d11depth_stencil_state	mDepthStencil;	// ステンシルステート
	CD3D11_RECT					mRect;			// クリッピングエリア
	bool						m_modified;		// 変更フラグ
};

