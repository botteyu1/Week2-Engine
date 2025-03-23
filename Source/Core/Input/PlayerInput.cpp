#include "PlayerInput.h"
#include "Core/Rendering/FViewport.h"
#include "Object/World/World.h"

[[maybe_unused]]
static FVector GetWindowSize(HWND hWnd)
{
    RECT Rect;
    int Width = 0, Height = 0;
    if (GetClientRect(hWnd, &Rect))
    {
        Width = Rect.right - Rect.left;
        Height = Rect.bottom - Rect.top;
    }

    return FVector(Width, Height, 0.0f);
}


UInputManager::UInputManager()
{
	CreateKeys();

	MousePreNDCPos = FVector(0.0f, 0.0f, 0.0f);

	MousePos = FVector(0.0f, 0.0f, 0.0f);
	MousePrePos = FVector(0.0f, 0.0f, 0.0f);
	MouseNDCPos = FVector(0.0f, 0.0f, 0.0f);
}

void UInputManager::UpdateKey(FKey& key)
{
	if (GetFocus())
	{
		if (IsKeyDown(key.KeyCode))
		{
			UpdateKeyDown(key);
		}
		else
		{
			UpdateKeyUp(key);
		}
	}
	else
	{
		ClearKeys();
	}
}

void UInputManager::UpdateKeyDown(FKey& key) const
{
	if (key.bPressed == true)
	{
		key.KeyState = EKeyState::Press;
	}
	else
	{
		key.KeyState = EKeyState::Down;
	}

	key.bPressed = true;
}

void UInputManager::UpdateKeyUp(FKey& key) const
{
	if (key.bPressed == true)
	{
		key.KeyState = EKeyState::Up;
	}
	else
	{
		key.KeyState = EKeyState::None;
	}

	key.bPressed = false;
}

void UInputManager::CreateKeys()
{
	for (int vk = 0; vk <= 0xFF; ++vk)
	{
		Keys.Emplace(FKey{
			.KeyCode = static_cast<EKeyCode>(vk),
			.KeyState = EKeyState::None,
			.bPressed = false
		});
	}
}

void UInputManager::ClearKeys()
{
	for (FKey& Key : Keys)
	{
		if (Key.KeyState == EKeyState::Down || Key.KeyState == EKeyState::Press)
		{
			Key.KeyState = EKeyState::Up;

		}
		else if (Key.KeyState == EKeyState::Up)
		{
			Key.KeyState = EKeyState::None;
		}

		Key.bPressed = false;
	}
}

bool UInputManager::IsKeyDown(EKeyCode code) const
{
	return GetAsyncKeyState(static_cast<int>(code)) & 0x8000;
}

void UInputManager::SetMousePos(HWND hWnd, uint32 FrameBufferWidth, uint32 FrameBufferHeight)
{
	MousePreNDCPos = MouseNDCPos;
	MousePrePos = MousePos;
	POINT Pts = {};
	GetCursorPos(&Pts);

	ScreenToClient(hWnd, &Pts);

	MousePos = FVector(Pts.x, Pts.y, 0);
	MouseNDCPos = CalNDCPos(MousePos, FVector(FrameBufferWidth, FrameBufferHeight, 0));
}

void UInputManager::Update(HWND hWnd, uint32 FramaeBufferWidth, uint32 FramaeBufferHeight)
{
	for (FKey& key : Keys)
	{
		UpdateKey(key);
	}		
	SetMousePos(hWnd, FramaeBufferWidth, FramaeBufferHeight);


	for (const auto& [Key, Callbacks] : KeyDownCallbacks)
	{
		if (GetKeyDown(Key))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback();
			}
		}
	}

	for (const auto& [Key,Callbacks] : KeyPressCallbacks)
	{
		if (GetKeyPress(Key))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback();
			}
		}
	}

	for (const auto& [Key, Callbacks] : KeyUpCallbacks)
	{
		if (GetKeyUp(Key))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback();
			}
		}
	}

	for (const auto& [button, Callbacks] : MouseDownCallbacks)
	{
		if (GetKeyDown(button))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback(MouseNDCPos);
			}
		}
	}

	for (const auto& [button, Callbacks] : MousePressCallbacks)
	{
		if (GetKeyPress(button))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback(GetMouseDeltaPos());
			}
		}
	}

	for (const auto& [button, Callbacks] : MouseUpCallbacks)
	{
		if (GetKeyUp(button))
		{
			for (const auto& Callback : Callbacks)
			{
				Callback(MouseNDCPos);
			}
		}
	}

}

FVector UInputManager::CalNDCPos(FVector InMousePos, FVector WindowSize) const
{
    return { (2.0f * InMousePos.X) / WindowSize.X - 1.0f,  (-2.0f * InMousePos.Y) / WindowSize.Y + 1.0f, 0};
}

void UInputManager::GetNDCPosWithSplitViewPort(
	const FVector InMousePos, 
	const TMap<EViewPortSplitter, FViewport> InViewPorts,
	FVector& OutMouseNDCPos,
	EViewPortSplitter& OutSelectedViewPortIndex
	) const {
	
	float screenPosX = InMousePos.X;
	float screenPosY = InMousePos.Y;
	UE_LOG("MousePos In Screen : %f, %f", screenPosX, screenPosY);
	OutSelectedViewPortIndex = EViewPortSplitter::None;
	for ( const auto& pair: InViewPorts ) {
		FViewport viewport = pair.Value;
		D3D11_VIEWPORT viewportInfo = viewport.GetViewportInfo();
		if (
			viewport.IsHover(FVector2D(screenPosX, screenPosY))
			//(viewportInfo.TopLeftX < InMousePos.X && InMousePos.X < viewportInfo.TopLeftX + viewportInfo.Width) &&
			//(viewportInfo.TopLeftY < InMousePos.Y && InMousePos.Y < viewportInfo.TopLeftY + viewportInfo.Height)
		) {
			OutSelectedViewPortIndex = pair.Key;
			OutMouseNDCPos.X = 2.0f * (InMousePos.X - viewportInfo.TopLeftX) / viewportInfo.Width - 1.0f;
			OutMouseNDCPos.Y = - 2.0f * (InMousePos.Y - viewportInfo.TopLeftY) / viewportInfo.Height + 1.0f;
			OutMouseNDCPos.Z = 0.f;
			UE_LOG("MousePos In NDC : %f, %f", OutMouseNDCPos.X, OutMouseNDCPos.Y);
		}
	}
}