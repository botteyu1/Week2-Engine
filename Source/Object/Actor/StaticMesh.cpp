#include "StaticMesh.h"
#include "Object/PrimitiveComponent/TextureComponent.h"
#include "Core/UObject/Casts.h"
#include "Object/Assets/AssetManager.h"
#include "Object/Assets/MeshAsset.h"

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
#else
	component->Max = component->GetMesh()->GetVertexBuffer()->GetMax();
	component->Min = component->GetMesh()->GetVertexBuffer()->GetMin();
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

void AStaticMesh::AddMesh(FString MeshType, bool texture)
{
	UTextureComponent* component = AddComponent<UTextureComponent>();
	//objName = MeshType;
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
	component->SetRelativeTransform(FTransform());
}

void AStaticMesh::ChangeMaterial(FString subMeshName, FString destMaterialName)
{
	if (RootComponent->IsA(UTextureComponent::StaticClass())) 
	{
		UMeshAsset* meshAsset = UAssetManager::Get().FindAsset<UMeshAsset>(objName);
		std::string typeName(GetTypeName());
		FString NewAssetName = FString(typeName) + "-" + FString::FromInt(GetUUID());
		meshAsset->ChangeMaterial(NewAssetName, subMeshName, destMaterialName);

		UTextureComponent* textureComponent = Cast<UTextureComponent>(RootComponent);
		textureComponent->SetMesh(NewAssetName);
		textureComponent->RemoveTexture(0);		// 일단은 0번에 다 넣어주니까 0번 삭제
		textureComponent->AddTexture(NewAssetName + ".textArray");
	}
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

TArray<FSubMesh> AStaticMesh::GetSubMeshes()
{
	UTextureComponent* textureComponent = Cast<UTextureComponent>(RootComponent);
	return textureComponent->GetMesh()->GetSubMeshes();
}
