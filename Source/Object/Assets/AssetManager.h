#pragma once
#include <concepts>

#include "Asset.h"
#include "Core/AbstractClass/Singleton.h"
#include "Core/HAL/PlatformType.h"
#include "Core/Container/String.h"
#include "Debug/DebugConsole.h"
#include "Object/ObjectFactory.h"


class UAsset;

class UAssetManager : public TSingleton<UAssetManager>
{
public:
	void RegisterAssetMetaDatas();
	void RegisterAssetMetaData(const FString& name, const FString& path, uint64 size, const FString& extension);

	template<typename T>
		requires std::derived_from<T, UAsset>
	T* FindAsset(FString& name)
	{
		T* asset = Assets.Find(name);
		if (asset == nullptr)
		{
			UE_LOG(TEXT("Can't found Asset: %s"), *name);
			return nullptr;
		}

		return dynamic_cast<T*>(asset);
	}

	void LoadAssets();

	template <typename T>
		requires std::derived_from<T, UAsset>
	T* LoadAsset(FAssetMetaData metaData)
	{
		T* asset = FindAsset<T>(metaData.GetAssetName());
		if (asset != nullptr)
		{
			UE_LOG(TEXT("Asset already loaded: %s"), *metaData.GetAssetName());
			return asset;
		}

		asset = FObjectFactory::ConstructObject<T>();
		asset->SetMetaData(metaData);
		if (asset->Load())
		{
			asset->GetMetaData().SetLoaded(true);
			Assets[metaData.GetAssetName()] = asset;
			return asset;
		}
		else
		{
			delete asset;
			UE_LOG(TEXT("Asset load failed: %s"), *metaData.GetAssetName());
			return nullptr;
		}
	}

	template <typename T>
		requires std::derived_from<T, UAsset>
	void UnloadAsset(const FString& name)
	{
		T* asset = FindAsset<T>(name);
		if (asset != nullptr)
		{
			asset->Unload();
			Assets.Remove(name);
			delete asset;
		}
	}

	template <typename T>
		requires std::derived_from<T, UAsset>
	void SaveAsset(const FString& name, const FString& path)
	{
		T* asset = FindAsset<T>(name);
		if (asset == nullptr)
		{
			UE_LOG(TEXT("Can't found Asset: %s"), *name);
			return;
		}

		if (asset != nullptr)
		{
			if (asset->Save(path) == true)
			{
				UE_LOG(TEXT("Save Success: %s"), *name);
			}
			else
			{
				UE_LOG(TEXT("Save Failed: %s"), *name);
			}
		}
	}
	void ObjParsing(const FString& filePath, TArray<FVertexSimple>& outVertex, TArray<uint32>& outIndex);
	
private:
	TMap<FString, FAssetMetaData> AssetMetaDatas;
	TMap<FString, UAsset*> Assets;
};

