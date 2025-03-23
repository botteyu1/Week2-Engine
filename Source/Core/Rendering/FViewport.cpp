#include "FViewport.h"
#include "Core/Rendering/FDevice.h"

inline FViewport::FViewport(FRect InRect) {
	Rect = InRect;
	ViewportInfo.TopLeftX = InRect.Left;
	ViewportInfo.TopLeftY = InRect.Top;
	ViewportInfo.Width = InRect.Width();
	ViewportInfo.Height = InRect.Height();
	ViewportInfo.MinDepth = 0.0f;
	ViewportInfo.MaxDepth = 1.0f;
}

void FViewport::OnResizeUpdate()
{
	ViewportInfo.TopLeftX = Rect.Left;
	ViewportInfo.TopLeftY = Rect.Top;
	ViewportInfo.Width = Rect.Width();
	ViewportInfo.Height = Rect.Height();
	ViewportInfo.MinDepth = 0.0f;
	ViewportInfo.MaxDepth = 1.0f;
}

void FViewport::UpdateViewport(FRect InRect) {
	Rect = InRect;
	ViewportInfo.TopLeftX = InRect.Left;
	ViewportInfo.TopLeftY = InRect.Top;
	ViewportInfo.Width = InRect.Width();
	ViewportInfo.Height = InRect.Height();
	ViewportInfo.MinDepth = 0.0f;
	ViewportInfo.MaxDepth = 1.0f;
}

void FViewport::SetViewportInfo(const D3D11_VIEWPORT& InViewportInfo) { 
	ViewportInfo = InViewportInfo; 
	Rect.Left = ViewportInfo.TopLeftX;
	Rect.Top = ViewportInfo.TopLeftY;
	Rect.Right = ViewportInfo.TopLeftX + ViewportInfo.Width;
	Rect.Bottom = ViewportInfo.TopLeftY + ViewportInfo.Height;
}

void FViewport::Setting() {
	FDevice::Get().GetDeviceContext()->RSSetViewports(1, &ViewportInfo);
}
