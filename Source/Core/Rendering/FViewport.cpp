#include "FViewport.h"

void FViewport::OnResizeUpdate()
{
	ViewportInfo.TopLeftX = Rect.Left;
	ViewportInfo.TopLeftY = Rect.Top;
	ViewportInfo.Width = Rect.Width();
	ViewportInfo.Height = Rect.Height();
	ViewportInfo.MinDepth = 0.0f;
	ViewportInfo.MaxDepth = 1.0f;
		


}
