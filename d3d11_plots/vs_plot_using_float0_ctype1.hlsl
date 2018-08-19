// スクリーン設定
cbuffer const_buffer_t : register(b0)
{
	float screenW;		// スクリーン幅
	float screenH;		// スクリーン高
	float dummy1;
	float dummy2;
};

// グラフ表示設定
cbuffer plot_constants_t : register(b1)
{
	float2	plot_pos;	// グラフ表示位置
	float2	plot_size;	// グラフサイズ
	float	h_min;		// 横軸最小値
	float	h_max;		// 横軸最大値
	float	v_min;		// 縦軸最小値
	float	v_max;		// 縦軸最大値
	float4	plot_color;	// 描画色
	float	h_log;		// 水平軸Log
	float	h_linear;	// 水平軸Linear
	float	v_log;		// 垂直軸Log
	float	v_linear;	// 垂直軸Linear
	float2	factor;		// 係数
	float2	dummy;
};

struct VS_IN
{
	float	x   : POSITION0;
	float2	vec : POSITION1;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};

VS_OUT main( VS_IN input )
{
	VS_OUT output;
	float2 world;
	float h_range, v_range, x, y;

	h_range = h_max - h_min;
	v_range = v_max - v_min;
	y = factor.x*input.vec.x + factor.y*input.vec.y;
	y = v_log*log10(dot(input.vec, input.vec)) + v_linear*y;
	x = h_log*log10(input.x*input.x) + h_linear*input.x;

	world.x = plot_size.x * (x - h_min) / h_range + plot_pos.x;
	world.y = -plot_size.y * (y - v_min) / v_range + plot_pos.y + plot_size.y;

	output.pos = float4(
		2.0*(world.x/screenW - 0.5),
		2.0*(0.5 - world.y/screenH),
		0.5,
		1.0);

	output.col = plot_color;
	return output;
}
