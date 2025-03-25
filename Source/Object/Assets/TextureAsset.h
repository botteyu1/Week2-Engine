#pragma once
#include "Asset.h"
#include "Core/Container/Array.h"

class UTexture;
class USampler;

class UTextureAsset: public UAsset {
	DECLARE_CLASS(UTextureAsset, UAsset)

public:
	UTextureAsset() = default;
	// UAsset을(를) 통해 상속됨
	bool RegisterAsset() override;
	bool Load() override;
	bool LoadForTextureArray(TArray<FString> TextureNames);
	bool Save(FString path = "") override;
	bool Unload() override;
	inline std::shared_ptr<UTexture> GetResource() { return Texture; }
	void PSSetting(UINT);
private:
	std::shared_ptr<UTexture> Texture;
	std::shared_ptr<USampler> Sampler;
};