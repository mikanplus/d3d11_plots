cbuffer const_buffer_t : register(b0)
{
	float screenW;
	float screenH;
	float dummy1;
	float dummy2;
};

struct VS_IN
{
	float2 pos : POSITION;
	float4 col : COLOR;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float4 col : COLOR0;
};

VS_OUT main( VS_IN input )
{
	VS_OUT output;

	output.pos = float4(
		2.0*(input.pos.x/screenW - 0.5),
		2.0*(0.5 - input.pos.y/screenH),
		0.5,
		1.0);
	output.col = input.col;

	return output;
}
