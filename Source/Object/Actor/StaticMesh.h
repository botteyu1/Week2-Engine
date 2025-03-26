#pragma once
#include "Actor.h"

struct FSubMesh;

class AStaticMesh : public AActor
{
	DECLARE_CLASS(AStaticMesh, AActor)

public:
	AStaticMesh();
	void SetMesh(FString MeshType, bool texture);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
	FString GetObjName() { return objName; }
	uint32 GetbUseTexture() { return bUseTexture; }
	void AddMesh(FString MeshType, bool texture);

	void ChangeMaterial(FString subMeshName, FString destMaterialName);
	void SetbUseTexture(bool value);

	TArray<FSubMesh> GetSubMeshes();
private:
	FString objName;
	uint32 bUseTexture;
	FString AssetName;
};