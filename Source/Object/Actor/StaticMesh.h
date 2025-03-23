#pragma once
#include "Actor.h"


class AStaticMesh : public AActor
{
	DECLARE_CLASS(AStaticMesh, AActor)

public:
	AStaticMesh();
	void SetMesh(FString MeshType);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};