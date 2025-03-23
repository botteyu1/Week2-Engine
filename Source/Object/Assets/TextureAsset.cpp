#include "TextureAsset.h"

#include <iostream>
#include <filesystem>
#include <fstream>

#include "Resource/DirectResource/Sampler.h"
#include "Resource/Texture.h"


using namespace std;

bool UTextureAsset::RegisterAsset() {
	return false;
}

bool UTextureAsset::Load() {
	if ( IsLoaded() ) {
		cout << "Asset already loaded: " << MetaData.GetAssetPath().GetData() << '\n';
		return true;
	}

	filesystem::path filePath = MetaData.GetAssetPath().GetData();
	if ( filesystem::exists(filePath) == false ) {
		cout << "File not found: " << filePath << '\n';
		return false;
	}

	ifstream file(filePath);
	if ( file.is_open() == false ) {
		cout << "File open failed: " << filePath << '\n';
		return false;
	}

	Texture = UTexture::Load(MetaData, MetaData.GetAssetName());
	Sampler = USampler::Find("LinearSamplerState");
	Texture->CreateShaderResourceView();

	return true;
}

bool UTextureAsset::Save(FString path) {
	return false;
}

bool UTextureAsset::Unload() {
	return false;
}

void UTextureAsset::PSSetting(UINT InSlot) {
	Texture->PSSetting(InSlot);
	Sampler->PSSetting(InSlot);
}

