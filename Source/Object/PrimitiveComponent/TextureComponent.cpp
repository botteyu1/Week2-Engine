#include "TextureComponent.h"
#include "Object/Assets/AssetManager.h"
#include "Resource/Texture.h"

UTextureComponent::UTextureComponent() {
	Super::Super();
}

UTextureComponent::~UTextureComponent() {

}

void UTextureComponent::AddTexture(FString textureName) {
	AddTexture(UAssetManager::Get().FindAsset<UTextureAsset>(textureName));
}

void UTextureComponent::BeginPlay() {
	Super::BeginPlay();
}

void UTextureComponent::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);
	UpdateBounds();
}

void UTextureComponent::Render() {
	for ( uint32 i = 0; i < textures.Len(); ++i )
		textures[i]->PSSetting(i);
	Super::Render();
}

void UTextureComponent::CalculateModelMatrix(FMatrix& OutMatrix) {
	Super::CalculateModelMatrix(OutMatrix);
}

UDiceComp::UDiceComp() {
	SetMesh("Dice.obj");
	SetMaterial("TextureMaterial");
	AddTexture("Dice.png");
}

UMugComp::UMugComp() {
	SetMesh("Mug.obj");
	SetMaterial("TextureMaterial");
	AddTexture("Dice.png");
}
