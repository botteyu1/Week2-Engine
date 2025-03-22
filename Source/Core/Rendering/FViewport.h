#pragma once
#include "Splitter.h"

#define _TCHAR_DEFINED  // TCHAR 재정의 에러 때문
#include <d3d11.h>

class FViewport : public SWindow
{


public:
	FViewport()
	{
	}

	FViewport(FRect InRect)
	{
		Rect = InRect;
		ViewportInfo.TopLeftX = InRect.Left;
		ViewportInfo.TopLeftY = InRect.Top;
		ViewportInfo.Width = InRect.Width();
		ViewportInfo.Height = InRect.Height();
		ViewportInfo.MinDepth = 0.0f;
		ViewportInfo.MaxDepth = 1.0f;
	}


	void OnResizeUpdate();

	float GetViewportRatio()
	{
		return static_cast<float>(ViewportInfo.Width) / static_cast<float>(ViewportInfo.Height);
	}

	void UpdateViewport(FRect InRect)
	{
		ViewportInfo.TopLeftX = InRect.Left;
		ViewportInfo.TopLeftY = InRect.Top;
		ViewportInfo.Width = InRect.Width();
		ViewportInfo.Height = InRect.Height();
		ViewportInfo.MinDepth = 0.0f;
		ViewportInfo.MaxDepth = 1.0f;
	}
	

	const D3D11_VIEWPORT& GetViewportInfo() const { return ViewportInfo; }
	void SetViewportInfo(const D3D11_VIEWPORT& InViewportInfo) { ViewportInfo = InViewportInfo; }

private:



	D3D11_VIEWPORT ViewportInfo = {};
};

