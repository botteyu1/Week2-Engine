#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/UObject/Object.h"
#include "Core/UObject/ObjectMacros.h"

enum class EAssetType : uint8
{
	Texture,
	Material,
	Mesh,
	Animation,
	Sound,
	Font,
	Shader,
	Blueprint,
	Scene,
	World,
	Asset,
	EditorConfig,
	Text,
	None
};

struct FAssetMetaData
{
public:
	FAssetMetaData() : AssetName(""), AssetPath(""), AssetSize(0), Type(EAssetType::None), BIsLoaded(false), BIsDirty(false) {}

	FAssetMetaData(FString InAssetName, FString InAssetPath, uint64 assetSize, const FString& extension)
		: AssetName(std::move(InAssetName))
		, AssetPath(std::move(InAssetPath))
		, AssetExtension(std::move(extension))
		, AssetSize(assetSize), BIsLoaded(false), BIsDirty(false)
	{
		if (extension.Equals(TEXT(".png")) || extension.Equals(TEXT(".jpb")) || extension.Equals(TEXT(".bmp")) || extension.Equals(TEXT(".dds")))
		{
			Type = EAssetType::Texture;
		}
		else if (extension.Equals(TEXT(".fbx")) || extension.Equals(TEXT(".obj")))
		{
			Type = EAssetType::Mesh;
		}
		else if (extension.Equals(TEXT(".wav")) || extension.Equals(TEXT(".mp3")))
		{
			Type = EAssetType::Sound;
		}
		else if (extension.Equals(TEXT(".ttf")) || extension.Equals(TEXT(".otf")))
		{
			Type = EAssetType::Font;
		}
		else if (extension.Equals(TEXT(".material")))
		{
			Type = EAssetType::Material;
		}
		else if (extension.Equals(TEXT(".anim")))
		{
			Type = EAssetType::Animation;
		}
		else if (extension.Equals(TEXT(".hlsl")))
		{
			Type = EAssetType::Shader;
		}
		else if (extension.Equals(TEXT(".blueprint")))
		{
			Type = EAssetType::Blueprint;
		}
		else if (extension.Equals(TEXT(".Scene")))
		{
			Type = EAssetType::Scene;
		}
		else if (extension.Equals(TEXT(".world")))
		{
			Type = EAssetType::World;
		}
		else if (extension.Equals(TEXT(".ini")))
		{
			Type = EAssetType::EditorConfig;
		}
		else if (extension.Equals(TEXT(".txt"))) 
		{
			Type = EAssetType::Text;
		}
		else
		{
			Type = EAssetType::None;
		}
	}

public:
	FString GetAssetName() const { return AssetName; }
	FString GetAssetPath() const { return AssetPath; }
	FString GetAssetExtension() const { return AssetExtension; }
	uint64 GetAssetSize() const { return AssetSize; }
	EAssetType GetAssetType() const { return Type; }
	bool IsLoaded() const { return BIsLoaded; }
	bool IsDirty() const { return BIsDirty; }

	void SetAssetName(const FString& InAssetName) { AssetName = InAssetName; }
	void SetAssetPath(const FString& InAssetPath) { AssetPath = InAssetPath; }
	void SetAssetSize(uint64 InAssetSize) { AssetSize = InAssetSize; }
	void SetAssetType(EAssetType InAssetType) { Type = InAssetType; }
	void SetIsLoaded(bool bIsLoaded) { BIsLoaded = bIsLoaded; }
	void SetIsDirty(bool bIsDirty) { BIsDirty = bIsDirty; }

private:
	FString AssetName;
	FString AssetPath;
	FString AssetExtension;
	uint64 AssetSize;
	EAssetType Type;

	bool BIsLoaded;
	bool BIsDirty;
};

class UAsset : public UObject
{
	DECLARE_CLASS(UAsset, UObject)
	
public:
	UAsset() = default;

	virtual bool RegisterAsset() = 0;
	virtual bool Load() = 0;
	virtual bool Save(FString path = "") = 0;
	virtual bool Unload() = 0;

	inline void SetMetaData(const FAssetMetaData& InMetaData) { MetaData = InMetaData; }
	inline void SetAssetName(const FString& InAssetName) { MetaData.SetAssetName(InAssetName); }
	inline void SetAssetPath(const FString& InAssetPath) { MetaData.SetAssetPath(InAssetPath); }
	inline void SetIsLoaded(bool bIsLoaded) { MetaData.SetIsLoaded(bIsLoaded); }
	inline void SetIsDirty(bool bIsDirty) { MetaData.SetIsDirty(bIsDirty); }

	inline bool IsLoaded() const { return MetaData.IsLoaded(); }
	inline bool IsDirty() const { return MetaData.IsDirty(); }

	inline FString GetAssetName() const { return MetaData.GetAssetName(); }
	inline FString GetAssetPath() const { return MetaData.GetAssetPath(); }
	inline uint64 GetAssetSize() const { return MetaData.GetAssetSize(); }
	inline EAssetType GetAssetType() const { return MetaData.GetAssetType(); }

protected:
	FAssetMetaData MetaData;
};

