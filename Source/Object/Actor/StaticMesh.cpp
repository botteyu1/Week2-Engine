#include "StaticMesh.h"
#include "Object/PrimitiveComponent/TextureComponent.h"

AStaticMesh::AStaticMesh()
{
	bCanEverTick = true;
}

void AStaticMesh::SetMesh(FString MeshType, bool texture)
{
	UTextureComponent* component = AddComponent<UTextureComponent>();
	objName = MeshType;
	std::string assetName = MeshType.GetData();
	assetName = assetName.substr(0, assetName.size() - 4);
	component->SetMesh(MeshType);
	if (texture) {
		component->SetMaterial("TextureMaterial");
		component->AddTexture(assetName + ".textArray");
		bUseTexture = 1;
	}
	else {
		component->SetMaterial("DefaultMaterial");
		bUseTexture = 0;  
	}
#if IS_OBJ_VIEWER
	component->SetUseVertexColor(false);
#endif
	RootComponent = component;
	component->SetRelativeTransform(FTransform());
	AssetName = assetName;
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

void AStaticMesh::SetbUseTexture(bool value)
{
	if (value != bUseTexture) {
		if (value) {
			Cast<UPrimitiveComponent>(RootComponent)->SetMaterial("TextureMaterial");
		}
		else {
			Cast<UPrimitiveComponent>(RootComponent)->SetMaterial("DefaultMaterial");
		}
	}
	bUseTexture = value;
}
