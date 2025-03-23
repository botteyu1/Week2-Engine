#include "StaticMesh.h"
#include "Object/PrimitiveComponent/TextureComponent.h"

AStaticMesh::AStaticMesh()
{
	bCanEverTick = true;
}

void AStaticMesh::SetMesh(FString MeshType)
{
	UTextureComponent* component = AddComponent<UTextureComponent>();
	component->SetMesh(MeshType);
	component->SetMaterial("TextureMaterial");
	component->AddTexture("Dice.png");
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
