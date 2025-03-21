Texture2D image : register(t2);
SamplerState samLinear : register(s0);

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

float4 Texture_PS(VS_OUTPUT input) : SV_TARGET
{
	float4 sampledColor = image.Sample(samLinear, input.Tex);
	//float4 sampledColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return sampledColor;  
}