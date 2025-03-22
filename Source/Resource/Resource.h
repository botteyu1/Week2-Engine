#pragma once
#include <mutex>
#include <memory>

#include "Core/Container/String.h"
#include "Core/Container/Map.h"
#include "Core/HAL/PlatformMemory.h"
#include "Core/UObject/Object.h"
#include "Core/UObject/Class.h"
#include "Object/ObjectFactory.h"

template <typename ResourcesType>
class UResource : public UObject
{

public:
	UResource() = default;
	virtual ~UResource() = default;

	// delete Function
	UResource(const UResource& Other) = delete;
	UResource(UResource&& Other) noexcept = delete;
	UResource& operator=(const UResource& Other) = delete;
	UResource& operator=(UResource&& Other) noexcept = delete;


public:
	// 부모 클래스와 현재 클래스 타입 정의
	using Super = UObject;
	using ThisClass = UResource<ResourcesType>;

	// StaticClass 함수 구현
	inline static UClass* StaticClass()
	{
		static std::unique_ptr<UClass, UClassDeleter> StaticClassInfo = nullptr;
		if (!StaticClassInfo)
		{
			constexpr size_t ClassSize = sizeof(UClass);
			void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(ClassSize);

			// 클래스 이름을 문자열로 생성 (템플릿 타입 정보 포함)
			//std::string ClassName = "FResource_";
			//ClassName += typeid(ResourcesType).name();
			
			std::string  ClssName = typeid(ResourcesType).name();

			const std::string classPrefix = "class ";
			if (ClssName.find(classPrefix) == 0)
			{
				ClssName.erase(0, classPrefix.length());
			}

			UClass* ClassPtr = new (RawMemory) UClass{
				ClssName.c_str(),
				static_cast<uint32>(sizeof(UResource<ResourcesType>)),
				static_cast<uint32>(alignof(UResource<ResourcesType>)),
				UObject::StaticClass()
			};

			StaticClassInfo = std::unique_ptr<UClass, UClassDeleter>(ClassPtr, UClassDeleter{});
		}
		return StaticClassInfo.get();
	}


	static std::shared_ptr<ResourcesType> Find(const FString& InName)
	{
		return FindFName(InName);
	}

	static std::shared_ptr<ResourcesType> FindFName(const FName& InName)
	{
		std::lock_guard Lock(NameMutex);
		auto pResult = NameRes.Find(InName);

		return (pResult != nullptr) ? *pResult : nullptr;
	}

	static void Release(FString InName)
	{
		ReleaseFName(InName);
	}

	static void ReleaseFName(FName InName)
	{
		std::lock_guard<std::mutex> Lock(NameMutex);
		NameRes.Remove(InName);
	}


	static void AllResourcesRelease()
	{
		{
			std::lock_guard<std::mutex> Lock(NameMutex);
			NameRes.Empty();
		}
	}

	void SetName(const FString& InName)
	{
		Name = InName;
	}

	FString GetName()
	{
		return Name;
	}

protected:
	static std::shared_ptr<ResourcesType> CreateRes(const FString& InName)
	{
		std::shared_ptr<ResourcesType> NewRes = FObjectFactory::ConstructObjectShared<ResourcesType>(InName);

		std::lock_guard Lock(NameMutex);
		NewRes->SetName(InName);
		NameRes.Add(InName, NewRes);
		return NewRes;
	}

private:
	static std::mutex NameMutex;
	static TMap<FName, std::shared_ptr<ResourcesType>> NameRes;

	FString Name;
};


template <typename ResourcesType>
TMap<FName, std::shared_ptr<ResourcesType>> UResource<ResourcesType>::NameRes;

template <typename ResourcesType>
std::mutex UResource<ResourcesType>::NameMutex;
