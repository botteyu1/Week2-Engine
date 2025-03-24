#include "SWindow.h"
#include "Core/Rendering/FViewport.h"
#include "Object/World/World.h"

SWorldWindow::SWorldWindow(FViewportClient* viewportClient): viewportClient(viewportClient) {
	D3D11_VIEWPORT viewport = viewportClient->viewport.GetViewportInfo();
	Rect = FRect(
		viewport.TopLeftX,
		viewport.TopLeftY,
		viewport.TopLeftX + viewport.Width,
		viewport.TopLeftY + viewport.Height
	);
}

SWorldWindow::~SWorldWindow() {
	UWorld* world = UEngine::Get().GetWorld();
	world->RemoveViewportClient(viewportClient);
}

void SWorldWindow::OnResizeUpdate() {
	viewportClient->viewport.UpdateViewport(Rect);
}

void SWorldWindow::OnMousePressed(FVector2D InCoord) {
	//for ( auto& pair : this->GetCameraMap() ) {
	//	ACamera* cam = pair.Value;
	//	viewports[pair.Key] = cam->GetViewPort();
	//}
	ACamera* cam = viewportClient->camera;
	UWorld* world = UEngine::Get().GetWorld();
	world->SetFocusCamera(cam);
	world->RayCasting(UInputManager::GetNDCPosInWindow(InCoord, Rect.Width(), Rect.Height()));
}
