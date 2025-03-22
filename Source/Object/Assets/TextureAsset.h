#pragma once
#include "Asset.h"

class UTexture;

class UTextureAsset: public UAsset {
	DECLARE_CLASS(UTextureAsset, UAsset)

public:
	UTextureAsset() = default;
	// UAsset을(를) 통해 상속됨
	bool RegisterAsset() override;
	bool Load() override;
	bool Save(FString path = "") override;
	bool Unload() override;
	inline std::shared_ptr<UTexture> GetResource() { return Resource; }
private:
	std::shared_ptr<UTexture> Resource;
};