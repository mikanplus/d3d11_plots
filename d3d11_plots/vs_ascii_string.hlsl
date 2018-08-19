cbuffer const_buffer_t : register(b0)
{
	float screenW;
	float screenH;
	float dummy1;
	float dummy2;
};

struct VS_IN
{
	float2 local : POSITION;
	float2 tex   : TEXCOORD0;
	float4 color : COLOR;
	float4 misc  : TEXCOORD1;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
	float3 tex : TEXCOORD0;
};

VS_OUT main( VS_IN input )
{
	VS_OUT output;

	float3 uv = float3(
		input.tex.x, input.tex.y, input.misc.w);

	float2 world = float2(
		input.misc.z*input.local.x + input.misc.x,
		input.misc.z*input.local.y + input.misc.y);

	output.pos = float4(
		2.0*(world.x/screenW - 0.5),
		2.0*(0.5 - world.y/screenH),
		0.5,
		1.0);

	output.col = input.color;
	output.tex = uv;

	return output;
}
