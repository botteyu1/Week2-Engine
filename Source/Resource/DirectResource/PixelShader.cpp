#include "PixelShader.h"

#include <d3dcompiler.h>
#include "Core/Rendering/FDevice.h"
#include "Debug/DebugConsole.h"

UPixelShader::UPixelShader()
{
}

UPixelShader::~UPixelShader()
{
	if (nullptr != ShaderPtr)
	{
		ShaderPtr->Release();
		ShaderPtr = nullptr;
	}
}

void UPixelShader::Setting()
{
	if (nullptr == ShaderPtr)
	{
		MsgBoxAssert("Error: FPixelShader Setting Failed");
	}
	
	FDevice::Get().GetDeviceContext()->PSSetShader(ShaderPtr, nullptr, 0);
}

void UPixelShader::ShaderLoad(const LPCWSTR& _Path, const FString& _EntryPoint, UINT _VersionHight, UINT _VersionLow)
{
	ID3DBlob* Error = nullptr;
	DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;

	
	SetShaderType(ShaderType::Pixel);
	
	HRESULT hr = D3DCompileFromFile(_Path, nullptr, nullptr, *_EntryPoint, "ps_5_0", shaderFlags, 0, &BinaryCode, &Error);
	FDevice::Get().GetDevice()->CreatePixelShader(BinaryCode->GetBufferPointer(), BinaryCode->GetBufferSize(), nullptr, &ShaderPtr);
}
