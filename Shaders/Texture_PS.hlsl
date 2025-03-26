Texture2DArray imageArray : register(t0);
SamplerState samLinear : register(s0);

struct VS_OUTPUT
{
	float4 LightColor : LIGHTCOLOR;
	float3 LightDirection : LIGHTDIRECTION;
	float3 Normal : NORMAL;
	float4 Color : COLOR;
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
	int TextIndex : TEXTINDEX;
	float4 UUID : UUID;
};

struct PS_OUTPUT
{
	float4 color : SV_Target0;
	uint4 UUID : SV_Target1;
};

PS_OUTPUT Texture_PS(VS_OUTPUT input, bool isFront : SV_IsFrontFace)
{
	PS_OUTPUT output;
	
	float3 sampleCoord = float3(input.Tex, input.TextIndex);
	float4 sampledColor = imageArray.Sample(samLinear, sampleCoord) * input.Color;
	//float4 sampledColor = float4(1.0f, 0.0f, 0.0f, 1.0f);
	if (!isFront)
	{
		input.Normal = -input.Normal;
	}
	if (length(input.Normal) != 0)
	{
		float3 lightDir = -input.LightDirection;
		float lightIntensity = saturate(dot(lightDir, input.Normal));
		float4 light = saturate(input.LightColor * lightIntensity);
		sampledColor = sampledColor * light;
		sampledColor.a = 1.0f;
	}
	output.color = sampledColor;
	output.UUID = input.UUID;
	return output;  
}