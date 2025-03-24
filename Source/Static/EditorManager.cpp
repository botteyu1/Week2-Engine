#include "EditorManager.h"
#include "Core/Engine.h"
#include "Object/World/World.h"
#include "Core/Math/Vector.h"
#include "Core/Math/Transform.h"
#include "Core/Rendering/SWindow.h"
#include <Object/Gizmo/GizmoActor.h>
#include "Debug/DebugDrawManager.h"

#include "Core/Input/PlayerInput.h"
#include "Resource/Texture.h"
#include "Core/Rendering/FDevice.h"

#include <functional>

void UEditorManager::Release() {
	UUIDTexture.reset();
	RootWindow.reset();
	SelectedWindow.reset();
}

void UEditorManager::Init()
{
	CreateUUIDTexture();
	RegisterInputCallbacks();
	InitMainSWindow();
	//D3D11_TEXTURE2D_DESC DepthBufferDesc = {};
	//DepthBufferDesc.Width = Width;
	//DepthBufferDesc.SplitterHeight = SplitterHeight;
	//DepthBufferDesc.MipLevels = 1;
	//DepthBufferDesc.ArraySize = 1;
	//DepthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;            // 32비트 중 24비트는 깊이, 8비트는 스텐실
	//DepthBufferDesc.SampleDesc.Count = 1;
	//DepthBufferDesc.SampleDesc.Quality = 0;
	//DepthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//DepthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;              // 텍스쳐 바인딩 플래그를 DepthStencil로 설정
	//DepthBufferDesc.CPUAccessFlags = 0;
	//DepthBufferDesc.MiscFlags = 0;
	//
	//UUIDTextureDepthStecil = FTexture::Create("UUIDTextureDepthStecil", DepthBufferDesc);
	//UUIDTextureDepthStecil->CreateDepthStencilView();
}

void UEditorManager::RegisterInputCallbacks() {
	UEngine::Get().GetInput()->RegisterMouseDownCallback(EKeyCode::LButton, [this](const FVector& MouseNDCPos) {
		UInputManager* inputManager = UEngine::Get().GetInput();
		FVector mousePos = inputManager->GetMousePos();
		FVector2D mousePosInWindow;

		this->SelectedWindow = this->GetClickedWindow(mousePos, this->GetRootWindow());
		if ( this->SelectedWindow == nullptr )
			return;
		UE_LOG("%x", this->SelectedWindow.get());
		mousePosInWindow.X = mousePos.X - this->SelectedWindow->Rect.Left;
		mousePosInWindow.Y = mousePos.Y - this->SelectedWindow->Rect.Top;
		this->SelectedWindow->OnMousePressed(mousePosInWindow);
	}, UEngine::Get().GetWorld()->GetUUID());
}

void UEditorManager::CreateUUIDTexture() {
	const int Width = static_cast<int>(FDevice::Get().GetFrameBufferWindowSize().X);
	const int Height = static_cast<int>(FDevice::Get().GetFrameBufferWindowSize().Y);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = Width;
	textureDesc.Height = Height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT; // 32비트 UINT로 변경
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	UUIDTexture = UTexture::Create("UUIDTexture", textureDesc);
	//UUIDTexture->CreateRenderTargetView(); // 이미 create 내에서 실행됨
}

void UEditorManager::InitMainSWindow() {
	UWorld* world = UEngine::Get().GetWorld();
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	FDevice::Get().GetSwapChain()->GetDesc(&SwapChainDesc);
	FViewportClient* viewportClient = world->AddViewportClient(
		FRect(
			0,
			0,
			SwapChainDesc.BufferDesc.Width,
			SwapChainDesc.BufferDesc.Height
		)
	);
	RootWindow = std::make_shared<SWorldWindow>(viewportClient);
	world->SetFocusCamera(viewportClient->camera);
}

void UEditorManager::SplitHorizontalSWindow(std::shared_ptr<SWindow>& window) {
	if ( window->child != nullptr )
		return;
	if ( std::dynamic_pointer_cast<SSplitterH>(window) != nullptr ||
		std::dynamic_pointer_cast<SSplitterV>(window) != nullptr)
		return;

	UWorld* world = UEngine::Get().GetWorld();
	FViewportClient* viewportClientLeft = world->AddViewportClient(
		FRect(
			window->Rect.Left,
			window->Rect.Top,
			(window->Rect.Left + window->Rect.Right) / 2.f,
			window->Rect.Bottom
		)
	);
	FViewportClient* viewportClientRight = world->AddViewportClient(
		FRect(
			(window->Rect.Left + window->Rect.Right) / 2.f,
			window->Rect.Top,
			window->Rect.Right,
			window->Rect.Bottom
		)
	);
	FTransform cameraTransfrom = std::dynamic_pointer_cast<SWorldWindow>(window)->GetViewportClient()->camera->GetActorTransform();
	viewportClientLeft->camera->SetActorTransform(cameraTransfrom);
	viewportClientRight->camera->SetActorTransform(cameraTransfrom);

	FRect rect = window->Rect;
	std::shared_ptr<SSplitter> parentSplitter = std::dynamic_pointer_cast<SSplitter>(window->parent);

	if (parentSplitter == nullptr) {
		// delete process
		window.reset();
		RootWindow.reset();

		// replace process
		RootWindow = std::make_shared<SWindow>();
		RootWindow->Rect = rect;

		// split
		std::shared_ptr<SSplitterH> splitter = std::make_shared<SSplitterH>(
			std::make_shared<SWorldWindow>(viewportClientLeft),
			std::make_shared<SWorldWindow>(viewportClientRight)
		);

		splitter->SideLT->parent = splitter;
		splitter->SideRB->parent = splitter;
		
		RootWindow->child = splitter;
		splitter->parent = RootWindow;

		SelectedWindow = splitter->SideLT;
		world->SetFocusCamera(viewportClientLeft->camera);

	} else {
		// delete and replace process
		std::shared_ptr<SWindow> replace;
		if ( window == parentSplitter->SideLT ) {
			parentSplitter->SideLT.reset();
			parentSplitter->SideLT = std::make_shared<SWindow>();
			replace = parentSplitter->SideLT;
		}
		else {
			parentSplitter->SideRB.reset();
			parentSplitter->SideRB = std::make_shared<SWindow>();
			replace = parentSplitter->SideRB;
		}
		window.reset();
		replace->Rect = rect;
		
		// split
		std::shared_ptr<SSplitterH> splitter = std::make_shared<SSplitterH>(
			std::make_shared<SWorldWindow>(viewportClientLeft),
			std::make_shared<SWorldWindow>(viewportClientRight)
		);
		splitter->SideLT->parent = splitter;
		splitter->SideRB->parent = splitter;

		replace->parent = parentSplitter;
		replace->child = splitter;
		splitter->parent = replace;

		SelectedWindow = splitter->SideLT;
		world->SetFocusCamera(viewportClientLeft->camera);
	}
}

void UEditorManager::SplitVerticalSWindow(std::shared_ptr<SWindow>& window) {
	if ( window->child != nullptr )
		return;
	if ( std::dynamic_pointer_cast<SSplitterH>(window) != nullptr ||
		std::dynamic_pointer_cast<SSplitterV>(window) != nullptr )
		return;

	UWorld* world = UEngine::Get().GetWorld();
	FViewportClient* viewportClientTop = world->AddViewportClient(
		FRect(
			window->Rect.Left,
			window->Rect.Top,
			window->Rect.Right,
			(window->Rect.Top + window->Rect.Bottom) / 2.f
		)
	);
	FViewportClient* viewportClientBottom = world->AddViewportClient(
		FRect(
			window->Rect.Left,
			(window->Rect.Top + window->Rect.Bottom) / 2.f,
			window->Rect.Right,
			window->Rect.Bottom
		)
	);
	FTransform cameraTransfrom = std::dynamic_pointer_cast<SWorldWindow>(window)->GetViewportClient()->camera->GetActorTransform();
	viewportClientTop->camera->SetActorTransform(cameraTransfrom);
	viewportClientBottom->camera->SetActorTransform(cameraTransfrom);

	FRect rect = window->Rect;
	std::shared_ptr<SSplitter> parentSplitter = std::dynamic_pointer_cast<SSplitter>(window->parent);

	if ( parentSplitter == nullptr ) {
		// delete process
		window.reset();
		RootWindow.reset();

		// replace process
		RootWindow = std::make_shared<SWindow>();
		RootWindow->Rect = rect;

		// split
		std::shared_ptr<SSplitterV> splitter = std::make_shared<SSplitterV>(
			std::make_shared<SWorldWindow>(viewportClientTop),
			std::make_shared<SWorldWindow>(viewportClientBottom)
		);
		splitter->SideLT->parent = splitter;
		splitter->SideRB->parent = splitter;

		RootWindow->child = splitter;
		splitter->parent = RootWindow;

		SelectedWindow = splitter->SideLT;
		world->SetFocusCamera(viewportClientTop->camera);

	} else {
		// delete and replace process
		std::shared_ptr<SWindow> replace;
		if ( window == parentSplitter->SideLT ) {
			parentSplitter->SideLT.reset();
			parentSplitter->SideLT = std::make_shared<SWindow>();
			replace = parentSplitter->SideLT;
		} else {
			parentSplitter->SideRB.reset();
			parentSplitter->SideRB = std::make_shared<SWindow>();
			replace = parentSplitter->SideRB;
		}
		window.reset();
		replace->Rect = rect;

		// split
		std::shared_ptr<SSplitterV> splitter = std::make_shared<SSplitterV>(
			std::make_shared<SWorldWindow>(viewportClientTop),
			std::make_shared<SWorldWindow>(viewportClientBottom)
		);
		splitter->SideLT->parent = splitter;
		splitter->SideRB->parent = splitter;

		replace->parent = parentSplitter;
		replace->child = splitter;
		splitter->parent = replace;

		SelectedWindow = splitter->SideLT;
		world->SetFocusCamera(viewportClientTop->camera);
	}
}

void UEditorManager::RemoveSWindow(std::shared_ptr<SWindow>& window) {
	if ( window->parent == nullptr )
		return;
	if ( window->child != nullptr )
		return;

	SSplitter* p = dynamic_cast<SSplitter*>(window->parent.get());
	std::shared_ptr<SWindow> sibiling;
	if (p->SideLT == window)
		sibiling = p->SideRB;
	else
		sibiling = p->SideLT;

	SWindow* pp = p->parent.get();
	FRect ppRect = pp->Rect;
	sibiling->Rect = ppRect;
	sibiling->OnResizeUpdate();

	SSplitter* ppp = dynamic_cast<SSplitter*>(pp->parent.get());
	
	{
		p->SideLT->parent.reset();
		p->SideRB->parent.reset();
		p->SideLT.reset();
		p->SideRB.reset();
		p->parent.reset();
		pp->child.reset();
	}

	if (pp->parent != nullptr) {

		sibiling->parent = pp->parent;

		if ( ppp->SideLT.get() == pp ) {
			ppp->SideLT.reset();
			ppp->SideLT = sibiling;
		} else {
			ppp->SideRB.reset();
			ppp->SideRB = sibiling;
		}
	} else {
		sibiling->parent = nullptr;
		RootWindow.reset();
		RootWindow = sibiling;
	}
	SelectedWindow.reset();
	SelectedWindow = sibiling;


	std::function<SWindow*(SWindow*)> findLeftest = [&](SWindow* window)->SWindow* {
		if ( window->child == nullptr )
			return window;
		return findLeftest(window->child->SideLT.get());
	};
	SWorldWindow* leftest = dynamic_cast<SWorldWindow*>(findLeftest(RootWindow.get()));
	UEngine::Get().GetWorld()->SetFocusCamera(leftest->GetViewportClient()->camera);

}

void UEditorManager::SelectActor(AActor* NewActor)
{
    if (Gizmo == nullptr)
    {
		Gizmo = UEngine::Get().GetWorld()->SpawnActor<AGizmoActor>();
		Gizmo->SetDepth(1);
    }
	if (NewActor == nullptr)
	{
		Gizmo->SetActorHiddenInGame(true);
		Gizmo->SetTickFunctionEnable(false);
	}
	else
	{
		Gizmo->SetActorHiddenInGame(false);
		Gizmo->SetTickFunctionEnable(true);
	}

	if (SelectedActor == NewActor)
		return;
	
    if (SelectedActor != nullptr && SelectedActor != NewActor)
    {
        SelectedActor->UnPick();
    }

	SelectedActor = NewActor;
	
    if (SelectedActor != nullptr)
    {
        SelectedActor->Pick();
		    const FTransform newActorTransform = NewActor->GetActorTransform();
		    Gizmo->SetActorTransform(newActorTransform);
	}

}

std::shared_ptr<SWindow> UEditorManager::GetClickedWindow(
	const FVector& InMouseScreenPos,
	const std::shared_ptr<SWindow> InSWindow
) {
	if ( !InSWindow->IsHover(FVector2D(InMouseScreenPos.X, InMouseScreenPos.Y)) )
		return nullptr;
	if ( InSWindow->child == nullptr ) {
		return InSWindow;
	} else {
		std::shared_ptr<SWindow> childLT = InSWindow->child->SideLT;
		std::shared_ptr<SWindow> childRB = InSWindow->child->SideRB;
		std::shared_ptr<SWindow> resLT = GetClickedWindow(InMouseScreenPos, childLT);
		std::shared_ptr<SWindow> resRB = GetClickedWindow(InMouseScreenPos, childRB);
		if ( resLT != nullptr )
			return resLT;
		else if ( resRB != nullptr )
			return resRB;
		else
			return nullptr;
	}
}

//void UEditorManager::RenderWindow(const std::shared_ptr<SWindow> InSWindow) {
//	if ( InSWindow->child == nullptr ) {
//		SWorldWindow* camWindow = dynamic_cast<SWorldWindow*>(InSWindow.get());
//		camWindow->GetViewportClient()->
//		/*camWindow->GetCamera()->UpdateCameraMatrix();
//		camWindow->GetCamera()->SettingViewport();*/
//	}
//}

void UEditorManager::SetGizmo(AGizmoActor* InGizmo)
{
	Gizmo = InGizmo;
}

FVector4 UEditorManager::EncodeUUID(uint32 UUID)
{
	const uint32 a = (UUID >> 24) & 0xff;
	const uint32 b = (UUID >> 16) & 0xff;
	const uint32 g = (UUID >> 8) & 0xff;
	const uint32 r = UUID & 0xff;

	const FVector4 color = {
		static_cast<float>(r),
		static_cast<float>(g),
		static_cast<float>(b),
		static_cast<float>(a)
	};
    
	return color;
}

uint32 UEditorManager::DecodeUUID(FVector4 color)
{
	return (
		static_cast<uint32>(color.W) << 24
		| (static_cast<uint32>(color.Z) << 16)
		| (static_cast<uint32>(color.Y) << 8)
		| (static_cast<uint32>(color.X))
	);
}

void UEditorManager::LateTick([[maybe_unused]] float DeltaTime)
{
	if (UEngine::Get().GetInput()->GetKeyDown(EKeyCode::LButton))
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(UEngine::Get().GetWindowHandle(), &pt);

		//float Width = FDevice::Get().GetViewPortInfo().Width;
		//float SplitterHeight = FDevice::Get().GetViewPortInfo().SplitterHeight;

		//      float ratioX = UEngine::Get().GetInitializedScreenWidth() / Width;
		//      float ratioY = UEngine::Get().GetInitializedScreenHeight() / SplitterHeight;
		//      pt.x = pt.x * ratioX;
		//      pt.y = pt.y * ratioY;

		const FVector4 color = GetPixel(FVector(pt.x, pt.y, 0));
		const uint32_t UUID = DecodeUUID(color);

		UActorComponent* PickedComponent = UEngine::Get().GetObjectByUUID<UActorComponent>(UUID);

		if (PickedComponent != nullptr)
		{
			// Component의 Owner도 Engine.GObjects에서 관리되기에, Component가 존재한다면 항상 존재 해야함
			AActor* PickedActor = PickedComponent->GetOwner();
			assert(PickedActor);

			// if (GetOwner()->Implements<IGizmoInterface>() == false) // TODO: RTTI 개선하면 사용
			if (!dynamic_cast<IGizmoInterface*>(PickedActor))
			{
				// PickedActor를 한번 더 클릭하면 UnPicked
				SelectActor(PickedActor);
			}
				
			
			UE_LOG("Pick - UUID: %d", UUID);

			if (const UGizmoComponent* GizmoCom = Cast<UGizmoComponent>(PickedComponent))
			{
				Gizmo->SetSelectedAxis(GizmoCom->GetSelectedAxis());
			}
		}
		else
		{
			//SelectActor(nullptr);
		}
	}

	//if (UInputManager::Get().GetKeyPress(EKeyCode::LButton))
	//{
	//	if (SelectedActor != nullptr)
	//	{
	//		if (AGizmoActor* Gizmo = Cast<AGizmoActor>(SelectedActor))
	//		{
	//			//Gizmo->SetSelectedAxis(ESelectedAxis::Y);


	//			//FVector MousePos = UInputManager::Get().GetMouseScreenDeltaPos();

	//			//FVector Dir = FVector{ 0.0f, MousePos.X, MousePos.Y } *0.1f;

	//			//Gizmo->AddActorLocalOffset(Dir);



	//			//if (Gizmo->GetSelectedAxis() != ESelectedAxis::None) return;
	//			//UCylinderComp* CylinderComp = static_cast<UCylinderComp*>(PickedComponent);
	//			//FVector4 CompColor = CylinderComp->GetCustomColor();
	//			//if (1.0f - FMath::Abs(CompColor.X) < KINDA_SMALL_NUMBER) // Red - X축
	//			//{
	//			//    Gizmo->SetSelectedAxis(ESelectedAxis::X);
	//			//}
	//			//else if (1.0f - FMath::Abs(CompColor.Y) < KINDA_SMALL_NUMBER) // Green - Y축
	//			//{
	//			//    Gizmo->SetSelectedAxis(ESelectedAxis::Y);
	//			//}
	//			//else  // Blue - Z축
	//			//{
	//			//    Gizmo->SetSelectedAxis(ESelectedAxis::Z);
	//			//}
	//		}
	//	}
	//}
	//else
	//{
	//	// if (AGizmoHandle* Handle = UEditorManager::Get().GetGizmoHandle())
	//	// {
	//	//     Handle->SetSelectedAxis(ESelectedAxis::None);
	//	// }
	//}
		 
}

void UEditorManager::OnUpdateWindowSize(uint32 Width, uint32 Height)
{
	if (Width == 0 || Height == 0)
	{
		return;
	}

	if (UUIDTexture != nullptr)
	{
		UTexture::Release("UUIDTexture");
		UUIDTexture = nullptr;
	}

	//D3D11_TEXTURE2D_DESC textureDesc = {};
	//textureDesc.Width = Width;
	//textureDesc.SplitterHeight = SplitterHeight;
	//textureDesc.MipLevels = 1;
	//textureDesc.ArraySize = 1;
	//textureDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT; // 32비트 UINT로 변경
	//textureDesc.SampleDesc.Count = 1;
	//textureDesc.Usage = D3D11_USAGE_DEFAULT;
	//textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	//UUIDTexture = UTexture::Create("UUIDTexture", textureDesc);
	//UUIDTexture->CreateRenderTargetView();
}

void UEditorManager::OnResizeComplete()
{
	CreateUUIDTexture();
}

FVector4 UEditorManager::GetPixel(FVector MPos) const
{

	const float Width = FDevice::Get().GetViewPortInfo().Width;
	const float Height = FDevice::Get().GetViewPortInfo().Height;
    MPos.X = FMath::Clamp(MPos.X, 0.0f, Width);
    MPos.Y = FMath::Clamp(MPos.Y, 0.0f, Height);
    // 1. Staging 텍스처 생성 (1x1 픽셀)
    D3D11_TEXTURE2D_DESC stagingDesc = {};
    stagingDesc.Width = 1; // 픽셀 1개만 복사
    stagingDesc.Height = 1;
    stagingDesc.MipLevels = 1;
    stagingDesc.ArraySize = 1;
    stagingDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT; // 원본 텍스처 포맷과 동일
    stagingDesc.SampleDesc.Count = 1;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

    ID3D11Texture2D* stagingTexture = nullptr;
    FDevice::Get().GetDevice()->CreateTexture2D(&stagingDesc, nullptr, &stagingTexture);

    // 2. 복사할 영역 정의 (D3D11_BOX)
    D3D11_BOX SrcBox;
    SrcBox.left = static_cast<UINT>(MPos.X);
    SrcBox.right = SrcBox.left + 1; // 1픽셀 너비
    SrcBox.top = static_cast<UINT>(MPos.Y);
    SrcBox.bottom = SrcBox.top + 1; // 1픽셀 높이
    SrcBox.front = 0;
    SrcBox.back = 1;
    FVector4 color {1, 1, 1, 1};

    if (stagingTexture == nullptr)
        return color;

    // 3. 특정 좌표만 복사
    FDevice::Get().GetDeviceContext()->CopySubresourceRegion(
        stagingTexture, // 대상 텍스처
        0,              // 대상 서브리소스
        0, 0, 0,        // 대상 좌표 (x, y, z)
        UUIDTexture->GetTexture2D(), // 원본 텍스처
        0,              // 원본 서브리소스
        &SrcBox         // 복사 영역
    );

    // 4. 데이터 매핑
    D3D11_MAPPED_SUBRESOURCE mapped = {};
    FDevice::Get().GetDeviceContext()->Map(stagingTexture, 0, D3D11_MAP_READ, 0, &mapped);

    // 5. 픽셀 데이터 추출 (1x1 텍스처이므로 offset = 0)
    const BYTE* pixelData = static_cast<const BYTE*>(mapped.pData);

    if (pixelData)
    {
        color.X = static_cast<float>(pixelData[0]); // R
        color.Y = static_cast<float>(pixelData[1]); // G
        color.Z = static_cast<float>(pixelData[2]); // B
        color.W = static_cast<float>(pixelData[3]); // A
    }

    std::cout << "X: " << (int)color.X << " Y: " << (int)color.Y 
              << " Z: " << color.Z << " A: " << color.W << "\n";

    // 6. 매핑 해제 및 정리
    FDevice::Get().GetDeviceContext()->Unmap(stagingTexture, 0);
    stagingTexture->Release();

    return color;
}
