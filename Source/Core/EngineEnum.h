//자주 사용되는 Enum들을 정의해놓은 파일입니다.

#pragma once
#include "Core/HAL/PlatformType.h"



enum class ERenderQueue
{
	Background = 0,
	Default = 100,
	AlphaTest = 200,
	Transparent = 300,
	EditorPrimitives = 400,
	Overlay = 500,
};