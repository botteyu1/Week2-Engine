#include "ObjMTLAsset.h"
#include <fstream>
#include <sstream>
#include "Debug/DebugConsole.h"

bool UObjMTLAsset::RegisterAsset()
{
	return false;
}

bool UObjMTLAsset::Load()
{
	ObjMaterialMap.Empty();
	
	std::ifstream file(GetAssetPath().GetData());
	if (!file.is_open()) 
	{
		MsgBoxAssert("Failed to open MTL File: ");
		return false;
	}

	std::string line;
	FObjMaterial currentMat = FObjMaterial();
	bool materialStarted = false;

	while (std::getline(file, line)) 
	{
		if (line.empty()) continue;

		if (line[0] == '#') continue;

		std::istringstream iss(line);
		std::string keyword;
		iss >> keyword;

		if (keyword == "newmtl") 
		{
			if (materialStarted) 
			{
				ObjMaterialMap.Add(FString(currentMat.Name.c_str()), currentMat);
				currentMat = FObjMaterial();
			}
			else
			{
				materialStarted = true;
			}
			iss >> currentMat.Name;
		}
		else if (!materialStarted) 
		{
			// 아직 머티리얼 정의가 시작되지 않은 경우는 무시
			continue;
		}
		else if (keyword == "Ka")
		{
			iss >> currentMat.Ka[0] >> currentMat.Ka[1] >> currentMat.Ka[2];
		}
		else if (keyword == "Kd")
		{
			iss >> currentMat.Kd[0] >> currentMat.Kd[1] >> currentMat.Kd[2];
		}
		else if (keyword == "Ks")
		{
			iss >> currentMat.Ks[0] >> currentMat.Ks[1] >> currentMat.Ks[2];
		}
		else if (keyword == "Ns")
		{
			iss >> currentMat.Ns;
		}
		else if (keyword == "Ni")
		{
			iss >> currentMat.Ni;
		}
		else if (keyword == "d")
		{
			iss >> currentMat.d;
		}
		else if (keyword == "Tr")
		{
			// Tr는 투명도 반대: d = 1 - Tr
			float tr;
			iss >> tr;
			currentMat.d = 1.0f - tr;
		}
		else if (keyword == "illum")
		{
			iss >> currentMat.illum;
		}
		else if (keyword == "map_Ka")
		{
			iss >> currentMat.map_Ka;
		}
		else if (keyword == "map_Kd")
		{
			iss >> currentMat.map_Kd;
			bool hasEqualName = false;
			for (int i = 0; i < TextureNames.Num(); i++)
			{
				if (TextureNames[i].Equals(currentMat.map_Kd, ESearchCase::IgnoreCase)) {
					currentMat.textureIndex = i;
					hasEqualName = true;
					break;
				}
			}
			if (!hasEqualName)
			{
				TextureNames.Add(currentMat.map_Kd);
				currentMat.textureIndex = TextureNames.Num() - 1;
			}
		}
		else if (keyword == "map_Ks")
		{
			iss >> currentMat.map_Ks;
		}
		else if (keyword == "map_d")
		{
			iss >> currentMat.map_d;
			bool hasEqualName = false;
			for (int i = 0; i < TextureNames.Num(); i++) 
			{
				if (TextureNames[i].Equals(currentMat.map_d, ESearchCase::IgnoreCase)) {
					currentMat.textureIndex = i;
					hasEqualName = true;
					break;
				}
			}
			if (!hasEqualName)
			{
				TextureNames.Add(currentMat.map_d);
				currentMat.textureIndex = TextureNames.Num() - 1;
			}
			// Obj Loarder에서 Vertex에 TextureIndex 넣어줄 때 TextureIndex 바로 넣어줄 수 있도록하기 위해

		}
		else if (keyword == "bump" || keyword == "map_bump")
		{
			iss >> currentMat.map_bump;
		}
		else if (keyword == "disp")
		{
			iss >> currentMat.disp;
		}
		else if (keyword == "decal")
		{
			iss >> currentMat.decal;
		}
	}

	if (materialStarted) 
	{
		ObjMaterialMap.Add(FString(currentMat.Name.c_str()), currentMat);
	}

	file.close();
	return true;
}

bool UObjMTLAsset::Save(FString path)
{
	return false;
}

bool UObjMTLAsset::Unload()
{
	ObjMaterialMap.Empty();
	TextureNames.Empty();
	return false;
}

FObjMaterial* UObjMTLAsset::GetMaterialByName(const FName& MaterialName)
{
	return ObjMaterialMap.Find(MaterialName);
}

const TArray<FString>& UObjMTLAsset::GetTextureNames() const
{
	return TextureNames;
}
