Texture2DArray imageArray : register(t4);
SamplerState samLinear : register(s0);

struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	int TextIndex : TEXTINDEX;
};

float4 Texture_PS(VS_OUTPUT input) : SV_TARGET
{
	float3 sampleCoord = float3(input.Tex, input.TextIndex);
	float4 sampledColor = imageArray.Sample(samLinear, sampleCoord);
	//float4 sampledColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	return sampledColor;  
}