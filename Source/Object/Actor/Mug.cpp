#include "Mug.h"
#include "Object/PrimitiveComponent/TextureComponent.h"

AMug::AMug()
{
	bCanEverTick = true;

	UMugComp* MugComponent = AddComponent<UMugComp>();
	RootComponent = MugComponent;
	MugComponent->SetRelativeTransform(FTransform());
}

void AMug::BeginPlay()
{
	Super::BeginPlay();
}

void AMug::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

const char* AMug::GetTypeName()
{
	return "StaticMesh";
}
