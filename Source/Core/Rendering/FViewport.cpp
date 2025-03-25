#include "FViewport.h"
#include "Core/Rendering/FDevice.h"

FViewport::FViewport(FRect InRect) {
	ViewportInfo.TopLeftX = InRect.Left;
	ViewportInfo.TopLeftY = InRect.Top;
	ViewportInfo.Width = InRect.Width();
	ViewportInfo.Height = InRect.Height();
	ViewportInfo.MinDepth = 0.0f;
	ViewportInfo.MaxDepth = 1.0f;
}

void FViewport::UpdateViewport(FRect InRect) {
	ViewportInfo.TopLeftX = InRect.Left;
	ViewportInfo.TopLeftY = InRect.Top;
	ViewportInfo.Width = InRect.Width();
	ViewportInfo.Height = InRect.Height();
	ViewportInfo.MinDepth = 0.0f;
	ViewportInfo.MaxDepth = 1.0f;
}

void FViewport::Setting() {
	FDevice::Get().GetDeviceContext()->RSSetViewports(1, &ViewportInfo);
}
