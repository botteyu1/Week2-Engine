#include "SWindow.h"
#include "Core/Rendering/FViewport.h"
#include "Object/World/World.h"

SWorldWindow::SWorldWindow(FViewportClient* viewportClient): viewportClient(viewportClient) {}

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
