#pragma once
#include "Core/Math/Rect.h"

#define _TCHAR_DEFINED  // MYTCHAR 재정의 에러 때문
#include <d3d11.h>

class FViewport
{
public:
	FViewport() {}
	FViewport(FRect InRect);
	void UpdateViewport(FRect InRect);
	void Setting();

	inline const D3D11_VIEWPORT& GetViewportInfo() const { return ViewportInfo; }
	inline const float GetViewportRatio() const {
		return static_cast<float>(ViewportInfo.Width) / static_cast<float>(ViewportInfo.Height);
	}

private:
	D3D11_VIEWPORT ViewportInfo = {};
};

