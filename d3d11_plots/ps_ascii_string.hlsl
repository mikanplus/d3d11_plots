struct PS_IN {
	float4	pos : SV_POSITION;
	float4	col : COLOR0;
	float3	tex : TEXCOORD0;
};

SamplerState g_sampler : register(s0);
Texture2DArray<float4> g_texture : register(t0);

float4 main(PS_IN input) : SV_TARGET
{
	float4 col = g_texture.Sample(
		g_sampler, float3(input.tex.x, input.tex.y, input.tex.z));
	col *= input.col;
	return col;
}
