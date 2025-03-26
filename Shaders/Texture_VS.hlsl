struct VS_INPUT
{
	float3 Position : POSITION; // 3개의 float 값 (DXGI_FORMAT_R32G32B32_FLOAT)
	float4 Color : COLOR; // 4개의 float 값 (DXGI_FORMAT_R32G32B32A32_FLOAT)
	float2 Texcoord : TEXCOORD; // 2개의 float 값 (DXGI_FORMAT_R32G32_FLOAT)
	float3 Normal : NORMAL; // 3개의 float 값 (DXGI_FORMAT_R32G32B32_FLOAT)
	int TextIndex : TEXTINDEX;
};

struct VS_OUTPUT
{
	float4 LightColor :LIGHTCOLOR;
	float3 LightDirection : LIGHTDIRECTION;
	float3 Normal : NORMAL;
	float4 Color : COLOR;
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	int TextIndex : TEXTINDEX;
	float4 UUID : UUID;
	float BreathRatio : BREATHRATIO;
};

cbuffer constants : register(b0)
{
	float4x4 MVP;
	float4 CustomColor;
	float4 UUIDColor;
	float4 LightColor;
	float3 LightDirection;
	uint bUseVertexColor;
	float breathRatio;
};
VS_OUTPUT Texture_VS(VS_INPUT input)
{
	VS_OUTPUT output;

	output.Pos = mul(float4(input.Position.xyz, 1.0f), MVP);
	output.Tex = input.Texcoord;
	output.TextIndex = input.TextIndex;
	output.Color = bUseVertexColor == true ? input.Color : CustomColor;
	output.LightColor = LightColor;
	output.LightDirection = LightDirection;
	output.Normal = input.Normal;
	output.UUID = UUIDColor;
	output.BreathRatio = breathRatio;
	
	return output;
}