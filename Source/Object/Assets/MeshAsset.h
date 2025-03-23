#pragma once
#include "Asset.h"
#include "Core/Container/Array.h"
#include "Primitive/PrimitiveVertices.h"

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
};

