#pragma once
#include "Asset.h"
#include "Core/Container/Array.h"
#include "Primitive/PrimitiveVertices.h"

struct FSubMesh 
{
	std::string SubMeshName;
	std::string MaterialName;

	FSubMesh()
		:SubMeshName(""), MaterialName("") { }
};

class UMeshAsset : public UAsset
{
	DECLARE_CLASS(UMeshAsset, UAsset)

public:
	UMeshAsset() = default;
	virtual bool RegisterAsset() override;
	virtual bool Load() override;
	virtual bool Save(FString path = "") override;
	virtual bool Unload() override;

private:
	FGeometryData GeometryData;
	TArray<FSubMesh> SubMeshes;
	TArray<FString> UsedTextureNames;

private:
	int GetTextureIndex(FString textureName);

public:
	TArray<FString> GetUsedTextureNames() const { return UsedTextureNames; }
};

