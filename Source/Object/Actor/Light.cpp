#include "Light.h"

ALight::ALight()
{
	LightColor = FVector4(1.0f, 0.75f, 0.5f, 1.0f);
	LightDirection = FVector(10, -10, -10).GetSafeNormal();
}
