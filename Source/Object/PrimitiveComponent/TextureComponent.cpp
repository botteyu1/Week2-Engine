#include "TextureComponent.h"
#include "Object/Assets/AssetManager.h"
#include "Resource/Texture.h"

UTextureComponent::UTextureComponent() {
	Super::Super();
}

UTextureComponent::~UTextureComponent() {

}

void UTextureComponent::AddTexture(const FString& textureName) {
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
	for ( uint32 i = 0; i < textures.Num(); ++i )
		textures[i]->PSSetting(i);	
	
	// 현재는 Texture t0만 쓰는 것으로
	// 텍스쳐 여러개를 한번에 draw 하기 위해 Texture2DArray 방식을 사용
	// 
	// 만약 t 레지스터를 여러개 사용하고자 한다면 별개의 Shader 코드를 사용해야할 것으로 보임
	// 그 때는 아래의 링크를 참고해보길 바랍니다. 
	// https://silky-grin-a74.notion.site/Texture2D-1c093d275711802d83ade0869e04a7a8?pvs=4
	
	Super::Render();
}

void UTextureComponent::CalculateModelMatrix(FMatrix& OutMatrix) {
	Super::CalculateModelMatrix(OutMatrix);
}
