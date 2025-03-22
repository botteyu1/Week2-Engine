#include "Dice.h"
#include "Object/PrimitiveComponent/UPrimitiveComponent.h"

ADice::ADice()
{
	bCanEverTick = true;

	UDiceComp* DiceComponent = AddComponent<UDiceComp>();
	RootComponent = DiceComponent;
	DiceComponent->SetRelativeTransform(FTransform());
}

void ADice::BeginPlay()
{
	Super::BeginPlay();
}

void ADice::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* ADice::GetTypeName()
{
	return nullptr;
}
