#pragma once
#include "Actor.h"


class AMug : public AActor
{
	DECLARE_CLASS(AMug, AActor)

public:
	AMug();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

