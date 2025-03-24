#pragma once
#include "Asset.h"
#include <array>
#include <string>
#include "Core/Container/Array.h"

struct FObjMaterial {
	// 머티리얼 이름 (newmtl)
	std::string Name;

	// 색상 정보 (0~1 범위)
	std::array<float, 3> Ka;   // Ka : Ambient color (r, g, b)
	std::array<float, 3> Kd;   // Kd : Diffuse color (r, g, b)
	std::array<float, 3> Ks;  // Ks : Specular color (r, g, b)

	// 머티리얼 속성들
	float Ns;         // Ns : Shininess (광택 정도)
	float Ni; // Ni : 굴절률
	float d;          // d  : 불투명도 (1.0이면 불투명, 0.0이면 완전 투명)
	int   illum; // illum : 조명 모델 번호

	// 텍스처 파일 경로들
	std::string map_Ka;  // map_Ka : Ambient texture map
	std::string map_Kd;  // map_Kd : Diffuse texture map
	std::string map_Ks; // map_Ks : Specular texture map
	std::string map_d;    // map_d  : Alpha (투명도) texture map
	int textureIndex;
	std::string map_bump;     // bump 또는 map_bump : Bump 또는 Normal map
	std::string disp; // disp : Displacement map
	std::string decal;        // decal : Decal map

	// 생성자: 기본값 초기화
	FObjMaterial()
		: Name(""),
		Ka({ 0.0f, 0.0f, 0.0f }),
		Kd({ 0.0f, 0.0f, 0.0f }),
		Ks({ 0.0f, 0.0f, 0.0f }),
		Ns(0.0f),
		Ni(1.0f),
		d(1.0f),
		illum(2), // 기본적으로 Phong 조명 모델 (illum 2)
		map_Ka(""),
		map_Kd(""),
		map_Ks(""),
		map_d(""),
		textureIndex(-1), // Texture 없으면 -1 존재하면 0 ~ N
		map_bump(""),
		disp(""),
		decal("")
	{
	}
};

class UObjMTLAsset : public UAsset 
{
	DECLARE_CLASS(UObjMTLAsset, UAsset)

public:
	UObjMTLAsset() = default;
	// UAsset을 통해 상속
	bool RegisterAsset() override;
	bool Load() override;
	bool Save(FString path = "") override;
	bool Unload() override;

private:
	TArray<FObjMaterial> ObjMaterials;
	TArray<FString> TextureNames;
};