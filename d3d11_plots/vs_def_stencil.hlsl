cbuffer const_buffer_t : register(b0)
{
	float screenW;
	float screenH;
	float dummy1;
	float dummy2;
};

float4 main( float2 pos : POSITION ) : SV_POSITION
{
	float4 output;

	output = float4(
		2.0*(pos.x/screenW - 0.5),
		2.0*(0.5 - pos.y/screenH),
		0.5,
		1.0);

	return output;
}
