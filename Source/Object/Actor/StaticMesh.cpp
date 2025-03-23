#include "StaticMesh.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

AStaticMesh::AStaticMesh()
{
	bCanEverTick = true;
}

void AStaticMesh::SetMesh(FString MeshType)
{
	UPrimitiveComponent* component = AddComponent<UPrimitiveComponent>();
	component->SetMesh(MeshType);
	component->SetMaterial("TextureMaterial");
	RootComponent = component;
	component->SetRelativeTransform(FTransform());
}

void AStaticMesh::BeginPlay()
{
	Super::BeginPlay();
}

void AStaticMesh::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* AStaticMesh::GetTypeName()
{
	return "StaticMesh";
}
