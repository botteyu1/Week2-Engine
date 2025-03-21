#pragma once
#include "Actor.h"


class ADice : public AActor
{
	DECLARE_CLASS(ADice, AActor)

public:
	ADice();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual const char* GetTypeName() override;
};

