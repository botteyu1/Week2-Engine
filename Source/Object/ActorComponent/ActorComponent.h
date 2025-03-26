#pragma once
#include "Core/EngineTypes.h"
#include "Core/UObject/Object.h"
#include "Core/UObject/ObjectMacros.h"
#include "Core/Math/Vector.h"


class UActorComponent : public UObject
{
	DECLARE_CLASS(UActorComponent, UObject)

public:
	UActorComponent() = default;

	virtual void BeginPlay();
	virtual void Tick(float DeltaTime);
	virtual void EndPlay(EEndPlayReason::Type Reason);

	bool CanEverTick() const { return bCanEverTick; }

	virtual class AActor* GetOwner() const;
	virtual void SetOwner(AActor* InOwner) { Owner = InOwner; }

	FVector GetActorPosition() const;

	virtual void Destroyed();
	bool HasActorBegunPlay() const { return bIsBeginPlay; }

protected:
	bool bCanEverTick = true;
	AActor* Owner = nullptr;
protected:
	bool bIsBeginPlay = false;
};

