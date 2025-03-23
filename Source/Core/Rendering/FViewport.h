#pragma once
#include "Splitter.h"

#define _TCHAR_DEFINED  // MYTCHAR 재정의 에러 때문
#include <d3d11.h>

class FViewport : public SWindow
{


public:
	FViewport() {}

	FViewport(FRect InRect);


	void OnResizeUpdate();

	inline const float GetViewportRatio() const
	{
		return static_cast<float>(ViewportInfo.Width) / static_cast<float>(ViewportInfo.Height);
	}

	void UpdateViewport(FRect InRect);
	

	inline const D3D11_VIEWPORT& GetViewportInfo() const { return ViewportInfo; }
	void SetViewportInfo(const D3D11_VIEWPORT& InViewportInfo) { ViewportInfo = InViewportInfo; }
	void Setting();
private:



	D3D11_VIEWPORT ViewportInfo = {};
};

