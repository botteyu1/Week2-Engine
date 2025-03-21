#pragma once
#include <concepts>
#include <memory>

#include "Core/Engine.h"
#include "Core/EngineStatics.h"
#include "Core/Container/String.h"
#include "Core/HAL/PlatformMemory.h"
#include "Core/Rendering/URenderer.h"
#include "Debug/DebugConsole.h"


class UObject;

class FObjectFactory
{
public:
    /**
     * UObject를 생성합니다.
     * @tparam T UObject를 상속받은 클래스
     * @return 캐스팅된 UObject*
     */

	 // raw 포인터 반환 버전
	template<typename T>
		requires std::derived_from<T, UObject>
	static T* ConstructObject()
	{
		return ConstructObjectShared<T>().get();
	}

	// shared_ptr 반환 버전
	template<typename T>
		requires std::derived_from<T, UObject>
	static std::shared_ptr<T> ConstructObjectShared()
	{
		UE_LOG("DEBUG: Construct %s Object", typeid(T).name());

		constexpr size_t ObjectSize = sizeof(T);
		void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(ObjectSize);

		T* ObjectPtr = new (RawMemory) T();
		std::shared_ptr<T> NewObject(ObjectPtr, [ObjectSize](T* Obj)
			{
				Obj->~T();
				FPlatformMemory::Free<EAT_Object>(Obj, ObjectSize);
			});
		NewObject->UUID = UEngineStatics::GenUUID();
		NewObject->NamePrivate = T::StaticClass()->GetName() + "__" + FString::FromInt(NewObject->UUID);
		NewObject->ClassPrivate = T::StaticClass();

		FString str = T::StaticClass()->GetName();

		UEngine::Get().GObjects.Add(NewObject->GetUUID(), NewObject);

		return NewObject;
	}

	 // raw 포인터 반환 버전
	template<typename T>
		requires std::derived_from<T, UObject>
	static T* ConstructObject(const FString& _FName)
	{
		return ConstructObjectShared<T>(_FName).get();
	}

	// shared_ptr 반환 버전
	template<typename T>
		requires std::derived_from<T, UObject>
	static std::shared_ptr<T> ConstructObjectShared(const FString& _FName)
	{
		UE_LOG("DEBUG: Construct %s Object", typeid(T).name());

		constexpr size_t ObjectSize = sizeof(T);
		void* RawMemory = FPlatformMemory::Malloc<EAT_Object>(ObjectSize);

		T* ObjectPtr = new (RawMemory) T();
		std::shared_ptr<T> NewObject(ObjectPtr, [ObjectSize](T* Obj)
			{
				Obj->~T();
				FPlatformMemory::Free<EAT_Object>(Obj, ObjectSize);
			});
		NewObject->UUID = UEngineStatics::GenUUID();
		NewObject->NamePrivate = _FName + "__" + FString::FromInt(NewObject->UUID);
		NewObject->ClassPrivate = T::StaticClass();

		FString str = T::StaticClass()->GetName();

		UEngine::Get().GObjects.Add(NewObject->GetUUID(), NewObject);

		return NewObject;
	}
};
