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

struct FTextureCount
{
	std::string TextureName;
	int32 RefCount;

	FTextureCount()
		:TextureName(""), RefCount(0) {  }
	FTextureCount(std::string InTextureName, int32 InRefCount)
		:TextureName(InTextureName), RefCount(InRefCount) {  }
};

struct FString;

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
	TArray<FTextureCount> UsedTextureNames;
	FString AssetName;

private:
	int GetTextureIndex(FString textureName);

public:
	TArray<FString> GetUsedTextureNames();

public:
	void ChangeMaterial(FString NewAssetName, FString subMeshName, FString materialName);
};

