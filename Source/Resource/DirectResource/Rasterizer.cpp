#include "Rasterizer.h"
#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"
#include "Debug/DebugConsole.h"
#include "Core/Engine.h"
#include "Resource/DirectResource/ViewMode.h"


FRasterizer::FRasterizer()
{
}

FRasterizer::~FRasterizer()
{
	if (nullptr != State)
	{
		State->Release();
		State = nullptr;
	}
}

void FRasterizer::Setting()
{
	if (nullptr == State)
	{
		UE_LOG("Error: FRasterizer Setting Failed");
	}
	FDevice::Get().GetDeviceContext()->RSSetState(State);
}

void FRasterizer::ResCreate(const D3D11_RASTERIZER_DESC& _Desc)
{
	Desc = _Desc;
	if (S_OK != FDevice::Get().GetDevice()->CreateRasterizerState(&_Desc, &State))
	{
		UE_LOG("Error: FRasterizer Setting Failed");
	}
}
