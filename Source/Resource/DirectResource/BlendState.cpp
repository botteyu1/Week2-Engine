#include "BlendState.h"

#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"
#include "Debug/DebugConsole.h"


UBlendState::UBlendState()
{
}

UBlendState::~UBlendState()
{
	if (nullptr != State)
	{
		State->Release();
		State = nullptr;
	}
}

void UBlendState::Setting()
{
	if (nullptr == State)
	{
		UE_LOG("Error: BlendState Setting Failed");
	}

	FDevice::Get().GetDeviceContext()->OMSetBlendState(State, nullptr, Mask);
}

void UBlendState::ResCreate(const D3D11_BLEND_DESC& _Desc)
{
	Desc = _Desc;
	if (S_OK != FDevice::Get().GetDevice()->CreateBlendState(&_Desc, &State))
	{
		UE_LOG("Error: BlendState Setting Failed");
	}
}
