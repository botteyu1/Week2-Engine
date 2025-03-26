#pragma once

#pragma once
#include "Core/HAL/PlatformType.h"
#include "Core/Interfaces/GizmoInterface.h"
#include "Object/Actor/Actor.h"
#include "Core/Math/Rect.h"
#include "Core/Rendering/FViewport.h"

class ALight : public AActor, public IGizmoInterface
{
	DECLARE_CLASS(ALight, AActor)

public:
	ALight();

	virtual bool IsGizmo() override { return true; }

	FVector4 GetLightColor() { return LightColor; }
	FVector GetLightDirection() { return LightDirection; }

private:
	FVector4 LightColor;
	FVector LightDirection;
};
