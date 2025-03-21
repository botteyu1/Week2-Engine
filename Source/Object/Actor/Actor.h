#pragma once
#include "Core/EngineTypes.h"
#include "Core/Container/Set.h"
#include "Core/Math/Transform.h"
#include "Core/UObject/Object.h"
#include "Core/UObject/ObjectMacros.h"
#include "Object/ObjectFactory.h"
#include "Object/USceneComponent.h"


class UWorld;

class AActor : public UObject
{
	DECLARE_CLASS(AActor, UObject)

	friend class UEditorManager;
public:
	AActor();
	virtual ~AActor() override = default;

	void SetDepth(int InDepth)
	{
		Depth = InDepth;
	}

	int GetDepth() const
	{
		return Depth;
	}
public:
	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void LateTick (float DeltaTime); // 렌더 후 호출
	
	virtual void Destroyed();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	TSet<UActorComponent*>& GetComponents() { return Components; }

	UWorld* GetWorld() const { return World; }
	void SetWorld(UWorld* InWorld) { World = InWorld; }

private:
	virtual void Pick();
	virtual void UnPick();

	uint32 Depth;
	bool bIsPicked = false;
	ETickState TickState; //현재 Tick 여부 상태
	bool bHidden = false ;

public:
	bool IsPicked() const { return bIsPicked; }
	bool IsActorTickEnabled() const
	{
		return TickState != ETickState::Disabled;
	}

	bool IsHidden() const
	{
		return bHidden;
	}
	void SetActorHiddenInGame(bool bNewHidden)
	{
		if (IsHidden() != bNewHidden)
		{
			bHidden = bNewHidden;
		}
	}

	void SetTickFunctionEnable(bool bInEnabled);

public:
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	T* AddComponent()
	{
		T* ObjectInstance = FObjectFactory::ConstructObject<T>();
		Components.Add(ObjectInstance);
		ObjectInstance->SetOwner(this);

		USceneComponent* NewSceneComp = dynamic_cast<USceneComponent*>(ObjectInstance);
		if (NewSceneComp != nullptr)
		{
			if (RootComponent == nullptr)
			{
				RootComponent = NewSceneComp;
			}
			else
			{
				NewSceneComp->SetupAttachment(RootComponent);
			}

		}

		return ObjectInstance;
	}

	// delete
	template<typename T>
		requires std::derived_from<T, UActorComponent>
	void RemoveComponent(T* Object)
	{
		Components.Remove(Object);
	}

	template<typename T>
		requires std::derived_from<T, UActorComponent>
	T* GetComponentByClass()
	{
		for (UActorComponent* Component : Components)
		{
			if (T* CastedComponent = dynamic_cast<T*>(Component))
			{
				return CastedComponent;
			}
		}
		return nullptr;
	}

public:
	const FTransform& GetActorTransform() const;
	const FTransform& ActorToWorld() const;

	FVector GetActorForwardVector() const;
	FVector GetActorRightVector() const;
	FVector GetActorUpVector() const;

	FVector GetActorPosition() const;
	FVector GetActorRotation() const;
	FQuat GetActorRotationQuat() const;
	FVector GetActorScale() const;

public:
	bool SetActorPosition(const FVector& InePosition);
	bool SetActorRotation(const FVector& InRotation);
	bool SetActorRotation(const FQuat& InQuat);

	bool SetActorPositionAndRotation(const FVector& InPosition, const FVector& InRotation);
	bool SetActorPositionAndRotation(const FVector& Ineosition, const FQuat& InQuat);

	bool SetActorScale(const FVector& InScale);

	void SetActorTransform(const FTransform& InTransform);

public:
	void SetActorRelativePosition(const FVector& InPosition);

	void SetActorRelativeRotation(const FVector& InRotation);
	void SetActorRelativeRotation(const FQuat& InRotation);
	void SetActorRelativeTransform(const FTransform& InTransform);

	void SetActorRelativeScale(const FVector& InScale);
	FVector GetActorRelativeScale() const;

public:
	void AddWorldOffset(const FVector& Delta);
	void AddActorWorldRotation(const FVector& Delta);
	void AddActorWorldRotation(const FQuat& Delta);

	void AddActorWorldTransform(const FTransform& Delta);
	void AddActorWorldTransformKeepScale(const FTransform& Delta);

	void AddActorLocalOffset(const FVector& Delta);

	void AddActorLocalRotation(const FVector& Delta);
	void AddActorLocalRotation(const FQuat& Delta);

	void AddActorLocalTransform(const FTransform& Delta);
public:
	/**
	 * 이 액터를 구성하는 모든 컴포넌트(ChildActorComponents 제외)의 경계 상자를 반환합니다.
	 * @param bOnlyCollidingComponents		true인 경우, 충돌이 활성화된 컴포넌트들의 경계 상자만 반환합니다.
	 * @param Origin						월드 공간에서 액터의 중심으로 설정됩니다.
	 * @param BoxExtent						3차원 공간에서 액터 크기의 절반으로 설정됩니다.
	 * @param bIncludeFromChildActors		true인 경우, ChildActor 컴포넌트까지 재귀적으로 포함합니다.
	 */
	virtual void GetActorBounds(bool bOnlyCollidingComponents, FVector& Origin, FVector& BoxExtent, bool bIncludeFromChildActors = false) const;
	/**
	* 이 액터 내의 모든 컴포넌트의 월드 공간 경계 상자를 반환합니다.
	* @param bNonColliding 경계 상자에 충돌을 사용하지 않는 컴포넌트를 포함할 것인지 여부를 나타냅니다.
	* @param bIncludeFromChildActors true이면 ChildActor 컴포넌트 내부로 재귀적으로 들어가 해당 액터들의 적절한 타입의 컴포넌트도 포함합니다.
	*/
	virtual FBox GetComponentsBoundingBox(bool bNonColliding = false, bool bIncludeFromChildActors = false) const;

	/**
	* 이 액터에 포함된 모든 컴포넌트들의 액터 공간 바운딩 박스를 계산합니다.
	* 이 함수는 GetComponentsBoundingBox()보다 느립니다. 왜냐하면 컴포넌트들의 로컬 바운드가 캐시되지 않고, 이 함수가 호출될 때마다 다시 계산되기 때문입니다.
	* @param bNonColliding 바운딩 박스에 충돌하지 않는 컴포넌트들도 포함할지 여부를 나타냅니다.
	* @param bIncludeFromChildActors true일 경우, ChildActor 컴포넌트 내부로 재귀적으로 들어가서 해당 액터들 내의 적절한 타입의 컴포넌트들도 찾아 포함합니다.
	*/
	virtual FBox CalculateComponentsBoundingBoxInLocalSpace(bool bNonColliding = false, bool bIncludeFromChildActors = false) const;

	// 임시
	FVector GetActorWorldBoundsMin() const;
	FVector GetActorWorldBoundsMax() const;

	FVector GetActorLocalBoundsMin() const;
	FVector GetActorLocalBoundsMax() const;

public:
	bool CanEverTick() const { return bCanEverTick; }
	virtual const char* GetTypeName();

	bool Destroy();

public:
	USceneComponent* GetRootComponent() const { return RootComponent; }
	void SetRootComponent(USceneComponent* InRootComponent);

public:
	void SetColor(FVector4 InColor);
	void SetUseVertexColor(bool bUseVertexColor);

protected:
	bool bCanEverTick = true; //원래 Tick을 하는 액터인지
	USceneComponent* RootComponent = nullptr;

private:
	UWorld* World = nullptr;
	TSet<UActorComponent*> Components;

public:
	AActor* Owner = nullptr;
	TArray<AActor> Children;
//Editor Only
	AActor* GroupActor = nullptr;
};

