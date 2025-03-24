#include "SWindow.h"
#include "Core/Rendering/FViewport.h"
#include "Object/World/World.h"


void SWindow::OnResizeUpdate() {
	child->OnResizeUpdate();
}


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
	ACamera* cam = viewportClient->camera;
	UWorld* world = UEngine::Get().GetWorld();
	world->SetFocusCamera(cam);
	world->RayCasting(UInputManager::GetNDCPosInWindow(InCoord, Rect.Width(), Rect.Height()));
}

void SSplitterH::OnResizeUpdate() {
	FRect parentRect = parent->Rect;
	SideLT->Rect = FRect(
		parentRect.Left,
		parentRect.Top,
		parentRect.Left + (parentRect.Right - parentRect.Left) * SplitPos,
		parentRect.Bottom
	);
	SideLT->OnResizeUpdate();
	SideRB->Rect = FRect(
		parentRect.Left + (parentRect.Right - parentRect.Left) * SplitPos,
		parentRect.Top,
		parentRect.Right,
		parentRect.Bottom
	);
	SideRB->OnResizeUpdate();
}

void SSplitterV::OnResizeUpdate() {
	FRect parentRect = parent->Rect;
	SideLT->Rect = FRect(
		parentRect.Left,
		parentRect.Top,
		parentRect.Right,
		parentRect.Top + (parentRect.Bottom - parentRect.Top) * SplitPos
	);
	SideLT->OnResizeUpdate();
	SideRB->Rect = FRect(
		parentRect.Left,
		parentRect.Top + (parentRect.Bottom - parentRect.Top) * SplitPos,
		parentRect.Right,
		parentRect.Bottom
	);
	SideRB->OnResizeUpdate();
}
