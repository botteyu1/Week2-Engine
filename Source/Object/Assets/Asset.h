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
	FontAtlas,
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

	FAssetMetaData(FString InAssetName, FString InAssetPath, uint64 assetSize, FString InExtension)
		: AssetName(std::move(InAssetName))
		, AssetPath(std::move(InAssetPath))
		, AssetExtension(std::move(InExtension))
		, AssetSize(assetSize), BIsLoaded(false), BIsDirty(false)
	{
		if ( AssetExtension.Equals(TEXT(".png")) || AssetExtension.Equals(TEXT(".jpb")) || InExtension.Equals(TEXT(".bmp")) || InExtension.Equals(TEXT(".dds")))
		{
			Type = EAssetType::Texture;
		}
		else if (AssetExtension.Equals(TEXT(".fbx")) || AssetExtension.Equals(TEXT(".obj")))
		{
			Type = EAssetType::Mesh;
		}
		else if (AssetExtension.Equals(TEXT(".wav")) || AssetExtension.Equals(TEXT(".mp3")))
		{
			Type = EAssetType::Sound;
		}
		else if (AssetExtension.Equals(TEXT(".ttf")) || AssetExtension.Equals(TEXT(".otf")))
		{
			Type = EAssetType::Font;
		}
		else if (AssetExtension.Equals(TEXT(".fontatlas")))
		{
			Type = EAssetType::FontAtlas;
		}
		else if (AssetExtension.Equals(TEXT(".material")))
		{
			Type = EAssetType::Material;
		}
		else if (AssetExtension.Equals(TEXT(".anim")))
		{
			Type = EAssetType::Animation;
		}
		else if (AssetExtension.Equals(TEXT(".hlsl")))
		{
			Type = EAssetType::Shader;
		}
		else if (AssetExtension.Equals(TEXT(".blueprint")))
		{
			Type = EAssetType::Blueprint;
		}
		else if (AssetExtension.Equals(TEXT(".Scene")))
		{
			Type = EAssetType::Scene;
		}
		else if (AssetExtension.Equals(TEXT(".world")))
		{
			Type = EAssetType::World;
		}
		else if (AssetExtension.Equals(TEXT(".ini")))
		{
			Type = EAssetType::EditorConfig;
		}
		else if (AssetExtension.Equals(TEXT(".txt")))
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

