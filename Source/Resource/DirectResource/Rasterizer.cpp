#include "Rasterizer.h"
#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"
#include "Debug/DebugConsole.h"
#include "Core/Engine.h"
#include "Resource/DirectResource/ViewMode.h"


URasterizer::URasterizer()
{
}

URasterizer::~URasterizer()
{
	if (nullptr != State)
	{
		State->Release();
		State = nullptr;
	}
}

void URasterizer::Setting()
{
	if (nullptr == State)
	{
		UE_LOG("Error: FRasterizer Setting Failed");
	}
	FDevice::Get().GetDeviceContext()->RSSetState(State);
}

void URasterizer::ResCreate(const D3D11_RASTERIZER_DESC& _Desc)
{
	Desc = _Desc;
	if (S_OK != FDevice::Get().GetDevice()->CreateRasterizerState(&_Desc, &State))
	{
		UE_LOG("Error: FRasterizer Setting Failed");
	}
}
